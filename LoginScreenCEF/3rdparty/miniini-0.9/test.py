#!/usr/bin/python3

# Copyright (C) 2009-2010 Ferdinand Majerech
# This file is part of MiniINI
# For conditions of distribution and use, see copyright notice in LICENSE.txt

import subprocess
import sys
import copy
import getopt
import configparser
import os


verbosity = 0


def help():
    print("Aggregate benchmark script")
    print("Copyright (C) 2009-2010 Ferdinand Majerech")
    print("Usage: test.py [OPTIONS] [ARGUMENTS]")
    print(" -h --help            display this help and exit")
    print(" -v --verbose         more detailed output - can be repeated to")
    print("                      increase verbosity further")
    print(" -t --tool       val  specify tool to use - possible values:")
    print("                      benchmark - run benchmarks specified in an ini")
    print("                                  file. The first argument is the")
    print("                                  ini file in test/ini directory.")
    print("                                  The second is name of subdirectory")
    print("                                  to write results to, in test/runs")
    print("                                  directory.")
    print("                      compare - compare two or more output")
    print("                                directories of the benchmark tool")
    print("                                - the first argument is the ")
    print("                                subdirectory to output results of")
    print("                                the comparison to, in test/comps.")
    print("                                Other arguments are names of ")
    print("                                directories to compare in test/runs.")
    print(" -T --bench-tools val Only applies with tool=benchmark. A string of")
    print("                      characters specifying what benchmarking tools")
    print("                      should be used. Valid characters:")
    print("                      m - Memcheck")
    print("                      c - Callgrind")
    print("                      C - Cachegrind")
    print("                      M - Massif")
    print("                      t - Run time")
    print("                      e - Extra benchmarking information provided")
    print("                          by bin/benchmark and/or bin/benchmark-dbg")
    print("                          compiled with -DMINIINI_BENCH_EXTRA")
    print(" -k --keep           keep all files produced by testutil.py runs,")
    print("                     not just summaries.")

#run given command and return its return value
def run_cmd(cmd):
    if verbosity > 1:
        print (cmd)
    return subprocess.call(cmd, shell=True)


#A group of files with identical filenames stored in different directories.
class FileGroup:
    def __init__(self, root_dir, file_name):
        #Base directory all base directories in the group share,
        self.__root_dir = root_dir
        #Directories that contain this file (under __root_dir).
        self.__base_dirs = []
        assert(file_name.startswith(root_dir))
        #File name shared by all the files in the group. Might contain 
        #directories, but they have to be the same for all the files.
        #Under __root_dir + __base_dirs.
        self.__file_name = file_name[len(root_dir):].partition("/")[2]
    
    #Add a file name to this group. Given filename must have the same root
    #directory and filename as this file group, only the base directory can vary.
    def add_file_name(self, file_name):
        assert(file_name.startswith(self.__root_dir))
        self.__base_dirs.append(file_name[len(self.__root_dir):].partition("/")[0])

    #Equality operator with a file name string. Must have the same root 
    #directory as this file group. The file name is equal if it has the same 
    #filename as this group, but can have varying base directory 
    def __eq__(self, file_name):
        assert(file_name.startswith(self.__root_dir))
        return (self.__file_name == 
                file_name[len(self.__root_dir):].partition("/")[2])
    
    #Return a sorted list of names or files in this group
    def __get_file_names(self):
        file_names = []
        for base_dir in self.__base_dirs:
            root = os.path.join(self.__root_dir, base_dir)
            file_names.append(os.path.join(root, self.__file_name))
        return sorted(file_names)

    #Compare files in the group using testutil-compare.py
    def compare(self, out_dir):
        if len(self.__base_dirs) < 2:
            #ignore files that can't be compared to anything
            return
        out_file_name = os.path.join(out_dir, self.__file_name)
        try:
            os.makedirs(os.path.split(out_file_name)[0])
        except OSError:
            pass
        command = ("./testutil-compare.py -r -o " + out_file_name + " " +
                   " ".join(self.__get_file_names()))
        run_cmd(command)

    #Merge files in the group using testutil-compare-merge.py
    def merge(self, out_dir):
        if len(self.__base_dirs) < 2:
            #ignore files that can't be compared to anything
            return
        out_file_name = os.path.join(out_dir, self.__file_name + ".merged")
        try:
            os.makedirs(os.path.split(out_file_name)[0])
        except OSError:
            pass
        command = ("./testutil-compare-merge.py -o " + out_file_name + " " +
                   " ".join(self.__get_file_names()))
        run_cmd(command)

