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


all: pdf_tools_test.exe 

pdf_tools_test.exe:
	echo "pdf_tools_test.exe"
	tcc -L$(lib) -lpdf_tools -I$(inc) "$(source)\pdf_tools_test.c"
	move pdf_tools_test.exe $(bin)\pdf_tools_test.exe
	copy $(lib)\filemap.dll $(bin)\filemap.dll
	copy $(lib)\memorymap.dll $(bin)\memorymap.dll
	copy $(lib)\file_tools.dll $(bin)\file_tools.dll
	copy $(lib)\pdf_tools.dll $(bin)\pdf_tools.dll
	copy $(lib)\str_tools.dll $(bin)\str_tools.dll
	copy $(lib)\zlib1.dll $(bin)\zlib1.dll
		

	 
