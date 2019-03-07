    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    pdf_tools
    //
    // Description : about pdf parse operation.
    //
    // Interface :
    //      1.  parsePDF()  -- getStartxrefPos(), getxref(), getPages(), getCmapList, getCMAPs()
    //
    // 下面的仅供本文件内部使用
    //      2.  getStartxrefPos()
    //      3.  getxref()  ---- getTrailer()
    //      4.  getTrailer()
    //  
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "pdf_tools.h"

    int     getXREF( FILEMAP * fm_p, XREF * xref_p,  long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p );
    //**************************************************************************
    //  函数名称 : getStartXrefPos()
    //  功    能 :
    //		获取xref 位置信息
    //  入口参数 :
    //       FILEMAP    * fm_p
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         long         <0  表示出错, -1 表示文件映射指针不可用. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			            
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    long getStartXrefPos( FILEMAP * fm_p )
    {
        uchar     * buf;
        long        startxrefpos = -3;
         
        if ( !fm_p )       // 如果文件映射指针为空则返回-1
            return -1;
         
        // 先定位到距离文件尾48个字节处, 一般xref在文件尾
        fm_seek( fm_p, fm_p->filelen - 48 );   

        while ( 1 ) {
            buf = fm_readLine( fm_p );
            if ( !buf )        
                return -2;             // 读取不到xref,文件格式可能不正确

            if ( strstr( buf, "startxref" ) ) {  // 如果没有就会返回空指针, 包含的话就会返回位置
                free( buf );       // 释放buf内存，重复利用
                buf = fm_readLine( fm_p);   // "startxref" 所在行的下一行就是实际的xref所在的位置信息

                if ( !buf )             // 如果buf 为NULL, 则文件格式不正确
                    return -2;
                
                if ( !IsNumberStr( buf ) ) {     // 如果不是数字字符串, 则仍然是文件格式不正确, 注意, 这儿要释放内存
                    free( buf );
                    return -2;
                }
                
                startxrefpos =  atol( buf );
                free( buf );
                return startxrefpos;
                
            }
            free( buf );
        }
    }
    //**************************************************************************
    //  函数名称 : getTrailer()
    //  功    能 :
    //		获取 Trailer 信息
    //  入口参数 :
    //       FILEMAP    * fm_p
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         long         <0  表示出错, -1 表示文件映射指针不可用. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		xref 的完整构成是   xref + trailer.  如果有多个xref, 那么就有多个trailer.	             
    //		<</Size 51387/Root 1 0 R/Info 50568 0 R/ID[<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]/Prev 3275241/xrefStm 771024>>\r\n
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    int getTrailer( char * buf, TRAILER * trailer_p )
    {
        char        *   item;
        int             len;

        if ( !buf )             // 容错处理, 空指针不处理
            return -1;

        trailer_p->retcode = -1;        // 初始化为-1, 假设不能获取trailer 成功

        item = (char *)strtok( buf , "/ " );     //  第一项可能是"trailer<<"或"<<", 丢弃
        int     i = 0;
        while( item != NULL ) {
            printf("\n第%d个项目:%s\n", i+1, item );

            if ( memcmp( item, "Size", 4 ) == 0 ) {  // 1. Size 项目
                item = (char *)strtok( NULL, "/ " );          
                printf("Size 的值为:%s\n", item );
                trailer_p->Size = atoi( item);
            }

            if ( memcmp( item, "Root", 4 ) == 0 ) {  // 2. Root 项目
                item = (char *)strtok( NULL, "/ " );          
                printf("Root 的值为:%s\n", item );
                trailer_p->Root = atoi( item);

                item = (char *)strtok( NULL, "/ " );    // 跳过后面的" 1 R"
                item = (char *)strtok( NULL, "/ " );    // 
            }

            if ( memcmp( item, "Info", 4 ) == 0 ) {  // 3. Info 项目
                item = (char *)strtok( NULL, "/ " );          
                printf("Info 的值为:%s\n", item );
                trailer_p->Info = atoi( item);
            }

            if ( memcmp( item, "ID", 2 ) == 0 ) {  // 4. ID 项目 ID [<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]>>
                item = (char *)strtok( NULL, "/ " );          
                printf("ID 的值为:%s\n", item );

                len = 0;
                while ( len < strlen( item ) && !( item[len] == '>' && item[len+1] == '>' ) ) //  处理尾部的">>" 
                    len ++;
                
                memset( trailer_p->ID, 0, L_ID + 1 );     
                memcpy( trailer_p->ID, item, len );       // [<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]
            }

            if ( memcmp( item, "Prev", 4 ) == 0 ) {  // 5. Prev 项目
                item = (char *)strtok( NULL, "/ " );          
                printf("Prev 的值为:%s\n", item );
                trailer_p->Prev = atoi( item);
            }

            if ( memcmp( item, "xrefStm", 4 ) == 0 ) {  // 6. xrefStm 项目
                item = (char *)strtok( NULL, "/ " );          
                printf("xrefStm 的值为:%s\n", item );
                item[ strlen(item) -2 ] = 0;            // 过滤掉尾部的">>"
                trailer_p->xrefStm = atoi( item);
            }

            item = (char *)strtok( NULL, "/ ");
            i ++;
        } 

        trailer_p->retcode = i;         // 获得的有效项目的数量
        return 0;
    }

    //**************************************************************************
    //  函数名称 : getXREF()
    //  功    能 :
    //		获取 XREF 信息
    //  入口参数 :
    //       FILEMAP    * fm_p
    //  出口参数 :
    //       
    //  返 回 值 :
    //         long         <0  表示出错, -1 表示文件映射指针不可用. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		xref 的完整构成是   xref + trailer.  如果有多个xref, 那么就有多个trailer.	 
    //		这个函数时嵌套函数, 因为有可能有多个xref, 只有找到有效的那个xref才行.           
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    //  ifXrefPosProcessed()  是内部函数, 用来判断xrefpos地址对应的xref 是否已经被处理过, 防止循环处理同一个xref变成死循环
    bool    ifXrefPosProcessed( long xrefpos, POSMAP * posmap_p )
    {   
        int         i = 0 ;
        
        if ( posmap_p == NULL )       // 给定位置信息不在链表中
            return false;
        
        while ( i < posmap_p->cursor ) {    // 
            if ( xrefpos == posmap_p->xrefpos[i] )
                return true;
            i ++;
        }

        // 如果遍历完链表也没有找到, 说明该地址没有在链表中, 即对应的xref没有处理过
        return false;

    }

    // XREF 数据区第一行的处理, 判断是否已经处理过该地址指向的xref(容错处理, 防止死循环)
    int procFirstLine( FILEMAP *fm_p, long xrefpos, XREF * xref_p, POSMAP * posmap_p )
    {
        char        * buf;
        
        xref_p->retcode = -1;

        if( ifXrefPosProcessed( xrefpos, posmap_p ) )   // 如果该地址已经处理过,  表示文件格式出错了, 直接返回xref_p (retcode=-1)
            return -1;
        
        // 该地址没有处理过, 把该地址添加到链表尾部
        posmap_p->xrefpos[ posmap_p->cursor ] = xrefpos;
        posmap_p->cursor += 1;

        fm_seek( fm_p, xrefpos );

        buf = fm_readLine( fm_p );

        if ( !buf || !strstr( buf, "xref" ) ) {     // 第一行没有"xref", 格式错误
            xref_p->retcode = -40421;
            if ( buf )
                free( buf );
            return -1;
        }

        free( buf );          // 由于 fm_readLine() 内部会申请内存, 这儿需要先释放之前申请的内存
        
        xref_p->retcode = 0;

        return 0;
    }

    // XREF 数据区第二行的处理, 获取 对象数量, 第一个对象编号
    // 返回至:  -1  表示出错,  0 表示获取成功
    int procSecondLine( FILEMAP * fm_p, int   * firstObj, int * objSum, TRAILER * trailer_p ) 
    {
        char    *   buf;
        char    *   item;
        int         ret;
        char    *   tmpbuf;

        buf = fm_readLine( fm_p);

        if ( !buf )         // 获取数据出错
            return -1;

        // 如果包含trailer, 表示需要继续在trailer寻找下一个xrefpos, 然后再解析xref
        if ( strstr( buf, "trailer" ) ) {     // 处理trailer 
            printf("--------- 第二行有trailer....\n");
            ret =  getTrailer( buf, trailer_p );
            free( buf );
            return 1;
        }

        // 开始解析xref数据区第二行 1 5267 之类的格式(起始对象编号 对象数量), 如果是 0 1 则说明后面要根据后面trailer的地址继续检索
        tmpbuf = (char *)strsplit1( buf, ' ', 2 );
        *objSum = atoi( tmpbuf );     // 起始对象编号
        free( tmpbuf);

        tmpbuf = (char *)strsplit1( buf, ' ', 1 );
        *firstObj = atoi( tmpbuf );   // 对象总数量
        free( tmpbuf );

        free( buf );            // 释放内存

        printf( "\nfirstObj=%d, objSum=%d\n", *firstObj, *objSum);
        
        return 0;
    }

    // 处理objSum <= 1 的情况.  容错处理, 有可能是0 0  或者  0 1. 如果是0  1, 就要多跳过一行
    void procObjSumLT1( FILEMAP *fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p, int objSum )
    {
        char    *   buf;
        int         ret;

        xref_p->retcode = -40424;       // 预设一个出错码
        buf = fm_readLine( fm_p );
    
        if ( objSum == 1 ) {            // 1 1 的第一行略过
            free( buf );
            buf = fm_readLine( fm_p );
        }

        if ( !buf )                    // 无效格式
            return xref_p;
        
        if ( !strstr( buf, "trailer" ) ) {   // 没有"trailer"的话就是无效格式
            free( buf );
            return xref_p;
        }

        if ( !strstr( buf, "Root" ) ) { // 如果trailer.<<...>>格式的话, trailer的内容实际上在下一行
            free( buf );
            buf = fm_readLine( fm_p );
            if ( !buf )                 // 如果没有下一行数据, 无效格式
                return xref_p;     
        }
        
        ret = getTrailer( buf, trailer_p );

        if ( trailer_p->retcode < 0 || trailer_p->Prev <= 0 ) { 
            // 无效的xref之后的trailer无效或没有包含有效的self.xref 地址, 则出错返回
            free( buf );
            return xref_p;
        }

        free( buf );

        xref_p->retcode = 0;

        ret = getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
    }

    // 获取每个对象的位置信息, 
    // 注意:
    //      对象编号从1开始, 第一行是: 0000000000 65535 f   不是实际对象, 而是对象信息说明表示最多有65535个对象
    long * getOBJPOS( FILEMAP * fm_p, int objSum )
    {
        char        *   buf;
        char        *   item;
        long        *   objpos_p;

        objpos_p = ( long * )calloc( sizeof(long) * objSum, 1 );       // 指向一个long数组, 长度未objSum

        // 只有 对象编号 < 对象总数  才进行处理
        for ( int i =0; i < objSum; i ++ ) {
            buf = fm_readLine( fm_p );      // 获取的应该是 0000000016 00000 n类似格式

            item = (char *)strtok( buf, " " );          // 其实只要获取第一项就可以, 第一项是地址， 全部获取是为了容错
            int   j = 0;
            while ( item ) {
                if ( j == 0 || j == 1 ) {           // 第一,二项应该是数字, 容错处理
                    if ( !IsNumberStr( item ) )  {
                        free( buf );
                        return NULL;
                    }
                }
            
                if ( j == 0 )
                    objpos_p[i] = atoi( item );              // 对象数据的位置, i 为 对象编号-1, 因为i 从0开始, 对象编号从1开始
            
                item = (char *)strtok( NULL, " " );
                j ++;
            }

            if (  j > 3 )   {   // 如果超过3项， 表示格式不正确
                free( buf );
                return NULL;
            }

            free( buf );            // 释放内存, 后面还需要用使用buf指针
        }

        return objpos_p;
    }

    int procTrailer( FILEMAP * fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p )
    {
        char     *  buf;
        int         ret;

        buf = fm_readLine( fm_p );
        if ( !buf )
            return;

        if ( !strstr( buf, "Root" ) ) { // 如果trailer.<<...>>格式的话, trailer的内容实际上在下一行
            free( buf );
            buf = fm_readLine( fm_p );
        }

        ret = getTrailer( buf, trailer_p );

        //print_trailer( trailer_p );
        
        if( trailer_p->retcode < 0 ) {
            free( buf );
            return -1;
        }

        if ( strstr( buf, "Prev" ) ) {
            ret = getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
            free( buf );
            return 0;
        }

        free( buf );
        
        return 0;
    }


    int getXREF( FILEMAP * fm_p, XREF * xref_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
    {
        int         firstObj;       // xref 中第一行记录的第一个对象编号
        int         objSum;         //                    对象总数量
        int         ret;
                
        if ( fm_p == NULL )
            return -1;

        // 1. 第一行的处理. 判断是否已经处理过该地址指向的xref(容错处理, 防止死循环), 容错是否有"xref"
        ret = procFirstLine( fm_p, xrefpos, xref_p, posmap_p );
        if ( ret != 0 ) 
            return -1;

        // 2. 第二行的处理.  获取 对象数量, 第一个对象编号
        ret = procSecondLine( fm_p, &firstObj, &objSum, trailer_p );       // 为了增加可读性, 将第一行的处理封装在一个函数中
        if ( ret < 0 )                     // 返回值不等于0 表示出错了,直接返回xref_p,  xref_p->retcode =-1(初始化de值), 也就是表示出错了
            return -1;
        if ( ret == 1 ) {               // 如果返回1, 表示找到trailer了, 说明需要继续找下一个xref, 嵌套调用getXREF(), 后面的处理跳过
            if ( !trailer_p || trailer_p->retcode < 0 ) {            // 如果trailer无数据, 则出错
                return -1;
            }
            return getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
        }

        // 3. objSum < 1 的情况容错处理。 这种情况有可能, doc文件转存pdf后, 第一个xref 就是0 0, 
        // 后面的trailer指向新的xref, 这儿应该继续处理trailer， 如果没有Prev 就错了      
        if ( objSum <= 1 ) {
            printf( "objSum<=1 , objSum=%d\n", objSum );
            procObjSumLT1( fm_p, xref_p, posmap_p, trailer_p, objSum );
            return 0;
        }

        xref_p->objTotal = objSum;
        // 4. 下面的xref数据区就是每个对象的位置信息, 对象编号从1开始
        xref_p->objpos_p = getOBJPOS( fm_p, objSum );
        if ( xref_p->objpos_p == NULL ) {
            printf("位置信息没返回！\n");
            xref_p->retcode = -40425;       // 对象位置信息 格式错误
            return -1;
        }

        /*
        for ( int i=0; i < xref_p->objTotal; i ++ ) {
            printf("xref_p->objpos_p[%d]=%016d\n",i, xref_p->objpos_p[i] );
        }
        */
        
        // 5. 获取完 对象位置数据后, 处理trailer 部分数据
        ret = procTrailer( fm_p, xref_p, posmap_p, trailer_p );

        //print_trailer(trailer_p);

        if ( !trailer_p )
            printf("没有有效传递trailer\n");

        return 0;
        
    }



