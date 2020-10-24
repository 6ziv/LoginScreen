#!/usr/bin/python3

# Copyright (C) 2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import subprocess
import sys
import getopt
import configparser
import os

def help():
    print("Script to compare outputs (summaries) of testutil.py")
    print("Copyright (C) 2010 Ferdinand Majerech")
    print("Usage: testutil-compare.py [OPTIONS] [ARGUMENTS]")
    print(" -h --help            display this help and exit")
    print(" -o --output      val file to output comparison results to")
    print("                      if not specified, output is printed to stdout")
    print(" -N --no-absolute     don't output absolute values")
    print(" -r --relative        output relative values")
    print("ARGUMENTS must be summaries to compare.")

#strip unnecessary characters from a float string (such as trailing zeroes)
def strip_float(string):
    str_list = list(string)
    while str_list[-1] == "0":
        str_list = str_list[:-1]
    if str_list[-1] == ".":
        str_list = str_list[:-1]
    return "".join(str_list)

#single line in output; a comparison of some value in all inputs
class Comparison:
    def __init__(self, name, size):
        #float values in all inputs - for inputs that don't contain this value,
        #None is used
        self.__values = []
        #Name of the compared value (e.g. instruction_refs)
        self.__name = name
        #Default name width for formatting
        self.__name_width = len(name) + 3
        #Default value width for formatting
        self.__column_width = 6
        #None used as a placeholder
        for val in range(0, size):
            self.__values.append(None)

    #set value at index
    def set_value(self, index, value):
        self.__values[index] = float(value)

    #get a printable, formatted string of the comparison
    #if relative is true, use percentage values instead of absolute
    def get_string(self, relative = False):
        out = [self.__name]
        if relative:
            out = [self.__name + "_%"]
        #insert spaces to get straight column
        out.append(" " * (self.__name_width - len(out[-1])))
        first = True
        percent = None
        for val in self.__values:
            #no value for this input
            if val is None:
                out.append("N/A")
            elif relative:
                #first available value is a base (100%) for relative comparison
                if first:
                    percent = val / 100.0
                    first = False
                    out.append("100%")
                else:
                    #prevent zero division errors
                    if percent == 0:
                        if val == 0:
                            out.append("100%")
                        elif val < 0:
                            out.append("-inf%")
                        elif val > 0:
                            out.append("inf%")
                    else:
                        #at most 3 decimal places for percentages
                        val_str = "%.3f" % (val / percent)
                        out.append(strip_float(val_str) + "%")
            else:
                #at most 5 decimal places for absolute values
                val_str = "%.5f" % (val)
                out.append(strip_float(val_str))
            out.append(" " * (self.__column_width - len(out[-1])))
        #insert spaces to get straight columns
        return "".join(out)

    def get_name(self):
        return self.__name

    #formatting methods:

    def get_name_width(self):
        #2 chars for _% , 1 char for space
        return len(self.__name) + 3

    def set_name_width(self, width):
        self.__name_width = width

    def get_column_width(self, percent):
        column_width = None
        if(percent):
            #-inf% + space
            column_width = 6
            percent = None
            first = True
            for val in self.__values:
                if val is None:
                    continue
                elif first:
                    percent = val / 100.0
                    first = False
                elif percent != 0:
                    #stripped number + % + space
                    val_width_percent = len(strip_float("%.3f"%(val/percent)))+2
                    if val_width_percent > column_width:
                        column_width = val_width_percent
        else:
            #N/A + space
            column_width = 4
            for val in self.__values:
                val_width = len(strip_float("%.5f" % (val))) + 1
                if val_width > column_width:
                    column_width = val_width
        return column_width

    def set_column_width(self, width):
        self.__column_width = width


    #equality operator with a string so we can use in operator
    def __eq__(self, string):
        return string == self.__name


