#!/usr/bin/python3

# Copyright (C) 2009-2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import random
import sys
import getopt

valtype = "string"
sections = 128
minarrays = 1
maxarrays = 2
mintags = 16
maxtags = 64
mintaglength = 4
maxtaglength = 16

minstrlength = 3
maxstrlength = 18

minint = -2000000000
maxint = 2000000000

minfloat = -2000000.0
maxfloat = 2000000.0

commentchance = 0
seed = 42

chars = "abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ-"
bools = ["yes", "true", "1", "no", "false", "0"]


ini_buffer = []

def help():
    print ("Random INI file generator")
    print ("Copyright (C) 2010 Ferdinand Majerech")
    print ("Usage: testgen.py [OPTION...] OUTPUT FILE")
    print (" -h --help               display this help and exit")
    print (" -v --val-type       val type of values to fill the file with:")
    print ("                             single values:    string int float bool")
    print ("                             arrays:           strings ints floats bools")
    print ("                             multi value tags: stringm intm floatm boolm")
    print ("                         default: string")
    print (" -s --sections       val number of sections in the file")
    print ("                         default: 128")
    print (" -a --min-arrays     val min number of arrays/multi value tags")
    print ("                         (if applicaple) per section")
    print ("                         default: 1")
    print (" -A --max-arrays     val max number of arrays/multi value tags")
    print ("                         (if applicaple) per section - can be at most")
    print ("                         26 for arrays")
    print ("                         default: 2")
    print (" -t --min-tags       val min number of tags per section/array/multi ")
    print ("                         value tag")
    print ("                         default: 16")
    print (" -T --max-tags       val max number of tags per section/array/multi")
    print ("                         value tag - Can be at most 1000 for multi")
    print ("                         value tags")
    print ("                         default: 64")
    print (" -l --min-tag-length val min length of a tag or section name")
    print ("                         default: 4")
    print (" -L --max-tag-length val max length of a tag or section name")
    print ("                         default: 16")

def random_char():
    return chars [random.randint(0, len(chars) - 1)]

def newline():
    ini_buffer.append("\n")

def comment(comment):
    ini_buffer.append(";" + comment + "\n")

def header(header):
    ini_buffer.append("[" + header + "]\n")

def tag(name, val):
    ini_buffer.append(name + " = " + str(val) + "\n")

def random_string(minlen, maxlen):
    strlist = []
    for char in range(0, random.randint(minlen, maxlen)):
        strlist.append(random_char())
    return "".join(strlist)

def random_int():
    return random.randint(minint, maxint)

def random_bool():
    return bools[random.randint(0, len(bools)-1)]

def random_float():
    return random.uniform(minfloat, maxfloat)

def ini_tag():
    val = None
    if(valtype == "string"):
        val = random_string(minstrlength, maxstrlength)
    if(valtype == "int"):
        val = random_int()
    if(valtype == "float"):
        val = random_float()
    if(valtype == "bool"):
        val = random_bool()
    tag(random_string(mintaglength, maxtaglength), val)

def ini_array(name):
    length = random.randint(mintags, maxtags)
    tag(name, length)
    for elem in range(1, length + 1):
        val = None
        if(valtype == "strings"):
            val = random_string(minstrlength, maxstrlength)
        if(valtype == "ints"):
            val = random_int()
        if(valtype == "floats"):
            val = random_float()
        if(valtype == "bools"):
            val = random_bool()
        tag(name + str(elem), val)
        if(random.randint(0, 100) < commentchance):
            comment(random_string(minstrlength, maxstrlength))

def ini_multitag():
    length = random.randint(mintags, maxtags)
    val = ""
    for elem in range(0, length):
        if(elem > 0):
            val += ","
        if(valtype == "stringm"):
            val += random_string(minstrlength, maxstrlength)
        if(valtype == "intm"):
            val += str(random_int())
        if(valtype == "floatm"):
            val += str(random_float())
        if(valtype == "boolm"):
            val += str(random_bool())
    tag(random_string(mintaglength, maxtaglength), val)

def ini_section():
    header(random_string(mintaglength, maxtaglength))
    if valtype[-1] == "s":
        for array in range(0, random.randint(minarrays, maxarrays)):
            ini_array(chars[array])
    elif valtype[-1] == "m":
        for multitag in range(0, random.randint(minarrays, maxarrays)):
            ini_multitag()
            if(random.randint(0, 100) < commentchance):
                comment(random_string(minstrlength, maxstrlength))
    else:
        for tag in range(0, random.randint(mintags, maxtags)):
            ini_tag()
            if(random.randint(0, 100) < commentchance):
                comment(random_string(minstrlength, maxstrlength))

def ini_file(fname):
    comment("Generated by testgen.py which is a part of MiniINI")
    for section in range(0, sections):
        newline()
        ini_section()
    with open(fname, "w") as inifile:
        inifile.write("".join(ini_buffer))

def main():
    global valtype
    global sections
    global mintags
    global maxtags
    global minarrays
    global maxarrays
    global mintaglength
    global maxtaglength
    random.seed(seed)
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hv:s:a:A:t:T:l:L:",
                     ["help", "val-type=", "sections=", "min-arrays=",
                      "max-arrays=", "min-tags=", "max-tags=", "min-tag-length=",
                      "max_tag_length="])
    except getopt.GetoptError:
        help()
        sys.exit(1)

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        elif(opt in ("-v", "--val-type")):
            valtype = arg
        elif(opt in ("-s", "--sections")):
            sections = int(arg)
        elif(opt in ("-a", "--min-arrays")):
            a = int(arg)
            if a > 26:
                a = 26
            minarrays = a
        elif(opt in ("-A", "--max-arrays")):
            a = int(arg)
            if a > 26:
                a = 26
            maxarrays = a
        elif(opt in ("-t", "--min-tags")):
            mintags = int(arg)
        elif(opt in ("-T", "--max-tags")):
            maxtags = int(arg)
        elif(opt in ("-l", "--min-tag-length")):
            mintaglength = int(arg)
        elif(opt in ("-L", "--max-tag-length")):
            maxtaglength = int(arg)
    if len(args):
        outfile = args[0]
        ini_file(outfile)
    else:
        help()
        sys.exit(1)

if __name__ == '__main__':
    main()
