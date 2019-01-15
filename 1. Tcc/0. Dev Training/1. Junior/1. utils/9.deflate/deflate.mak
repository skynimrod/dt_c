##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 9.deflate
obj		= obj
lib		= lib
inc		= inc


all: deflate.dll

deflate.dll: "$(source)\deflate.c"
	tcc -shared "$(source)\deflate.c" -I.

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\deflate.h" "$(inc)\deflate.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\deflate.dll"
	move deflate.dll "$(lib)\deflate.dll"	 
	move deflate.def "$(lib)\deflate.def"	 