#Compare tool - compares directories written by the benchmark tool
class Compare:
    def __init__(self, out_dir, comp_dirs):
        #benchmark outputs are stored here
        self.__runs_dir = "./test/runs/"
        #comparisons are output written
        self.__comps_dir = "./test/comps/"
        #output directory in test/comps
        self.__out_dir = os.path.join(self.__comps_dir, out_dir) + "/"
        run_cmd("rm -r -f " + self.__out_dir)
        #comared directories output by test.py benchmark tool
        self.__comp_dirs = [] 
        for comp_dir in comp_dirs:
            self.__comp_dirs.append(os.path.join(self.__runs_dir, comp_dir))
        self.__compare()

    #compare outputs of identical benchmarks in input directories
    def __compare(self):
        compared_files = []
        for comp_dir in self.__comp_dirs:
            print("Adding files to compare from", comp_dir)
            for root, dirs, files in os.walk(comp_dir):
                for file_name in files:
                    file_name = os.path.join(root, file_name)
                    if file_name.endswith(".sum.ini"):
                        if not file_name in compared_files:
                            compared_files.append(FileGroup(self.__runs_dir,
                                                            file_name))
                        index = compared_files.index(file_name)
                        compared_files[index].add_file_name(file_name)
        print("Comparing files")
        for comp in compared_files:
            comp.compare(self.__out_dir)
        self.__merge()

    #merge comparisons of identical benchmarks with different inputs
    def __merge(self):
        merged_files = []
        print("Adding files to merge from", self.__out_dir)
        comp_dirs = None
        
        for root, dirs, files in os.walk(self.__out_dir):
            #first iteration of os.walk will return subdirs of
            #__out_dir, which can be deleted after done with merging
            if comp_dirs == None:
                comp_dirs = dirs
            for file_name in files:
                file_name = os.path.join(root, file_name)
                if file_name.endswith(".sum.ini"):
                    if not file_name in merged_files:
                        merged_files.append(FileGroup(self.__out_dir,file_name))
                    index = merged_files.index(file_name)
                    merged_files[index].add_file_name(file_name)
        print("Merging comparisons")
        for merged in merged_files:
            merged.merge(self.__out_dir)
        print("Deleting comparisons")
        for comp_dir in comp_dirs:
            run_cmd("rm -r " + os.path.join(self.__out_dir, comp_dir))


#Generates input ini files for benchmarks
class FileGenerator:
    def __init__(self, ini_file, ini_section):
        print("Loading benchmark input file settings")
        #used for reading types from ini file
        self.__types_map = {"s" : "string", 
                            "i" : "int", 
                            "f" : "float", 
                            "b" : "bool"}
        self.__bench_ini_dir = "test/benchmark/"

        #names of generated files
        self.__gen_files = []
        #readstring, readint etc.
        self.__benchmarks = []
        #sizes of generated files
        self.__file_sizes = []

        #number of sections to generate
        self.__sections = ini_file.getint(ini_section, "sections")
        #arrays, multitags, tags
        self.__mode = ini_file.get(ini_section, "mode")
        #a string of chars, s for string, i for int etc.
        self.__types = ini_file.get(ini_section, "types")
        #min, max number of tags per section/array/multi value tag to generate
        self.__tags_min = ini_file.getint(ini_section, "tags-min")
        self.__tags_max = ini_file.getint(ini_section, "tags-max")
        #min, max sizes of tags in chars to generate
        self.__tag_len_min = ini_file.getint(ini_section, "tag-len-min")
        self.__tag_len_max = ini_file.getint(ini_section, "tag-len-max")
        if(self.__mode == "arrays"):
            #min, max arrays per section to generate
            self.__arrays_min = ini_file.getint(ini_section, "arrays-min")
            self.__arrays_max = ini_file.getint(ini_section, "arrays-max")
        if(self.__mode == "multitags"):
            #min, max multi value tags per section to generate
            self.__multitags_min = ini_file.getint(ini_section, "multitags-min")
            self.__multitags_max = ini_file.getint(ini_section, "multitags-max")

    #Generate ini files 
    def generate(self):
        print("Generating benchmark input files")
        cmd = ("./testgen.py -s " + str(self.__sections) + " -t " +
               str(self.__tags_min) + " -T " + str(self.__tags_max) + " -l " +
               str(self.__tag_len_min) + " -L " + str(self.__tag_len_max))

        file_ext = ".ini"
        #extension to a data type taken from __types_map
        type_ext = ""
        if(self.__mode == "arrays"):
            cmd += " -a " + str(self.__arrays_min) + " -A " + str(self.__arrays_max) + " -v "
            file_ext = "s" + file_ext
            type_ext = "s"
        elif(self.__mode == "multitags"):
            cmd += (" -a " + str(self.__multitags_min) + " -A " +
                    str(self.__multitags_max) + " -v ")
            file_ext = "m" + file_ext
            type_ext = "m"
        elif(self.__mode == "tags"):
            cmd += " -v "
        else:
            #unknown mode
            raise NotImplementedError
        for data_type in self.__types:
            data_type = self.__types_map[data_type]
            gen_file_name = self.__bench_ini_dir + data_type + file_ext
            self.__gen_files.append(gen_file_name)
            self.__benchmarks.append("read" + data_type + type_ext)
            run_cmd(cmd + data_type + type_ext + " " + gen_file_name)
            self.__file_sizes.append(os.path.getsize(gen_file_name))

    #Remove ini files
    def clean_up(self):
        print("Removing benchmark input files")
        for gen_file in self.__gen_files:
            run_cmd("rm " + gen_file)

    #return benchmark names to perform on generated files
    def get_benchmarks(self):
        return self.__benchmarks

    #return file sizes of generated files
    def get_file_sizes(self):
        return self.__file_sizes

