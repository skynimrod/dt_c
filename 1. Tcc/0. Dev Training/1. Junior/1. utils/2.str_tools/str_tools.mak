##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.��LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 2.str_tools
obj		= obj
lib		= lib
inc		= inc


all: str_tools.dll

str_tools.dll: "$(source)\str_tools.c"
	tcc -shared "$(source)\str_tools.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\str_tools.h" "$(inc)\str_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\str_tools.dll"
	move str_tools.dll "$(lib)\str_tools.dll"	 
	move str_tools.def "$(lib)\str_tools.def"	 


