##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 0.test
bin		= bin
inc		= inc
lib   = lib


all: str_tools_test.exe 

str_tools_test.exe:
	 tcc -L$(lib) -lstr_tools -I$(inc) "$(source)\str_tools_test.c"
	 move str_tools_test.exe $(bin)\str_tools_test.exe
	 copy $(lib)\str_tools.dll $(bin)\str_tools.dll
		

	 
