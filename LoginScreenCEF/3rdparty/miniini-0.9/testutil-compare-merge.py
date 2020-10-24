#!/usr/bin/python3

# Copyright (C) 2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import sys
import getopt
import os

def help():
    print("Script to merge outputs (comparisons) of testutil-compare.py")
    print("Copyright (C) 2010 Ferdinand Majerech")
    print("Usage: testutil-compare-merge.py [OPTIONS] [ARGUMENTS]")
    print(" -h --help            display this help and exit")
    print(" -o --output      val file to output merged comparisons to")
    print("                      if not specified, output is printed to stdout")
    print("ARGUMENTS must be comparisons to merge.")

#Stores comparisons of same data type from all inputs
class Merged:
    def __init__(self, line):
        #each comparison is a list, first element of which is name of the
        #input to which the comparison belongs, other are values compared
        self.__comparisons = []
        #name of data compared in the comparisons
        #(e.g. cachegrind|Relative|data_refs)
        self.__data_name = line.partition(" ")[0]

    def add_comparison(self, name, line):
        self.__comparisons.append([name] + line.split()[1:])

    #format for nicer printing
    def __format(self):
        #widths of each column in the comparisons (first column are input names,
        #others are values compared)
        column_widths = []
        #get widths required for each column
        for comp in self.__comparisons:
            for column_index in range(0, len(comp)):
                column_width = len(comp[column_index])
                if column_index >= len(column_widths):
                    column_widths.append(column_width)
                else:
                    if column_width > column_widths[column_index]:
                        column_widths[column_index] = column_width
        #pad elements of each comparison with spaces to get straight columns
        for comp_index in range(0, len(self.__comparisons)):
            for column_index in range(0, len(comp)):
                column = self.__comparisons[comp_index][column_index]
                column = column + " " * (column_widths[column_index] - len(column))
                self.__comparisons[comp_index][column_index] = column

    #get a printable string representation of merged comparisons
    def get_string(self):
        self.__format()
        out = []
        #write data name
        out.append(self.__data_name + ":\n")
        #underline the data name for better readability
        out.append("-" * (len(out[-1]) - 1) + "\n")
        #write comparisons with their inputs' names and values
        for comp in self.__comparisons:
            out.append(" ".join(comp) + "\n")
        return "".join(out)

    #equality operator with a string so we can use the in operator and list search
    def __eq__(self, line):
        return self.__data_name == line.partition(" ")[0]

#handles merging of multiple comparison files output by testutil-compare.py
class Merge:
    def __init__(self, comparisons, output):
        #filenames of comparison files
        self.__comparisons = comparisons
        #merged data comparisons will be stored here (e.g. cachegrind data refs)
        self.__merged = []
        #process input files and merge them
        for comp in comparisons:
            self.__add_comparison(comp)
        #write out the result
        self.__write(output)

    #process an input file and merge it to already merged comparisons
    def __add_comparison(self, comp):
        #relative or absolute
        value_type = "unknown"
        #memcheck, callgrind, etc.
        section = "unknown"
        with open(comp, "r") as comp_file:
            for line in comp_file:
                #ignore comments
                if line[0] in (";", "#"):
                    continue
                #value type or section headers
                elif line[0] == "=":
                    if line[1] == "=":
                        section = line[2:-3]
                    else:
                        value_type = line[1:-2]
                #ignore empty lines, non-empty ones will be merged
                elif (len(line) > 0) and (not line.isspace()):
                    line = section + "|" + value_type + "|" + line
                    #if we don't have this data type yet, add it
                    if not line in self.__merged:
                        self.__merged.append(Merged(line))
                    #merge the comparison
                    merged_index = self.__merged.index(line)
                    self.__merged[merged_index].add_comparison(comp, line)

    def __write(self, output):
        #if no output file specified, write out to stdout
        if output == None:
            print(";merged test comparisons: " + " ".join(self.__comparisons))
            for merged in self.__merged:
                print (merged.get_string())
        #write to file
        else:
            with open(output, "w") as out_file:
                out_file.write(";merged test comparisons: " +
                               " ".join(self.__comparisons) + "\n")
                for merged in self.__merged:
                    out_file.write(merged.get_string() + "\n")

def main():
    output = None
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ho:",
                     ["help", "output="])
    except getopt.GetoptError:
        help()
        sys.exit(1)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        if opt in ("-o", "--output"):
            output = arg

    if len(args) < 2:
        print ("Need at least 2 arguments: comparisons to merge.")
        sys.exit(2)
    Merge(args, output)

if __name__ == '__main__':
    main()
