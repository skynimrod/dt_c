    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    memorymap
    //
    // Description : about memorymap operation.
    //
    // Interface :
    //      1.  initReadFileMap()
    //
    #ifndef __MEMORYMAP_H__    // define memorymap.h Start {
    #define __MEMORYMAP_H__

    typedef unsigned char  uchar;
    
    typedef struct __memorymap {
	    uchar * 	stream;
	    long	    streamlen;
	    long	    pos;

    }MEMORYMAP;

    // about MEMORYMAP  µÄ²Ù×÷
    void        print_mm( MEMORYMAP * mm_p );
    MEMORYMAP * initMemoryMap( uchar * stream, long  len );
    void        freeMemoryMap( MEMORYMAP * mm_p );
    uchar *     mm_readLine( MEMORYMAP *mm_p );
    uchar *     mm_read( MEMORYMAP *fm_p, long length, long start );
    void        mm_seek( MEMORYMAP  * fm_p, long pos );


    #endif                   // define memorymap.h ENd }
