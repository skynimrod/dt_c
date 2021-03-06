##################################################################################
#
#                   Copyright (C) 2017-2017 AQUGEN TECH CO.，LTD                
#                                All Rights Reserved                            
#
##################################################################################
#
#  用nmake  编译
#  
source	= 15.zlib/
source1 = 15.zlib\			# 因为copy的时候不认/ , 只认 \
obj			= obj/
lib			= lib
inc			= inc

CC = tcc

SHAREDLIB = zlib1.dll

CFLAGS = 

ZINC = 

#OBJS = $(obj)adler32.o $(obj)deflate.o $(obj)crc32.o $(obj)trees.o $(obj)zutil.o
OBJS = $(obj)adler32.o $(obj)compress.o $(obj)crc32.o $(obj)deflate.o $(obj)gzclose.o \
			$(obj)gzlib.o $(obj)gzread.o $(obj)gzwrite.o $(obj)infback.o $(obj)inffast.o \
			$(obj)inflate.o $(obj)inftrees.o $(obj)trees.o $(obj)uncompr.o $(obj)zutil.o

all: $(SHAREDLIB)

$(SHAREDLIB): $(OBJS)
	@echo "hello, begin zlib1.dll compile"
	tcc -shared -o $@ $(OBJS)
	move $@ $(lib)\$@
	copy "$(source1)zlib.h" "$(inc)\zlib.h"
	copy "$(source1)zutil.h" "$(inc)\zutil.h"
	copy "$(source1)zconf.h" "$(inc)\zconf.h"
	
	
$(obj)adler32.o: $(source)adler32.c
	@echo "compile adler32.o"
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)adler32.c
	
$(obj)crc32.o: $(source)crc32.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)crc32.c

$(obj)deflate.o: $(source)deflate.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)deflate.c

$(obj)infback.o: $(source)infback.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)infback.c

$(obj)inffast.o: $(source)inffast.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)inffast.c

$(obj)inflate.o: $(source)inflate.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)inflate.c

$(obj)inftrees.o: $(source)inftrees.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)inftrees.c

$(obj)trees.o: $(source)trees.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)trees.c

$(obj)zutil.o: $(source)zutil.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)zutil.c

$(obj)compress.o: $(source)compress.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)compress.c

$(obj)uncompr.o: $(source)uncompr.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)uncompr.c

$(obj)gzclose.o: $(source)gzclose.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)gzclose.c

$(obj)gzlib.o: $(source)gzlib.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)gzlib.c

$(obj)gzread.o: $(source)gzread.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)gzread.c

$(obj)gzwrite.o: $(source)gzwrite.c
	$(CC) $(CFLAGS) $(ZINC) -c -o $@ $(source)gzwrite.c
