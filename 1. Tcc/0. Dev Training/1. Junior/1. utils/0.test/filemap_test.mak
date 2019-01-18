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


all: filemap_test.exe 

filemap_test.exe:
	 tcc -L$(lib) -lfilemap -lfile_tools -I$(inc) "$(source)\filemap_test.c"
	 move filemap_test.exe $(bin)\filemap_test.exe
	 copy $(lib)\filemap.dll $(bin)\filemap.dll
	 copy $(lib)\file_tools.dll $(bin)\file_tools.dll
		

	 
