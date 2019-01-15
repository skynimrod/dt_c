    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    filemap
    //
    // Description : about filemap operation.
    //
    // Interface :
    //      1.  initReadFileMap()
    //
    #ifndef __FILEMAP_H__    // define filemap.h Start {
    #define __FILEMAP_H__

    typedef unsigned char  uchar;
    
    typedef struct __filemap {
	    char    *   filename;
	    size_t	    filelen;
	    uchar   *   stream;         // 字节流
	    size_t	    steamlen;
	    size_t	    pos;

    }FILEMAP;

    typedef struct __memorymap {
	    char * 	_byteStream;
	    char * 	buf;
	    long	buflen;
	    long	pos;

    }MEMORYMAP;

    // about FILEMAP  的操作
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initReadFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );


    #endif                   // define filemap.h ENd }
