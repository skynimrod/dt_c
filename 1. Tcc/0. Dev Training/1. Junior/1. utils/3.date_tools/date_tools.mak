##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 3.date_tools
obj		= obj
lib		= lib
inc		= inc


all: date_tools.dll 

date_tools.dll: "$(source)\date_tools.c"
	tcc -shared "$(source)\date_tools.c" -L$(lib) -lstr_tools  -I$(inc)

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\date_tools.h" "$(inc)\date_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\date_tools.dll"
	move date_tools.dll "$(lib)\date_tools.dll"	 
	move date_tools.def "$(lib)\date_tools.def"	 


