##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 0.test\7.cJSONtest
bin		= bin
inc		= inc
lib   = lib


all: cJSON_test.exe 

cJSON_test.exe:
	 tcc -L$(lib) -lcJSON -I$(inc) "$(source)\cJSON_test.c"
	 move cJSON_test.exe $(bin)\cJSON_test.exe
	 copy $(lib)\cJSON.dll $(bin)\cJSON.dll
		

	 
