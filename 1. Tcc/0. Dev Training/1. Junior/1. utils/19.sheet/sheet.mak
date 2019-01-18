##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 17.filemap
obj			= obj
lib			= lib
inc			= inc


all: filemap.dll

filemap.dll: "$(source)\filemap.c"
	tcc -shared -L$(lib) -lfile_tools -I$(inc) "$(source)\filemap.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\filemap.h" "$(inc)\filemap.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\filemap.dll"
	move filemap.dll "$(lib)\filemap.dll"	 
	move filemap.def "$(lib)\filemap.def"	 


