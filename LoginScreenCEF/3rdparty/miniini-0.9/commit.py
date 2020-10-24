#!/usr/bin/python3

# Copyright (C) 2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import subprocess
import getopt
import sys
import os
import os.path

verbosity = 0

def help():
    pass

#run given command and return its return value
def run_cmd(cmd):
    if verbosity > 0:
        print (cmd)
    return subprocess.call(cmd, shell=True)

def test_examples():
    for subdir in os.listdir("example"):
        if run_cmd("cd " + os.path.join("example", subdir) + "; make debug"):
            print("ERROR: Compilation of debug build of example " + subdir + 
                  " failed")
            return 1
        if run_cmd("cd " + os.path.join("example", subdir) + "; make optimized"):
            print("ERROR: Compilation of optimized build of example " + subdir + 
                  " failed")
            return 1
    return 0

def commit(message):
    if run_cmd("make"):
        "ERROR: Compilation failed"
        return 1
    if run_cmd("./tester.py -a"):
        "ERROR: Testing failed"
        return 1
    if(test_examples()):
        return 1
    if run_cmd("make no-stl"):
        "ERROR: Compilation with STL support disabled failed"
        return 1
    if run_cmd("./tester.py -a"):
        "ERROR: Testing with STL support disabled failed"
        return 1

    run_cmd("bzr commit -m '" + message + "'")
    run_cmd("bzr push")
    run_cmd("echo >> code-stats.txt")
    run_cmd("echo 'revision:' >> code-stats.txt")
    run_cmd("bzr revno >> code-stats.txt")
    run_cmd("echo >> code-stats.txt")
    run_cmd("ohcount >> code-stats.txt")
    return 0

def main():
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hv",
                     ["help", "verbose"])
    except getopt.GetoptError:
        print('Input error')
        help()
        sys.exit(1)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        elif opt in ("-v", "--verbose"):
            verbosity += 1
    if len(args) < 1:
        print("Need a commit message")
        sys.exit(2)
    if commit(args[0]):
        sys.exit(3)

if __name__ == '__main__':
    main()
