##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.£¬LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#

source	= 16.pdf_tools/
obj		= obj/
lib		= lib
inc		= inc

CC = tcc

SHAREDLIB = pdfParser.dll
DEF = pdfParser.def

all: $(SHAREDLIB)

OBJS = $(obj)pdf_tool.o $(obj)pdf_xref.o $(obj)pdf_pages.o $(obj)pdf_free.o \
			$(obj)pdf_cmap.o $(obj)pdf_decode.o $(obj)pdf_sheet.o $(obj)pdf_interface.o

$(SHAREDLIB): $(OBJS)
	tcc -shared -o $@ $(OBJS) -L$(lib) -lzlib1 -lfile_tools -lfilemap -lmemorymap -lstr_tools 

	IF not Exist "$(inc)" md "$(inc)"
	copy "$(source)\pdf_tools.h" "$(inc)\pdf_tools.h"	 

	IF not Exist "$(lib)" md "$(lib)"
	echo "$(lib)\$(SHAREDLIB)"
	move $(SHAREDLIB) "$(lib)\$(SHAREDLIB)"	 
	move $(DEF) "$(lib)\$(DEF)"	 
	
$(obj)pdf_tool.o: $(source)pdf_tool.c
	@echo "compile pdf_tool.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_tool.c

$(obj)pdf_xref.o: $(source)pdf_xref.c
	@echo "compile pdf_xref.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_xref.c

$(obj)pdf_pages.o: $(source)pdf_pages.c
	@echo "compile pdf_pages.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_pages.c

$(obj)pdf_free.o: $(source)pdf_free.c
	@echo "compile pdf_free.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_free.c

$(obj)pdf_cmap.o: $(source)pdf_cmap.c
	@echo "compile pdf_cmap.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_cmap.c

$(obj)pdf_decode.o: $(source)pdf_decode.c
	@echo "compile pdf_decode.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_decode.c

$(obj)pdf_sheet.o: $(source)pdf_sheet.c
	@echo "compile pdf_sheet.o"
	$(CC) -c -o $@ -I$(inc) $(source)pdf_sheet.c

$(obj)pdf_interface.o: $(source)pdf_interface.c
	@echo "compile pdf_tools.o"
	$(CC) -c -o $@ -I$(inc) "$(source)\pdf_interface.c"


