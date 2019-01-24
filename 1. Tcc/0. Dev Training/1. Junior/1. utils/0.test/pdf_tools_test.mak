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


all: pdf_tools_test.exe 

pdf_tools_test.exe:
	 tcc -L$(lib) -lpdf_tools -lfilemap -lfile_tools -lstr_tools -I$(inc) "$(source)\pdf_tools_test.c"
	 move pdf_tools_test.exe $(bin)\pdf_tools_test.exe
	 copy $(lib)\filemap.dll $(bin)\filemap.dll
	 copy $(lib)\file_tools.dll $(bin)\file_tools.dll
	 copy $(lib)\pdf_tools.dll $(bin)\pdf_tools.dll
	 copy $(lib)\str_tools.dll $(bin)\str_tools.dll
		

	 