#Generates and runs benchmark commands for one section in a benchmark ini file
class BenchRunner:
    def __init__(self, keep_files, bench_tools, ini_file, ini_section):
        print("Loading benchmark settings from section", ini_section)
        #types of benchmarks to run
        self.__bench_all = False
        self.__bench_separate = False
        self.__bench_rules = False
        self.__bench_tester = False

        #benchmark the STL API instead of default API
        self.__stl = False
        #use extra benchmarking information output by bin/benchmark compiled
        #with -DMINIINI_BENCH_EXTRA
        self.__extra_bench_tool = False
        
        #load benchmark settings from given section
        if(ini_file.has_option(ini_section, "bench-all")):
            self.__bench_all = ini_file.getboolean(ini_section, "bench-all")
        if(ini_file.has_option(ini_section, "bench-separate")):
            self.__bench_separate = ini_file.getboolean(ini_section, "bench-separate")
        if(ini_file.has_option(ini_section, "bench-rules")):
            self.__bench_rules = ini_file.getboolean(ini_section, "bench-rules")
        if(ini_file.has_option(ini_section, "bench-tester")):
            self.__bench_tester = ini_file.getboolean(ini_section, "bench-tester")
        if(ini_file.has_option(ini_section, "stl")):
            self.__stl = ini_file.getboolean(ini_section, "stl")

        if "e" in bench_tools:
            self.__extra_bench_tool = True
            bench_tools = bench_tools.replace("e", "")

        keep = ""
        if(keep_files):
            keep = " -k "

        self.__test_base = ("./testutil.py -t " + bench_tools + keep + 
                            " -f bench-")


        if verbosity > 2:
            #print verbose testutil.py output
            self.__test_base = ("./testutil.py -vv -t " + bench_tools + keep + 
                                " -f bench-")
        self.__generator = FileGenerator(ini_file, ini_section)

    #Returns number of benchmarks to be performed by this BenchRunner
    #Used for percent feedback
    def __bench_count(self):
        out = 0
        if self.__bench_tester:
            out += 1
        if self.__bench_all:
            out += 2
        if self.__bench_separate:
            out += 2 * len(self.__generator.get_benchmarks())
        if self.__bench_rules:
            out += 2
        return out
    
    #Print current progress of benchmarks done by this BenchRunner
    def __progress_report(self, benchmarks_done):
        print("Section progress:",100*benchmarks_done/self.__bench_count(),"%")
        
    #Generate and run benchmark commands
    def run(self, runs_dir):
        print("Running benchmarks")
        self.__generator.generate()
        run_cmd("rm -r -f " + runs_dir)

        #number of benchmarks done so far (used by percent feedback)
        benchmarks_done = 0
        
        #benchmark on the regression tester executable
        if(self.__bench_tester):
            run_cmd(self.__test_base + "tester -o " + 
                    os.path.join(runs_dir, "dbg")  + " bin/tester")
            benchmarks_done += 1
            self.__progress_report(benchmarks_done)

        for target, bench_cmd in [("dbg", "bin/benchmark-dbg "),
                                  ("opt", "bin/benchmark ")]:
            print("Target:", target)
            extra_str = ""
            if self.__stl:
                bench_cmd += " --stl"
            target_dir = os.path.join(runs_dir, target)

            #single benchmark on all generated input files
            if(self.__bench_all):
                benchmarks = " ".join(self.__generator.get_benchmarks())
                bench_str = " '" + bench_cmd + benchmarks + "' "
                file_size = 0
                for s in self.__generator.get_file_sizes():
                    file_size += s
                if self.__extra_bench_tool:
                    extra_str = (" -e '" + bench_cmd + "--extra " + 
                                 benchmarks + "' ")
                run_cmd(self.__test_base  + "all -o " + target_dir + 
                        extra_str + bench_str + " filesize=" + str(file_size))
                benchmarks_done += 1
                self.__progress_report(benchmarks_done)

            #one benchmark per generated input file
            if(self.__bench_separate):
                for bench_index in range(len(self.__generator.get_benchmarks())):
                    benchmark = self.__generator.get_benchmarks()[bench_index]
                    bench_str = " '" + bench_cmd + " " + benchmark + "' "
                    file_size = self.__generator.get_file_sizes()[bench_index]
                    if self.__extra_bench_tool:
                        extra_str = (" -e '" + bench_cmd + " --extra " + 
                                     benchmark + "' ")
                    run_cmd(self.__test_base + benchmark + " -o " + 
                            target_dir + extra_str + bench_str + " filesize=" + 
                            str(file_size))
                    benchmarks_done += 1
                    self.__progress_report(benchmarks_done)

            #readstringrules benchmark
            if(self.__bench_rules):
                bench_str = " '" + bench_cmd + " readstringrules' "
                if self.__extra_bench_tool:
                    extra_str = " -e '" + bench_cmd + " --extra readstringrules' "
                run_cmd(self.__test_base + "readstringrules -o " + target_dir + 
                        extra_str + bench_str)
                benchmarks_done += 1
                self.__progress_report(benchmarks_done)
    
    #Clean up any unneeded files created by this BenchRunner 
    def clean_up(self):
        self.__generator.clean_up()

