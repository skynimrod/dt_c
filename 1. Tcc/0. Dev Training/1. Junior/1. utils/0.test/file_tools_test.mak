##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.��LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 0.test
bin		= bin
inc		= inc
lib   = lib


all: file_tools_test.exe 

file_tools_test.exe:
	 tcc -L$(lib) -lfile_tools -lexercise -I$(inc) "$(source)\file_tools_test.c"
	 move file_tools_test.exe $(bin)\file_tools_test.exe
	 copy $(lib)\file_tools.dll $(bin)\file_tools.dll
		

	 
