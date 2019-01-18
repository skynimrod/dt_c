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


all: memorymap_test.exe 

memorymap_test.exe:
	 tcc -o $(bin)/$@ -L$(lib) -lfilemap -lfile_tools -lmemorymap -I$(inc) "$(source)\memorymap_test.c"
	 copy $(lib)\filemap.dll $(bin)\filemap.dll
	 copy $(lib)\file_tools.dll $(bin)\file_tools.dll
	 copy $(lib)\memorymap.dll $(bin)\memorymap.dll
		

	 
