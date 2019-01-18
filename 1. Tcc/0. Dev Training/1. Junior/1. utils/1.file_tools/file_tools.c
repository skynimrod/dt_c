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
    //       参见: C99  --- 7.19.3   P277
    //                      7.19.4   P279   关于文件的操作
    // Interface :
    //        1.   IfOpened()
    //        2.   getFileSize()
    //        4.   
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  使用 true, false 值的定义

    #include    "file_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    //**************************************************************************
    //  函数名称 : ifOpened()
    //  功    能 :
    //		判断给定的文件是否已经打开
    //  入口参数 :
    //            str : 字符串
    //  出口参数 :
    //            无
    //  返 回 值 :
    //            char  * : 目标字符串
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			调用后源串不变, 对中文空格不处理, 只处理开头西文空格与Tab键            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    /*
     * _access() 如果能以指定模式成功打开文件, 则返回0, 否则返回-1
     */
    int DLL_EXPORT ifOpened( char * filename ) 
    {
	    if ( _access( filename, 0 ) != 0 ) {
		    printf( "file:%s is not exists!\n", filename );
		    return -2;
	    }

	    int    h = sopen( filename, 2, 0 );
	    if ( h == 3 ) {
		    _close (h );
		    return 0;            // # 文件没有被任何人打开
	    }

	    return 1;          //  # 文件已经被打开了
    }

    //**************************************************************************
    //  函数名称 : getFileSize()
    //  功    能 :
    //		获取文件大小
    //  入口参数 :
    //            fname : 文件名称
    //  出口参数 :
    //            无
    //  返 回 值 :
    //            char  * : 目标字符串
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			调用后源串不变, 对中文空格不处理, 只处理开头西文空格与Tab键            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    size_t DLL_EXPORT getFileSize( char *fname ) 
    {
        size_t   len;
        FILE    *fp;

        if ( ! (fp = fopen(fname, "rb" )) )
                return -1;
        fseek( fp, 0,  SEEK_END );
        len = ftell( fp );
        fclose(fp);
        return len;

    }   


