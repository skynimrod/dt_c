##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 8.time_tools
obj		= obj
lib		= lib
inc		= inc


all: time_tools.dll

time_tools.dll: "$(source)\time_tools.c"
	tcc -shared "$(source)\time_tools.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\time_tools.h" "$(inc)\time_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\time_tools.dll"
	move time_tools.dll "$(lib)\time_tools.dll"	 
	move time_tools.def "$(lib)\time_tools.def"	 


