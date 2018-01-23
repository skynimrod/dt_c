##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 6.exercise
obj		= obj
lib		= lib
inc		= inc


all: exercise.dll exercise_math

exercise.dll: "$(source)\exercise.c"
	tcc -shared "$(source)\exercise.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\exercise.h" "$(inc)\exercise.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\exercise.dll"
	move exercise.dll "$(lib)\exercise.dll"	 
	move exercise.def "$(lib)\exercise.def"	 
	
exercise_math: 
  echo "-------------------"
	tcc -shared -L$(lib) -ltime_tools -I$(inc) "$(source)\exercise_math.c"

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\exercise_math.h" "$(inc)\exercise_math.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\exercise_math.dll"
	move exercise_math.dll "$(lib)\exercise_math.dll"	 
	move exercise_math.def "$(lib)\exercise_math.def"	 


