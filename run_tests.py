#!/usr/bin/env python2
'''
TEST SCRIPT

Procedure:

1.) Get all the files that end with .mat extension in test_path directory
2.) Sort these file names so that the final output of the program looks OK
3.) Compile all these .mat files to corresponding .c files using the
matlang2c program.
4.) Compile all .c files using gcc compiler.
5.) Run the final executables and write their outputs to corresponding
files. Then compare these files with the correct test results. Accordingly,
output SUCCESS or FAILED.

NOTE: All corresponding .mat, .c, .exe, .test, .out files have the
same file names.

ASSUMPTIONS:
1.) This script and the matlang2c compiler should be in the same directory
2.) gcc is installed
3.) Test files are stored in tests directory relative to this program and
all test cases are stored in files with extension .test
4.) Name of the .mat file and .test file must be the same.
'''

import os
import sys
import subprocess
import filecmp

# Extensions for various types of files
mat_extension = '.mat'
c_extension = '.c'
executable_extension = '.exe'
test_extension = '.test'
output_extension = '.out'
# The current path where the test cases are stored
test_path = 'tests'

def file_test_path(file):
	'''
	Returns the full path of a file that is stored in test_path directory
	'''
	return os.path.join(test_path, file)

def mat_file_names():
	'''
	Returns the name of all files that end with .mat extension. The search
	directory is test_path
	'''
	names = []
	for file in [f for f in os.listdir(test_path)\
				 if os.path.isfile(file_test_path(f)) and\
				 f.endswith(mat_extension)]:
		names.append(file.split('.')[0])
	return names

def compile_to_c(f):
	'''
	Compile all .mat files in test directory to .c files in test directory
	'''
	compiler = 'matlang2c'
	# compile all files to their corresponding c files
	print 'matlang2c compiling ', f + mat_extension, '...'
	return_code = subprocess.call(
			[os.path.join('.', compiler),
			 file_test_path(f + mat_extension),
			 '-o',
			 file_test_path(f + c_extension)])
	return return_code


def compile_c_file(f):
	'''
	Compiles all .c files in the test directory to .exe files in the test dir
	'''
	compiler = 'gcc'
	print 'gcc compiling ', f + c_extension, '...'
	return_code = subprocess.call(
			[compiler,
			 file_test_path(f + c_extension),
			 '-o',
			 file_test_path(f + executable_extension),
			 '-lm'
			 ])
	return return_code


def run_test(f):
	'''
	For all files in file_names, runs the corresponding C program compiled
	before and writes its output to a .out file. Then compares the
	.out and .test files. If they are the same, outputs SUCCESS; else FAILED.
	'''
#Run each test case with its corresponding program and save the output file
#with the name of the program concatenated by .out extension
	cmd = subprocess.Popen(
		[os.path.join('.', file_test_path(f + executable_extension))],
		 stdout = subprocess.PIPE)
	output = cmd.communicate()[0]
	string = ''
	with open(file_test_path(f + '.test'), 'r') as in_file:
		for line in in_file:
			string = string + line
	with open(file_test_path(f + '.out'), 'w') as out_file:
		out_file.write(output)
	if output == string:
		return True
	else:
		return False

def main():
	'''
	A main function wrapping other function calls.
	'''
	file_names = mat_file_names()
	file_names.sort()
	for f in file_names:
		print 'FILE:', f
		if compile_to_c(f) == 0: #All .mat files are compiled to .c
			if compile_c_file(f) == 0:
				if run_test(f):
					print 'SUCCESS'
				else:
					print 'FAILED'
			else:
				print 'ERROR: gcc'
		else:
			print 'ERROR: matlang2c'
		print '========================================'
		print

#start the program
main()
