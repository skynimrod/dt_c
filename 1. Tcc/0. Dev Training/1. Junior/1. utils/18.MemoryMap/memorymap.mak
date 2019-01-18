##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 18.memorymap
obj			= obj
lib			= lib
inc			= inc


all: memorymap.dll

memorymap.dll: "$(source)\memorymap.c"
	tcc -shared -I$(inc) "$(source)\memorymap.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\memorymap.h" "$(inc)\memorymap.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\memorymap.dll"
	move memorymap.dll "$(lib)\memorymap.dll"	 
	move memorymap.def "$(lib)\memorymap.def"	 


