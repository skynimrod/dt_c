##################################################################################
#
#                   Copyright (C) 2017-2018 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 7.JSON_tools\cJSON
bin		= bin
lib		= lib
inc		= inc


all: cJSONtest.exe

cJSONtest.exe: "$(source)\cJsontest.c"
	tcc -L$(lib) -fcJSON -I$(inc) "$(source)\cJsontest.c"

    move cJsontest.exe $(bin)\cJsontest.exe
	copy cJSON.dll "$(bin)\cJSON.dll"	 


