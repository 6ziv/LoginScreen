#!/usr/bin/python3

# Copyright (C) 2009-2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import subprocess
import getopt
import sys

def help():
    print ("MiniINI test suite")
    print ("Copyright (C) 2009-2010 Ferdinand Majerech")
    print ("Usage: tester.py [OPTION...]")
    print ("Runs various MiniINI regression tests, checks for memory leaks.")
    print ("Returns 0 if no errors were detected, non-zero otherwise.")
    print ("Without arguments, only normal test suite is run ")
    print ("(without memory checking)")
    print (" -h --help          display this help and exit")
    print (" -a --all           runs all tests")
    print (" -m --memcheck      runs test suite through valgrind's memcheck")


def run_cmd(cmd):
    print (cmd)
    return subprocess.call(cmd, shell=True)

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hma",
                     ["help", "memcheck", "all"])
    except getopt.GetoptError:
        help()
        sys.exit(1)

    memcheck = False;
    runall = False

    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        elif(opt in ("-a", "--all")):
            runall = True;
        elif(opt in ("-m", "--memcheck")):
            memcheck = True;
    command = "./bin/tester"
    if runall:
        sys.exit(run_cmd("valgrind --error-exitcode=1 " + command) or run_cmd(command))
    elif memcheck:
        sys.exit(run_cmd("valgrind --error-exitcode=1 " + command))
    else:
        sys.exit(run_cmd(command))

if __name__ == '__main__':
    main()
