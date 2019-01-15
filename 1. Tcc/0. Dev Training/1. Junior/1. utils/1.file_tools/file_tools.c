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


    //############################################################################
    //#  
    //# 下面是关于  FILEMAP  的接口
    //#
    //#     0.  printf_fm()       显示当前 fm 的内容, 调试用
    //#     1.  initReadFileMap()
    //#     2.  freeFileMap()  
    //
    void DLL_EXPORT print_fm( FILEMAP * fm_p )
    {
	    if ( fm_p == NULL ) {
		    printf("filemap is  NULL \n");
	        return;
	    }
            printf("filemap->filename=%s\n", fm_p->filename );
            printf("filemap->filelen=%d\n", fm_p->filelen );
            printf("filemap->pos=%d\n", fm_p->pos );

	    return;
    }

    // 注意, 用完该函数后, 一定要注意释放返回指针中的内存
    FILEMAP * DLL_EXPORT initReadFileMap( char * srcFile )
    {
	    FILEMAP     * fm_p;
       
        if ( !srcFile || ifOpened( srcFile ) != -2 )     // 空文件名或不存在的文件名就不处理了
              return NULL;

        fm_p = ( FILEMAP * )malloc( sizeof( FILEMAP ) );
    	printf("Size of FILEMAP=%d\n", sizeof(FILEMAP ) );
	    printf("size of long=%d, sizeof char *=%d\n", sizeof(long), sizeof(char *));
	
	    int	len = strlen( srcFile );
	    fm_p->filename = (char *) malloc( len + 1 );
	    memset( fm_p->filename, 0, len + 1 );
	    memcpy( fm_p->filename, srcFile, len );

	
	    fm_p->filelen   = getFileSize( srcFile );       // filelen    文件长度
        fm_p->buflen    = fm_p->filelen;             // buflen     等同于文件长度, 暂时没用
        fm_p->pos       = 0;                            // pos        位置指针, 初始化为0

	    print_fm( fm_p );
	    fm_p->buf       = (uchar *) malloc(  fm_p->buflen + 1  );   // 为缓冲区申请内存空间
	    printf( "fm_p->buflen=%d\n", fm_p->buflen );
	    memset( fm_p->buf, 0, fm_p->buflen + 1 ); // 置0

	    FILE  *fp = fopen( srcFile, "rb" );
	    int n = fread( fm_p->buf, 1, fm_p->buflen, fp );  // 把文件内容读取到内存映射缓冲区中
	    printf("读取的文件内容数量:%d\n", n);
	    fclose( fp );
	
	    //printf( "buf内容=%s\n", fm_p->buf );
	    return fm_p;
    }

    void DLL_EXPORT freeFileMap( FILEMAP * fm_p ) 
    {
    	if ( fm_p != NULL ) {
    	    if ( fm_p->filename != NULL ) {
    		    printf("filemap->filename 需要释放内存filename=%s!\n", fm_p->filename );
    	        free( fm_p->filename );
    		    printf("fm_p->filename 释放内存结束\n");
    	    }
    	    printf("free 1\n");
    	    
    	    
    	    if ( fm_p->buf != NULL  )
    	        free( fm_p->buf );
    	    printf("free 2\n");
    	    
    	    free( fm_p );
    	    printf("free 3\n");
        } else {
    	    printf("NULL, 不用free!\n");
    	}
    
    	return;
    }

