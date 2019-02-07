##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 16.pdf_tools
obj		= obj
lib		= lib
inc		= inc


all: pdf_tools.dll

pdf_tools.dll: "$(source)\pdf_tools.c"
	tcc -shared -L$(lib) -lzlib1 -lfile_tools -lfilemap -lmemorymap -lstr_tools -I$(inc) "$(source)\pdf_tools.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\pdf_tools.h" "$(inc)\pdf_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\pdf_tools.dll"
	move pdf_tools.dll "$(lib)\pdf_tools.dll"	 
	move pdf_tools.def "$(lib)\pdf_tools.def"	 


