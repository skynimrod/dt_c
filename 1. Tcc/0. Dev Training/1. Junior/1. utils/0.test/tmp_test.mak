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


all: tmp_test 

tmp_test:
	tcc "$(source)\tmp_test.c"
	move tmp_test.exe $(bin)\tmp_test.exe

	 