#a section containing related comparisons- corresponds to sections in
#summaries compared.
class Section:
    def __init__(self, name, size):
        #number of values per comparison (number of compared inputs)
        self.__comp_size = size
        #name of this section
        self.__name = name
        #comparisons in this section (corresponding to values in inputs)
        self.__comparisons = []

    #index specifies which one of input files we're processing.
    def add_comparisons(self, index, ini):
        for tag, val in ini.items(self.__name):
            #if this comparison doesn't exist yet, add it
            if not tag in self.__comparisons:
                self.__comparisons.append(Comparison (tag, self.__comp_size))
            #add the value from this input to the comparison
            comp_index = self.__comparisons.index(tag)
            self.__comparisons[comp_index].set_value(index, float(val))

    #set format of the comparisons (really brute force...)
    def __format(self, percent):
        name_width = 0
        column_width = 0
        for comp in self.__comparisons:
            comp_name_width = comp.get_name_width()
            comp_column_width = comp.get_column_width(percent)
            if comp_name_width > name_width:
                name_width = comp_name_width
            if comp_column_width > column_width:
                column_width = comp_column_width
        for comp in self.__comparisons:
            comp.set_name_width(name_width)
            comp.set_column_width(column_width)

    #get a formatted, printable string representation of this section
    #if relative is true, use percentage values instead of absolute
    def get_string(self, relative=False):
        self.__format(relative)
        out = ["==" + self.__name + "==\n"]
        for comp in self.__comparisons:
            out.append(comp.get_string(relative) + "\n")
        return "".join(out)


    #equality operator with a string so we can use in operator
    def __eq__(self, string):
        return string == self.__name

#high-level comparison class
class Compare:
    def __init__(self, summaries, output, absolute, percent):
        self.__sections = []
        #filenames of summaries compared
        self.__summaries = summaries
        self.__compare()
        self.__write(output, absolute, percent)

    def __compare(self):
        for summary in range(0, len(self.__summaries)):
            self.__add_comparisons(summary)

    def __add_comparisons(self, index):
        with open(self.__summaries[index], "r") as infile:
            #read using configparser
            ini = configparser.ConfigParser()
            ini.readfp(infile)
            for section in ini.sections():
                #if this section doesn't exist yet, add it
                if not section in self.__sections:
                    self.__sections.append(Section(section,
                                                   len(self.__summaries)))
                #add values and any new comparisons to the section
                section_index = self.__sections.index(section)
                self.__sections[section_index].add_comparisons(index, ini)

    def __write(self, output, absolute, percent):
        #write to stdout if no output file is specified
        if output == None:
            print(";Comparison of testutil.py summaries: " +
                   " ".join(self.__summaries))
            if(absolute):
                print("\n=Absolute=\n")
                for section in self.__sections:
                    print(section.get_string())
            if(percent):
                print("\n=Relative=\n")
                for section in self.__sections:
                    print(section.get_string(True))
        else:
            #write to output file
            with open(output, "w") as out_file:
                out_file.write(";Comparison of testutil.py summaries: " +
                               " ".join(self.__summaries) + "\n")
                if(absolute):
                    out_file.write("\n=Absolute=\n\n")
                    for section in self.__sections:
                        out_file.write(section.get_string() + "\n")
                if(percent):
                    out_file.write("\n=Relative=\n\n")
                    for section in self.__sections:
                        out_file.write(section.get_string(True) + "\n")

def main():
    output = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ho:Nr",
                     ["help", "output=", "no-absolute", "relative"])
    except getopt.GetoptError:
        help()
        sys.exit(1)

    absolute = True
    percent = False
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        if opt in ("-o", "--output"):
            output = arg
        if opt in ("-N", "--no-absolute"):
            absolute = False
        if opt in ("-r", "--relative"):
            percent = True

    if len(args) < 2:
        print ("Need at least 2 arguments: summaries to compare.")
        sys.exit(2)
    Compare(args, output, absolute, percent)

if __name__ == '__main__':
    main()
