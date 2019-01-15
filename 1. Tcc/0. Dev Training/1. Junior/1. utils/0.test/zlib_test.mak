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


all: zlib_test.exe 

zlib_test.exe:
	 tcc -L$(lib) -lzlib1 -lstr_tools -I$(inc) "$(source)\zlib_test.c"
	 move zlib_test.exe $(bin)\zlib_test.exe
	 copy $(lib)\zlib1.dll $(bin)\zlib1.dll
		

	 
