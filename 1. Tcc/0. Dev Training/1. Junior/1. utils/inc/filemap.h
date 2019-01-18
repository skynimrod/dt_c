    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    sheet
    //
    // Description : about sheet operation.
    //
    // Interface :
    //      1.  initReadFileMap()
    //
    #ifndef __SHEET_H__    // define sheet.h Start {
    #define __SHEET_H__

    typedef unsigned char  uchar;
    
    typedef struct __filemap {
        char    *   filename;
        long    *   filelen;
        uchar * 	stream;
	    long	    streamlen;
	    long	    pos;

    }FILEMAP;

    // about FILEMAP  �Ĳ���
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );
    uchar *     fm_readLine( FILEMAP *fm_p );
    uchar *     fm_read( FILEMAP *fm_p, long length, long start );
    void        fm_seek( FILEMAP  * fm_p, long pos );


    #endif                   // define sheet.h ENd }
