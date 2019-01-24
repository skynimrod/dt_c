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
    //      5.  getPages()  --- getObjContent(), getPageleaf()
    //      6.  getObjContent()
    //      7.  getPageleaf()
    //      8.  getCmapList() ---- getObjContent(),isType0()
    //      9.  isType0()
    //      10. getCMAPs() -----getObjContent(), getItemOfObj(),procType0Stream()
    //      11. getItemOfObj() -- getObjContent
    //      12. procType0Stream() ---  getItemOfObj(), decompress()
    //      13. decompress() --- import zlib(zlib.decomporess())
    //      14. processPage() ---- getItemOfObj()
    //      ##########################################
    //      #     下面的方法全部都是处理内容的, 包括解码, 格式调整等
    //      #  2016.12.21:
    //      #      删除了IFCTRL1() 判断换行的实现方式不再用之前的方法.
    //      # 15. decode()---  file_tools.initStreamBuf(), .readLineST( streambuf)
    //      #                  self.processBT(), .processBDC()
    //      # 16. processBT() -- self.processBDC(), .processRE(), /*processQ()*/
    //      #                    self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
    //      #                    file_tools.readLineST()
    //      # 17. processBDC() -- self.processBT(), .processRE(), /*processQ()*/
    //      #                    self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
    //      #                    file_tools.readLineST()
    //      # 18. hasText()
    //      # 19. processText()---  self.ifCRLF(), .processTj(), .processTJ()
    //      # 20. processTj() --- none
    //      # 21. processTJ() ---
    //      # 23. processRE()
    //      # 24. processQ()
    //      # 25. hasTxtPosition()
    //      # 26. precessTxtPosition()
    //      # 27. processTd()
    //      # 28. processTD()
    //      # 29. getOriginXY()
    //      # 30. processTm()
    //      # 31. processTstar()
    //      # 99. rlen()
    //      
    //  
    #include "file_tools.h"
    #include "filemap.h"
    #include "str_tools.h"
    #include "pdf_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    // 位置信息映射表, 记录已经处理过的xref位置信息, 防止重复处理,造成死循环
    typedef struct __posmap_ {
        long                xrefpos;
        struct __posmap_  * next;
    }POSMAP;

    // 交叉引用表  XREF 信息
    typedef struct __obj_pos_ {
        int                 objNo;          // 对象编号
        int                 pos;            // 对象地址
        struct __obj_pos_ * next;           // 下一个对象
    } OBJ_POS;

    typedef struct __xref__ {
        int                 retcode;
        int                 objTotal;   // 对象总数
        OBJ_POS           * objpos_p;
    } XREF;

    // trailer 信息中包含: 根object编号，object数量,  startxref( xref 位置信息)
    typedef struct __trailer__ {
        int         Root;       // 根对象ID
        int         Size;       // 对象数量. 比对象最大编号大1. 编号为0的对象 不是数据对象.
        // Encrypt    // 如果文件加密则有
        long        Info;       // 可选. 记录文档信息的对象ID.   作者信息等等
        char      * ID;         // 可选。 为了保证文件的唯一性, 强烈建议有该内容. 
        long        Prev;       // 前一个Trailer的地址.
        long        xrefStm;    // 可用的xref地址
        int         retcode;    // 额外添加的内容, 用来判断该Trailer是否有效
    } TRAILER;

    //**************************************************************************
    //  函数名称 : parsePDF()
    //  功    能 :
    //		解析 pdf文件, 将解析后的结果存放在desfile文件中.
    //  入口参数 :
    //       desfile :      解析后的内容存放的文件名
    //       srcfile :      需要解析的pdf文件
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         int         <0  表示出错, -1 表示文格式不对. >=0 表示找到xref位置
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
    //         用到的本文件内的API
    //             1. getStartxrefPos()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    int DLL_EXPORT parsePDF( char * desfile,  char * srcfile )
    {
        FILEMAP       * fm_p;
        long            xrefpos;                // xref 起始位置

        POSMAP      *   posmap_p = NULL;        //  链表, 因为无法确认实际有几个xref, 用来记录已经处理过的xref, 防止出现死循环
        XREF        *   xref_p = NULL;          //  存放有效地xref 信息.
        TRAILER     *   trailer_p = NULL;       //  存放 有效地trailer信息

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        xrefpos   = getStartXrefPos( fm_p );          // 获取startxref 的内容, 也就是第一个xref 的位置

        if ( xrefpos < 0 )
            return -1; 
       
        // posmap_p 初始化
        posmap_p = (POSMAP *) malloc( sizeof(POSMAP) );
        
        //XREF * getXREF( FILEMAP * fm_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
        //xref_p = (XREF *)getXREF( fm_p, xrefpos, posmap_p, trailer_p );
        xref_p = getXREF();

        return 0;
    }


    //一下为内部调用的API
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
        uchar     * buf = NULL;
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
                return startxrefpos;
                
             }
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
    TRAILER * getTrailer( char * buf )
    {
        TRAILER     *   trailer_p;
        char        *   buf;
        char        *   item;
        int             len;

        if ( !buf )             // 容错处理, 空指针不处理
            return NULL;

        trailer_p = ( TRAILER * )malloc( sizeof(TRAILER) );
        
        trailer_p->retcode = -1;        // 初始化为-1, 假设不能获取trailer 成功

        item = (char *)strtok( buf + 2, "/ " );     // +2 是为了过滤掉开始的"<<"
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

            if ( memcmp( item, "ID", 2 ) == 0 ) {  // 4. ID 项目 ID[<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]
                item = (char *)strtok( NULL, "/ " );          
                printf("ID 的值为:%s\n", item );

                len =  strlen( item ) - 2;          // -2 是不包括"ID"
                
                trailer_p->ID = (char *)malloc( len +1 );      
                memset( trailer_p->ID, 0, len + 1 );
                memcpy( trailer_p->ID, item, len );
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
        return trailer_p;
    }

    // 释放trailer指针内存
    void  freeTrailer( TRAILER * trailer_p )
    {
        if ( !trailer_p )           // 空指针不用释放内存
            return;

        if ( trailer_p->ID )
            free( trailer_p->ID );

        free( trailer_p );
        
        return;
    }

    // 调试用函数
    void print_trailer( TRAILER * trailer_p )
    {
        if ( !trailer_p ) {
            printf( "\n给定的trailer_p 是空指针！\n" );
            return;
        }

        printf("Size=%d\n", trailer_p->Size );
        printf("Root=%d\n", trailer_p->Root );
        printf("Info=%d\n", trailer_p->Info );
        if ( trailer_p->ID != NULL )
            printf("ID=%s\n", trailer_p->ID );
        printf("Prev=%d\n", trailer_p->Prev );
        printf("xrefStm=%d\n", trailer_p->xrefStm );
        printf("retcode=%d\n", trailer_p->retcode );
        return;
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
        POSMAP  *   tmp_p = posmap_p;   // 为了不破坏posmap_p 内容， 临时指针变量来遍历

        if ( posmap_p == NULL )       // 给定位置信息不在链表中
            return false;
        
        while( tmp_p != NULL ) {
            if ( xrefpos == tmp_p->xrefpos )    // 给定地址在链表中， 表示已经处理过了
                return true;
            tmp_p = (POSMAP *)tmp_p->next;
        }

        // 如果遍历完链表也没有找到, 说明该地址没有在链表中, 即对应的xref没有处理过
        return false;

    }

    XREF * getXREF( void )
    {
        XREF    * xref_p;

        return xref_p;

    }
    /*
    XREF * getXREF( FILEMAP * fm_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
    {
        XREF    *   xref_p;
        char    *   buf     = NULL;
        char    *   item    = NULL;     
        int         firstObj;       // xref 中第一行记录的第一个对象编号
        int         objSum;         //                    对象总数量
                
        if ( fm_p == NULL )
            return;

        xref_p = (XREF *)malloc( sizeof( XREF ) );

        xref_p->retcode = -1;

        if( ifXrefPosProcessed( xrefpos, posmap_p ) )   // 如果该地址已经处理过,  表示文件格式出错了, 直接返回xref_p (retcode=-1)
            return xref_p;
        
        // 该地址没有处理过, 把该地址添加到链表尾部
        posmap_p->next = (POSMAP *)malloc( sizeof(POSMAP) );
        posmap_p->next->xrefpos = xrefpos;

        fm_seek( fm_p, xrefpos );

        buf = fm_readLine( fm_p );

        if ( !buf || !strstr( buf, "xref" ) ) {     // 第一行没有"xref", 格式错误
            xref_p->retcode = -40421;
            return xref_p;
        }

        // 循环处理第二行和后续的， 如果后续的trailer 里面有prev, 则需要嵌套调用getXREF()
        while( 1 ) {
            if ( buf )          // 由于 fm_readLine() 内部会申请内存, 这儿需要先释放之前申请的内存
                free( buf );

            buf = fm_readLine( fm_p);

            if ( !buf )         // 获取数据出错
                return xref_p;

            // 如果包含trailer, 表示需要继续在trailer寻找下一个xrefpos, 然后再解析xref
            if ( strstr( buf, "trailer" ) )     // 退出循环， 直接处理trailer 
                break;

            // 开始解析xref数据
            item = (char *)strtok( buf, " " );     // 第一行: 1 5267 之类的格式(起始对象编号 对象数量)
            int     i = 0;
            while( item != NULL ) {
               if ( !IsNumberStr( item ) || i >= 2 )   {   // 如果不是数字 或超过2项， 表示格式不正确
                    free( buf );
                    return xref_p;
               }
                
                if ( i == 0 )
                    firstObj = atoi( item );                // 起始对象编号
                
                if ( i == 1 ) 
                    objSum = atoi( item );                  // 对象总数量
                
                item = (char *)strtok( NULL, " " );
                i ++;
            }
            free( buf );            // 释放内存, 后面还需要用使用buf指针

            // objSum < 1 的情况容错。 这种情况有可能, doc文件转存pdf后, 第一个xref 就是0 0, 
            // 后面的trailer指向新的xref, 这儿应该继续处理trailer， 如果没有Prev 就错了      
            if ( objSum < 1 ) {
                xref_p->retcode = -40424;       // 预设一个出错码
                buf = fm_readLine( fm_p );

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

                if ( trailer_p )        // 由于 getTrailer() 内部会申请内存, 所以这儿要释放之前可能申请的内存
                    freeTrailer( trailer_p ); 
                
                trailer_p = getTrailer( buf );
                if ( trailer_p->retcode < 0 || !strstr( buf, "Prev" ) || trailer_p->Prev <= 0 ) { 
                    // 无效的xref之后的trailer无效或没有包含有效的self.xref 地址, 则出错返回
                    free( buf );
                    freeTrailer( trailer_p );
                    return xref_p;
                }

                return getXREF( fm_p, trailer_p->Prev, posmap_p, trailer_p );
            }

            long        cpos;           // 用来计算对象的地址位置数据
            
            
            return xref_p;
        }
        
    }

    */
