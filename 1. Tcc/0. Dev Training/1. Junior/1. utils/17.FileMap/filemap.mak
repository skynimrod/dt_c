##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 1.file_tools
obj			= obj
lib			= lib
inc			= inc


all: file_tools.dll

file_tools.dll: "$(source)\file_tools.c"
	tcc -shared "$(source)\file_tools.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\file_tools.h" "$(inc)\file_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\file_tools.dll"
	move file_tools.dll "$(lib)\file_tools.dll"	 
	move file_tools.def "$(lib)\file_tools.def"	 