#Benchmark tool - runs benchmarks specified in specified ini file and writes 
#their ouptuts to specified directory
class Benchmark:
    def __init__(self, ini_file_name, runs_dir, keep_files, bench_tools):
        self.__bench_tools = bench_tools
        self.__keep_files = keep_files
        self.__run(os.path.join("test/runs/", runs_dir), 
                   os.path.join("test/ini/", ini_file_name))

    def __run(self, runs_dir, ini_file_name):
        print("Running benchmarks from", ini_file_name, 
              "\noutput directory:", runs_dir)
        ini_file = configparser.ConfigParser()
        ini_file.read(ini_file_name)
        for ini_section in ini_file.sections():
            runner = None
            try:
                runner = BenchRunner(self.__keep_files, self.__bench_tools,
                                    ini_file, ini_section)
            except configparser.NoOptionError:
                print ("Error reading ", ini_file_name, ". Aborting")
                sys.exit(4)
            runner.run(os.path.join(runs_dir, ini_section))    
            runner.clean_up()


def main():
    global verbosity

    keep_files = False
    tool = "benchmark"
    bench_tools = "MctCm"
    try:
        opts, args = getopt.getopt(sys.argv[1:], "ht:vkT:",
                     ["help", "tool=", "verbose", "keep", "bench-tools"])
    except getopt.GetoptError:
        help()
        sys.exit(1)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            help()
            sys.exit(0)
        elif opt in ("-v", "--verbose"):
            verbosity += 1
        elif opt in ("-t", "--tool"):
            tool = arg
        elif opt in ("-T", "--bench-tools"):
            bench_tools = arg
        elif opt in ("-k", "--keep"):
            keep_files = True
    if(tool == "benchmark"):
        Benchmark(args[0], args[1], keep_files, bench_tools)
    elif tool == "compare":
        if(len(args) < 3):
            help()
            sys.exit(5)
        Compare(args[0], args[1:])
    else:
        help()
        sys.exit(3)

if __name__ == '__main__':
    main()
