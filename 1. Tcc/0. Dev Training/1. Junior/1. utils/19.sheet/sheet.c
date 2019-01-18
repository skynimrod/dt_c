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
    //       涉及  file_tools.h
    // Interface :
    //      0.  printf_fm()       显示当前 fm 的内容, 调试用
    //      1.  initReadFileMap()
    //      2.  freeFileMap()  
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  使用 true, false 值的定义

    #include    "file_tools.h"                  // getFileSize()
    #include    "filemap.h"

    #define DLL_EXPORT __declspec(dllexport)

    //**************************************************************************
    //  函数名称 : print_fm()
    //  功    能 :
    //		显示  filemap 指针对应的内容， 用于调试。
    //  入口参数 :
    //            fm_p : FILEMAP * 指针
    //  出口参数 :
    //            无
    //  返 回 值 :
    //           无
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			            
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
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

    //**************************************************************************
    //  函数名称 : initReadFileMap()
    //  功    能 :
    //		初始化 FILEMAP 指针，主要是申请内存, 并将文件内容按字节流方式写入 内存中.
    //  入口参数 :
    //            srcFile :  字符串。 文件名
    //  出口参数 :
    //            无
    //  返 回 值 :
    //           FILEMAP 指针
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //          注意, 用完该函数后, 一定要注意释放返回指针中的内存, 需要调用freeFileMap()
    //	Standard .h  :
    //          file_tools.h  /  file_tools.dll  ------------- getFileSize()          
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    FILEMAP * DLL_EXPORT initReadFileMap( char * srcFile )
    {
	    FILEMAP     * fm_p;
       
        if ( !srcFile || ifOpened( srcFile ) == -2 )    {// 空文件名或不存在的文件名就不处理了
            //printf("无效文件名!\n");
            return NULL;
        }

        fm_p = ( FILEMAP * )malloc( sizeof( FILEMAP ) );
    	//printf("Size of FILEMAP=%d\n", sizeof(FILEMAP ) );
	    //printf("size of long=%d, sizeof char *=%d\n", sizeof(long), sizeof(char *));
	
	    int 	len = strlen( srcFile );

	    fm_p->filename = (char *) malloc( len + 1 );
	    memset( fm_p->filename, 0, len + 1 );
	    memcpy( fm_p->filename, srcFile, len );

	
	    fm_p->filelen   = getFileSize( srcFile );       // filelen    文件长度
        fm_p->streamlen = fm_p->filelen;                // buflen     等同于文件长度, 暂时没用
        fm_p->pos       = 0;                            // pos        位置指针, 初始化为0

	    print_fm( fm_p );
	    fm_p->stream       = (uchar *) malloc(  fm_p->streamlen + 1  );   // 为缓冲区申请内存空间
	    //printf( "fm_p->streamlen=%d\n", fm_p->streamlen );
	    memset( fm_p->stream, 0, fm_p->streamlen + 1 ); // 置0

	    FILE  *fp = fopen( srcFile, "rb" );
	    long n = fread( fm_p->stream, 1, fm_p->streamlen, fp );  // 把文件内容读取到内存映射缓冲区中
	    //printf("读取的文件内容数量:%d\n", n);
	    fclose( fp );

	    //printf( "stream内容=%s\n", fm_p->stream );
	    return fm_p;
    }

    //**************************************************************************
    //  函数名称 : freeFileMap()
    //  功    能 :
    //		释放 FILEMAP 指针，主要是释放内存
    //  入口参数 :
    //            fm_p : FILEMAP指针
    //  出口参数 :
    //            无
    //  返 回 值 :
    //           无
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //          注意, 用完FILEMAP后, 一定要注意释放内存, 需要调用freeFileMap()
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    void DLL_EXPORT freeFileMap( FILEMAP * fm_p ) 
    {
    	if ( fm_p != NULL ) {
    	    if ( fm_p->filename != NULL ) {
    		    //printf("filemap->filename 需要释放内存filename=%s!\n", fm_p->filename );
    	        free( fm_p->filename );
    		    //printf("fm_p->filename 释放内存结束\n");
    	    }
    	    //printf("free 1\n");
    	    
    	    
    	    if ( fm_p->stream != NULL  )
    	        free( fm_p->stream );
    	    //printf("free 2\n");
    	    
    	    free( fm_p );
    	    //printf("free 3\n");
        } else {
    	    ;
            //printf("NULL, 不用free!\n");
    	}
    
    	return;
    }

    // 获取内存文件的属性，直接访问FILEMAP 的的成员变量即可
    //  1. 文件名称   2. 文件长度    3. 
    //
    
    //内存映射文件的操作方法, 读取当前操作文件的一行, 并自动移动指针
    //**************************************************************************
    //  函数名称 : fm_readLine()
    //  功    能 :
    //		访问内存文件， 获取 当前位置开始的一行, 以回车或换行为结束符
    //  入口参数 :
    //            fm_p : FILEMAP指针
    //  出口参数 :
    //            无
    //  返 回 值 :
    //           uchar  *
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //          注意, 用完FILEMAP后, 一定要注意释放内存, 需要调用freeFileMap()
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    uchar * DLL_EXPORT fm_readLine( FILEMAP *fm_p )
    {
        long        curpos;         // 记录内存文件的当前位置
        uchar       ch;
        uchar   *   retbuf = NULL;
        
        if ( fm_p->pos < 0 )               // 容错处理, 如果当前位置为负数， 则重置为头部0
            fm_p->pos = 0;

        curpos = fm_p->pos;

        if ( curpos >= fm_p->streamlen )    // 如果到结尾或超出结尾位置， 返回空
            return NULL;

        for ( long i = curpos; i <= fm_p->streamlen; i ++ ) {
            ch = fm_p->stream[i];
            
            if ( ch == 13 || ch == 10 ) {  //  找到换行回车了   13 = "\0x0D", 10 = "0x0A"
                // 先把\0A或\0D之前的数据获取出来
                retbuf = ( uchar * ) malloc( i-curpos + 1 );
                memset( retbuf, 0, i-curpos+1 );

                memcpy( retbuf, &fm_p->stream[curpos], i-curpos );

                // 继续往后面找找， 如果是\0D 或 \0A 继续跳过
                          
                curpos = i;      // curpos  赋值为当前的位置
                ch = fm_p->stream[curpos];

                while ( ch == 13 || ch == 10 ) {     // 继续跳过 后面的回车或换行
                    curpos += 1;
                    ch = fm_p->stream[curpos];
                }
                fm_p->pos = curpos;
                break;
            }
        }
        return retbuf;
        
    }

    //**************************************************************************
    //  函数名称 : fm_read()
    //  功    能 :
    //		访问内存文件， 获取 当前位置开始指定长度的字节流
    //  入口参数 :
    //            fm_p : FILEMAP指针
    //            length :  需要获取的字节流长度
    //            start  :  起始位置缺省为-1, 表示从当前位置读取, 否则是指定的位置
    //  出口参数 :
    //           无
    //  返 回 值 :
    //           uchar  *   获取的字节流
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //          注意, 返回的指针有可能是申请内存的， 使用完后要释放内存
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    uchar * DLL_EXPORT fm_read( FILEMAP *fm_p, long length, long start )
    {
        long    buflen, rlen, rpos;
        uchar  *desbuf;

        buflen  = fm_p->streamlen;
        rlen    = length;

        //printf(  "start=%d, len=%d, buflen=%d\n", start, length, buflen  );        

        if ( start > buflen )           //       如果越界就返回NULL
                return NULL;

        //起始位置的处理
        if ( start == -1 )
            rpos = fm_p->pos; 
        else 
            rpos = fm_p->pos = start;
            
        // 实际读取长度的处理
        // printf( "start=%d(%08x), len=%d, buflen=%d" , rpos, rpos, length, buflen );
        if ( ( rpos + length ) > buflen )
            rlen = buflen - start;

        desbuf = ( uchar * )malloc( rlen + 1 );
        memset( desbuf, 0, rlen + 1 );
        memcpy( desbuf, &fm_p->stream[rpos], rlen );

        fm_p->pos = rpos + rlen;                //  读取后修改当前的位置

        return desbuf;
    }

    //**************************************************************************
    //  函数名称 : fm_seek()
    //  功    能 :
    //		似文件操作的定位功能， 不过这儿的定位很简单，就是修改缓冲区中的游标位置pos
    //      做了一些冗余， 定位如果是负数， 则从0开始 (这个未来可能会改变，即负数可以从后往前数)
    //      如果长度大于缓冲去长度(文件长度)，也就是定位到最后了. 
    //         如果是正数，则表示是绝对位置。
    //  入口参数 :
    //            fm_p : FILEMAP指针
    //            pos  : 是指定的位置
    //  出口参数 :
    //           无
    //  返 回 值 :
    //           uchar  *   获取的字节流
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //          注意, 返回的指针有可能是申请内存的， 使用完后要释放内存
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    void DLL_EXPORT fm_seek( FILEMAP  * fm_p, long pos )
    {
        long      rpos = pos;

        //printf("\n指定的位置为%d\n", pos);
        
        if ( pos < 0 ) {                        // 指定位置为负数，表示回退指定的字节书
            //printf("\nf负的能处理吗?\n");
            rpos = fm_p->pos + pos;             // 因为是负数， 所以相加才能回退
            if ( rpos < 0 )                     //  表示回退范围超出了头部， 设置为0
                rpos = 0;
        }
        
        if ( pos >= fm_p->streamlen )           // pos 是非负数, 则表示绝对位置, 要判断是否超出尾部, 超出则设为尾部
            rpos = fm_p->streamlen - 1;          // 下标是从0开始的， 所以要减1
        
        fm_p->pos = rpos;                       // 设置位置
    }

