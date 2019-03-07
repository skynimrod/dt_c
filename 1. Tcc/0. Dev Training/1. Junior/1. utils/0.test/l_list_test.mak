##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 0.test
bin		= bin
inc		= inc
lib   = lib


all: l_list_test 

l_list_test:
	tcc -L$(lib) -lstr_tools -I$(inc) "$(source)\l_list_test.c"
	move l_list_test.exe $(bin)\l_list_test.exe

	 
