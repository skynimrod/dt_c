    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    File_tools
    //
    // Description : about file operation.
    //
    // Interface :
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //      4.   getFileSize()
    //
    //  下面是关于  FILEMAP 的接口
    //      1.  initReadFileMap()
    //
    #ifndef __FILE_TOOLS_H__    // define file_tools.h Start {
    #define __FILE_TOOLS_H__

    typedef unsigned char  uchar;
    
    typedef struct __filemap {
	    uchar   *   _byteStream;
	    char    *   filename;
	    long	    filelen;
	    uchar   *   buf;
	    long	    buflen;
	    long	    pos;

    }FILEMAP;

    typedef struct __memorymap {
	    char * 	_byteStream;
	    char * 	buf;
	    long	buflen;
	    long	pos;

    }MEMORYMAP;

    size_t      getFileSize( char *fname );
    int		    ifOpened( char * filename );

    // about FILEMAP  的操作
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initReadFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );


    #endif                   // define file_tools.h ENd }
