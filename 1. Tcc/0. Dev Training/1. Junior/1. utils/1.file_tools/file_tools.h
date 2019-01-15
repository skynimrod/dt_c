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
    //
    #ifndef __FILE_TOOLS_H__    // define file_tools.h Start {
    #define __FILE_TOOLS_H__

    size_t      getFileSize( char *fname );
    int		    ifOpened( char * filename );


    #endif                   // define file_tools.h ENd }
