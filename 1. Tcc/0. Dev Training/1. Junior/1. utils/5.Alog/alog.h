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
    //        1.   IfFileExist()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    #ifndef __FILE_TOOLS_H__    // define file_tools.h Start {
    #define __FILE_TOOLS_H__
    typedef struct __filemap {
	    char * 	_byteStream;
	    char *	filename;
	    long	filelen;
	    char * 	buf;
	    long	buflen;
	    long	pos;

    }FILEMAP;

    typedef struct __memorymap {
	    char * 	_byteStream;
	    char * 	buf;
	    long	buflen;
	    long	pos;

    }MEMORYMAP;


    int		ifOpened( char * filename );
    FILEMAP *  	initFileMap( char * srcFile, FILEMAP * filemap );
    void  	freeFileMap( FILEMAP *filemap );
    void        printfm( FILEMAP * filemap );
    #endif                   // define Astring.h ENd }
