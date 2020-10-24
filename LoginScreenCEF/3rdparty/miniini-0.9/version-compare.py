#!/usr/bin/python3

# Copyright (C) 2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import subprocess
import getopt
import sys
import os.path

verbosity = 0

def help():
    print("MiniINI version comparator script")
    print("Copyright (C) 2010 Ferdinand Majerech")
    print("Usage: version-compare.py [OPTIONS] [ARGUMENTS]")
    print(" -h --help          display this help and exit")
    print(" -v --verbose       more detailed output - can be repeated to")
    print("                    increase verbosity further")
    print(" -k --keep          keep all files produced by testutil.py runs,")
    print("                    not just summaries.")
    print("ARGUMENTS must be directories of versions to compare.")

#run given command and return its return value
def run_cmd(cmd):
    if verbosity > 1:
        print (cmd)
    return subprocess.call(cmd, shell=True)

#A group of directories that are outputs of identical benchmarks on different
#versions
class DirGroup:
    def __init__(self, base_name):
        #base name of all directories in the group
        self.__base_name = base_name
        #extensions to base name that differ between directories in the group
        #i.e. actual directory names are __base_name + __dir_extensions[i]
        self.__dir_extensions = []

    #Equality operator with a string to allow list searching, in operator
    def __eq__(self, dir_name):
        return dir_name.startswith(self.__base_name)
    
    #Return a list of names of all directories in this group
    def __get_dir_names(self):
        out = []
        for ext in self.__dir_extensions:
            out.append(self.__base_name + ext)
        return sorted(out)

    #Add a directory to this group. Given directory must have same base name
    #as this group
    def add_dir(self, dir_name):
        assert(dir_name == self)
        self.__dir_extensions.append(dir_name[len(self.__base_name):])

    #Compare all directories in this group using compare tool of test.py
    def compare(self):
        #ignore groups with not enough directories to compare
        if(len(self.__dir_extensions) < 2):
            return
        command = ("./test.py --tool=compare " + self.__base_name + " " +
                   " ".join(self.__get_dir_names()))
        run_cmd(command)

#Handles comparison of multiple MiniINI versions
class CompareVersions:
    def __init__(self, comp_dirs, keep_files):
        #scripts that we copy to directories of the versions so
        #we always compare using the same scripts
        self.__scripts = ["testutil.py", "testutil-compare.py", 
                          "testutil-compare-merge.py", "test.py"]
        self.__dir_groups = []
        self.__keep_files = keep_files

        self.__benchmark(comp_dirs)
        self.__compare()
    
    #Run benchmarks to compare in each version directory
    def __benchmark(self, comp_dirs):
        comp_dir_index = 0
        for comp_dir in comp_dirs:
            print("VERSION-COMPARE.PY: benchmarking directory", comp_dir )
            #back up scripts, copy our scripts into the directory
            for script in self.__scripts:
                dir_script = os.path.join(comp_dir, script)
                run_cmd("mv " + dir_script + " " + dir_script + ".bkp")
                run_cmd("cp " + script + " " + dir_script)

            #run test.py --tool=benchmark for every inifile
            comp_ini_dir = os.path.join(comp_dir, "test/ini")
            bench_dirs =  []
            for file_name in os.listdir(comp_ini_dir):
                if file_name.endswith(".ini"):
                    #the last 4 chars of the dir name are index of the version
                    bench_dir = file_name + "%04d" % comp_dir_index
                    bench_dirs.append(bench_dir)
                    keep = ""
                    if self.__keep_files:
                        keep = " -k "
                    command = ("cd " + comp_dir + 
                               "; ./test.py --tool=benchmark" + keep + 
                               file_name + " " + bench_dir)
                    run_cmd(command)
            #remove our scripts, restore backed up scripts
            for script in self.__scripts:
                dir_script = os.path.join(comp_dir, script)
                run_cmd("rm " + dir_script)
                run_cmd("mv " + dir_script + ".bkp" + " " + dir_script)
            #move runs dirs to our runs
            for bench_dir in bench_dirs:
                bench_dir =  os.path.join("test/runs", bench_dir)
                command = ("mv " + os.path.join(comp_dir, bench_dir) + " " +
                           bench_dir)
                run_cmd(command)
            self.__add_run_dirs(bench_dirs)
            comp_dir_index += 1

    #Add directories written out by benchmarks to directory groups
    def __add_run_dirs(self, bench_dirs):
        for bench_dir in bench_dirs:
            if not bench_dir in self.__dir_groups:
                #the last 4 chars of the dir name are index of the version
                self.__dir_groups.append(DirGroup(bench_dir[:-4]))
            group_index = self.__dir_groups.index(bench_dir)
            self.__dir_groups[group_index].add_dir(bench_dir)

    #Compare directories in directory groups
    def __compare(self):
        print("VERSION-COMPARE.PY: comparing directories")
        for group in self.__dir_groups:
            group.compare()

def main():
    keep_files = False
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hvk",
                     ["help", "verbose", "keep"])
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
        elif opt in ("-k", "--keep"):
            keep_files = True
    if len(args) < 2:
        print("Need at least 2 arguments")
        help()
        sys.exit(2)
    CompareVersions(args, keep_files)

if __name__ == '__main__':
    main()
