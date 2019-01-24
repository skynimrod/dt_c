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
        long        filelen;
        uchar * 	stream;
	    long	    streamlen;
	    long	    pos;

    }FILEMAP;

    // about FILEMAP  µÄ²Ù×÷
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );
    char  *     fm_readLine( FILEMAP *fm_p );
    uchar *     fm_read( FILEMAP *fm_p, long length, long start );
    void        fm_seek( FILEMAP  * fm_p, long pos );


    #endif                   // define filemap.h ENd }
