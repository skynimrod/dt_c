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

    // 
    typedef struct __posmap__{
        int     cursor;                 // 游标, 最后一个登记的xref地址位置后面的位置
        long    xrefpos[128];           // 假设最大可能有128个xref地址
    }POSMAP;

    // 交叉引用表  XREF 信息
    typedef struct __xref__ {
        int                 retcode;
        int                 objTotal;   // 对象总数
        long              * objpos_p;       // 对象地址指针, objpos_p[0]的内容的内容就是第1个对象的地址
    } XREF;

    // trailer 信息中包含: 根object编号，object数量,  startxref( xref 位置信息)
    #define ID_LEN  128             // 设定 Trailer中的ID 最大长度为128， 一般是不会超过这个长度
    typedef struct __trailer__ {
        int         Root;       // 根对象ID
        int         Size;       // 对象数量. 比对象最大编号大1. 编号为0的对象 不是数据对象.
        // Encrypt    // 如果文件加密则有
        long        Info;       // 可选. 记录文档信息的对象ID.   作者信息等等
        char        ID[ID_LEN+1];         // 可选。 为了保证文件的唯一性, 强烈建议有该内容. 
        long        Prev;       // 前一个Trailer的地址.
        long        xrefStm;    // 可用的xref地址
        int         retcode;    // 额外添加的内容, 用来判断该Trailer是否有效
    } TRAILER;

    #define L_FONTNAME      64
    typedef struct __fontmap__ {    // 一个字体占用一个FONTMAP
        int         obj;                        // type0 字体对应的对象编号
        char        fontname[L_FONTNAME+1];     // type0 字体名称
    }FONTMAP;

    typedef struct __pages__ {
        int         total;      // 总页数
        int         cursor;     // 最后一个叶子页面对象存储在leafs数组中的位置(下标), 这个是为了便于处理
        int     *   leafs_p;        // 数组, 用来存放叶子页面对象编号
        int     **  c_objlist_p;    // leafs_p中的叶子对象包含的内容对象列表 c_objlist_p[total][对象列表]
        FONTMAP **  fontmap_p;      // fontname[页数量total][多少个字体][FONTMAP]
    }PAGES;

    // 下面是 CMAP 部分的数据结构定义
    typedef struct __cmap__ {
        int         code;
        int         decode;
    } CMAP;

    typedef struct __cmaplist__ {
        int         pageNo;     // 页面编号
        int         count;      // CMAP 数量
        CMAP        * cmaps_p;  // 该页面使用的CMAPs, 可能是多个
    }CMAPLIST;

    typedef struct __contentMap__ {     // 记录每页中的内容页面对象
        int         obj;            // 内容对象编号
        int         * objlist;      // 内容对象编号所含的对象列表
    }CONTENTMAP;

    int     getXREF( FILEMAP * fm_p, XREF * xref_p,  long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p );
    
    void    freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p );
    void    print_trailer( TRAILER * trailer_p );
    int     getPages( FILEMAP * fm_p, XREF * xref_p,  PAGES *pages_p, int Root );
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );

    int     getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    
    int     getCMAPLIST( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p, CMAPLIST * cmaplist_p );

    // 下面的函数为测试函数, 用来把相关信息输出到文件中, 便于查阅
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

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
        int             ret;
        FILEMAP       * fm_p;
        long            xrefpos;                // xref 起始位置

        POSMAP      *   posmap_p;               //  因为无法确认实际有几个xref, 用来记录已经处理过的xref, 防止出现死循环。 最大128个, 
        XREF        *   xref_p = NULL;          //  存放有效地xref 信息.
        TRAILER     *   trailer_p ;             //  存放 有效地trailer信息

        PAGES       *   pages_p;                //  存放页面对应内容存放的对象信息(Content对象)

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        xrefpos   = getStartXrefPos( fm_p );          // 获取startxref 的内容, 也就是第一个xref 的位置

        if ( xrefpos < 0 ) {
            freeFileMap( fm_p );
            return -1; 
        }
       
        // posmap_p 初始化
        posmap_p = (POSMAP *)malloc( sizeof(POSMAP) + 1 );
        memset( posmap_p, 0, sizeof(POSMAP) + 1 );
        
        trailer_p = (TRAILER *) malloc( sizeof(TRAILER) +1  );
        memset( trailer_p, 0, sizeof(TRAILER) + 1 );

        xref_p = (XREF *)malloc( sizeof(XREF) + 1 );
        memset( xref_p, 0, sizeof(XREF) + 1);

        // 1. 获取XREF 和 Trailer
        ret = getXREF( fm_p, xref_p, xrefpos, posmap_p, trailer_p );

        if ( xref_p->retcode < 0 || trailer_p->retcode < 0 ) {
            printf("出错了\n");
            freeAll( posmap_p, trailer_p, xref_p, pages_p, fm_p );
            return -1;
        }
        print_trailer( trailer_p ); 
        printXREFContent( fm_p, xref_p, "F:/F_T_tmp/tmp1/xrefAll_c.txt" );

        // 2. 下面获取页面 叶子对象信息 PAGES->leafs_p
        pages_p = (PAGES *)malloc( sizeof(PAGES) + 1 );
        memset( pages_p, 0, sizeof(pages_p) + 1);

        ret = getPages( fm_p, xref_p, pages_p, trailer_p->Root );

        // 下面是CMAP 相关的处理
        /*
        CMAPLIST    *cmaplist_p;
        cmaplist_p = (CMAPLIST *)malloc(sizeof(CMAPLIST)+1);
        memset( cmaplist_p, 0, sizeof(CMAPLIST) + 1 );

        getCMAPLIST( fm_p, xref_p, pages_p, cmaplist_p );
        */
        // 3. 获取每页对应的内容对象列表 contentMap
        getContentMap( fm_p, xref_p, pages_p );
        getFontMap( fm_p, xref_p, pages_p );


        
        freeAll( posmap_p, trailer_p, xref_p, pages_p, fm_p );

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
                
                memset( trailer_p->ID, 0, ID_LEN + 1 );     
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

    // 调试用函数
    void print_trailer( TRAILER * trailer_p )
    {
        if ( !trailer_p ) {
            printf( "\n给定的trailer_p 是空指针！\n" );
            return;
        }

        printf("Size=%d\n",     trailer_p->Size );
        printf("Root=%d\n",     trailer_p->Root );
        printf("Info=%d\n",     trailer_p->Info );
        printf("ID=%s\n",       trailer_p->ID );
        printf("Prev=%d\n",     trailer_p->Prev );
        printf("xrefStm=%d\n",  trailer_p->xrefStm );
        printf("retcode=%d\n",  trailer_p->retcode );
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

        if ( buf )          // 由于 fm_readLine() 内部会申请内存, 这儿需要先释放之前申请的内存
            free( buf );

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

        // 开始解析xref数据区第二行
        item = (char *)strtok( buf, " " );     // 第一行: 1 5267 之类的格式(起始对象编号 对象数量)
        int     i = 0;
        while( item != NULL ) {
            printf( "\nitem-%s\n", item );
            if ( !IsNumberStr( item ) || i >= 2 )   {   // 如果不是数字 或超过2项， 表示格式不正确
                free( buf );
                return -1;
            }
            
            if ( i == 0 )
                *firstObj = atoi( item );                // 起始对象编号
            
            if ( i == 1 ) 
                *objSum = atoi( item );                  // 对象总数量
            
            item = (char *)strtok( NULL, " " );
            i ++;
        }
        free( buf );            // 释放内存

        printf( "\nfirstObj=%d, objSum=%d\n", *firstObj, *objSum);
        
        return 0;
    }

    // 处理objSum < 1 的情况.  容错处理
    void procObjSumLT1( FILEMAP *fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p )
    {
        char    *   buf;
        int         ret;

        printf("--in procObjSumLT1()\n");

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

        ret = getTrailer( buf, trailer_p );

        if ( trailer_p->retcode < 0 || !strstr( buf, "Prev" ) || trailer_p->Prev <= 0 ) { 
            // 无效的xref之后的trailer无效或没有包含有效的self.xref 地址, 则出错返回
            free( buf );
            return xref_p;
        }

        if ( buf )
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

        objpos_p = ( long * )malloc( sizeof(long) * objSum );       // 指向一个long数组, 长度未objSum

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
        
        return 0;
    }


    int getXREF( FILEMAP * fm_p, XREF * xref_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
    {
        int         firstObj;       // xref 中第一行记录的第一个对象编号
        int         objSum;         //                    对象总数量
        int         ret;
                
        if ( fm_p == NULL )
            return -1;

        // 1. 第一行的处理
        ret = procFirstLine( fm_p, xrefpos, xref_p, posmap_p );
        if ( ret != 0 ) 
            return -1;

        // 2. 第二行的处理
        ret = procSecondLine( fm_p, &firstObj, &objSum, trailer_p );       // 为了增加可读性, 将第一行的处理封装在一个函数中
        if ( ret < 0 )                     // 返回至不等于0 表示出错了,直接返回xref_p,  xref_p->retcode =-1(初始化de值), 也就是表示出错了
            return -1;
        if ( ret == 1 ) {               // 如果返回1, 表示找到trailer了, 说明需要继续找下一个xref, 嵌套调用getXREF(), 后面的处理跳过
            if ( !trailer_p || trailer_p->retcode < 0 ) {            // 如果trailer无数据, 则出错
                return -1;
            }
            printf("\n到这儿了？\n");
            return getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
        }

        // 3. objSum < 1 的情况容错处理。 这种情况有可能, doc文件转存pdf后, 第一个xref 就是0 0, 
        // 后面的trailer指向新的xref, 这儿应该继续处理trailer， 如果没有Prev 就错了      
        if ( objSum < 1 ) {
            procObjSumLT1( fm_p, xref_p, posmap_p, trailer_p );
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

        
        for ( int i=0; i < xref_p->objTotal; i ++ ) {
            printf("xref_p->objpos_p[%d]=%016d\n",i, xref_p->objpos_p[i] );
        }
        
        // 5. 获取完 对象位置数据后, 处理trailer 部分数据
        ret = procTrailer( fm_p, xref_p, posmap_p, trailer_p );

        //print_trailer(trailer_p);

        if ( !trailer_p )
            printf("没有有效传递trailer\n");

        return 0;
        
    }

    // ------------获取所有的页 对应的对象的信息
    // 获取指定对象中的指定item的内容
    char * getItemOfObj( FILEMAP * fm_p, XREF *xref_p, int obj, char *item ) 
    {
        char    * buf;
        char    * tmpbuf;
        char    * value;
        int     len;
        
        buf = (char *)getObjContent( fm_p, xref_p, obj );
        if ( !buf )
            return NULL;

        tmpbuf = (char *)strtok( buf, "/" );
        while ( tmpbuf != NULL ) {
            if ( strstr( tmpbuf, item ) ) {     // 找到了包含item 的项目 
                len = strlen( tmpbuf );
                value = ( char * )malloc( len + 1 );
                memset( value, 0, len + 1 );
                memcpy( value, tmpbuf, len );
                
                free( buf );
                return value;
            }
            tmpbuf = (char *)strtok( NULL, "/" );
        }

        free( buf );
        return NULL;

    }

    /* 根据对象编号获取对象的内容, 使用内存映射文件 
     * 入口参数:
     *      obj     是整数字符串 ， 对应的是对象编号
     *      self.xref    self.xref哈希表， 存放的是 对象编号:对象物理位置
     * 返回:
     *      对象的文字描述内容部分.
     * 说明:
     *      对象的描述部分， 第一行是 3 0 R , 也就是对象编号等信息, 直接过滤即可. 
     *      第二行才是对象的描述性文本内容.
     * 2016.12.19:
     *      修正, 对于有 \xae 类似的内容, 需要额外处理.
     * 调用的外部方法有:
     *      1. seek() ------------------>util_fileService
     *      2. readLine() -------------->util_fileService
     * 调用的类对象内部的方法有:
     *      1. getObjPosInself.xref()
     */
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo )
    {
        char    * buf;

        if ( !fm_p || !xref_p || objNo <= 0 )       // 无效参数
            return NULL;

        fm_seek( fm_p, xref_p->objpos_p[objNo] );       //由于数组下标是从0开始， 对象1对应的下标是0
        
        buf = fm_readLine( fm_p );      // 第一行是  3 0 R  也就是对象编号
        if ( !buf ) 
            return NULL;

        free( buf );
        buf = fm_readLine( fm_p );      // 第二行才是对象对应的数据
        
        if ( !buf )
            return NULL;
       
        return buf;
    }
    // 下面是页面相关信息的处理i

    void printPages( PAGES * pages_p )
    {
        if ( pages_p == NULL ) {
            printf("\npages_p 是空指针！\n");
            return ;
        }

        printf( "pages_p->total = %d\n", pages_p->total );

        for ( int i = 0; i < pages_p->total; i ++ )
            printf("pages_p->leafs_p[%d]=%d\n", i, pages_p->leafs_p[i] ); 
    }
    //

    // getPagesObjList() 仅在  getPages() 中使用, 是为了简化代码长度， 便于阅读代码。
    // 处理 "Kids[3 0 R 12 0 R]" 字符串 , 调用前的代码已经确保该字符串的正确性, 所以不用再容错了
    // 注意也可能是"Kids [6 0 R ....]"
    int getObjList( char * item, int count, int *objlist )
    {
        int         i, j, k;        // i  用来标记当前下标, j 用来计算空格数,  k 用来标记对象编号之前位置的下标
        int         n=0;              // 用来标记pageslist下标， 不能超过count
        int         len;
        char        buf[16];            // 用来获取对象编号字符串

        len = strlen( item );
        
        j = 0;      // Kids[3 0 R 12 0 R], 用来在遍历字符串是计算对象数量, j 用来计算空格数, 3的倍数碰到一个对象
        n = 0;
        for ( i = 0; i < len && n < count ; i ++ ) {
            if ( item[i] == '[' ) {         //  根据(空格数+1)/3来计算总页数，从而申请空间, 不判断'R'是怕有些文件改字段会变化
                k = i;
                while( item[i] != ' ' && item[i] != ']' && i < len && n < count )    // '['后面的第一个数字是第一个页面对象编号, i < len 是容错, 防止意外(比如格式不正确)
                    i ++;
                if ( i>= len || n >= count ) {
                    printf("溢出了！！\n");
                    return -1;
                }

                memset( buf, 0, 16 );
                memcpy( buf, &item[k+1], i-(k+1) );
                objlist[n] = atoi(buf);

                n ++;
            }
            if ( item[i] == ' ' )  {         // 只要碰到空格, 先j ++ ,   "Kids[3 0 R 12 0 R]" 
                if ( n > 0 )                // 防止 "Kids [" 中这种情况中的空格不该计数
                    j ++;
                if ( j == 3 ) {          // 如果j=3，k 就记下i 的值, 因为下一个就是对象编号
                    k = i;
                    i ++;               // 跳过这个空格  
                    while( item[i] != ' ' && item[i] != ']'  && i < len && n < count )    // 具体处理同上
                        i ++;

                    if ( i>=len || n >= count) 
                        return -1;

                    j = 1;   // 用来计算下一个对象编号之前的空格数, 因为这个空格也要计数
                    memset( buf, 0, 16 );
                    memcpy( buf, &item[k+1], i-(k+1) );
                    objlist[n] = atoi(buf);

                    n ++;
                }
            }

        }

        return 0;
    }

    // 获取拆分后的第几个项
    char    *strsplit( char * src, char * split, int n )
    {
        char    * item;
        int     i=0;
        
        item = (char *)strtok( src, split );
        while ( item ) {
            i ++;
            if ( i == n )
                return item;

            item = (char *)strtok( NULL, split );
        }

        return NULL;

    }

    // getPageleaf() 是用来获取所有的叶子页面对象, 也就是带有Content的对象信息, 该对象也就是页面数据对象
    // 每个页面对象实际上也就是对应一个页面的数据, leafs 是个long 型数组
    // cursor 是用来标记leafs_p的位移的
    int getPageleaf( FILEMAP * fm_p, XREF * xref_p, int obj, int *leafs_p, int *cursor )
    {
        char        * buf;
        char        * item;
        int         kidscount;
        int         * objlist;
        int         ret;


        // # 5251||<</Count 100/Kids[5252 0 R 5253 0 R 5254 0 R 5255 0 R 5256 0 R 5257 0 R 5258 0 R 5259 0 R 5260 0 R 5261 0 R]/Parent 5250 0 R/Type/Pages>>
        buf = getObjContent( fm_p, xref_p, obj );
        
        if ( strstr( buf, "Kids" ) ) {      // 如果有"Kids", 说明不是叶子页面, 需要继续嵌套遍历kids页面, 直到找到最终的内容页面
            item = (char *) strtok( buf, "/" );
            while( item ) {
                if ( strstr( item, "Count" ) ) {    // kids 数量  //Count 100
                    kidscount = atoi( strsplit( item, " ", 2 ) );
                }
                if ( strstr( item, "Kids" ) ) {
                    objlist = (int *)malloc( sizeof(int) * kidscount + 1 );
                    memset( objlist, 0, sizeof(int)*kidscount + 1 );
                    ret = getObjList( item, kidscount, objlist );
                }
            }
            // 逐个处理页面对象, 找到叶子对象, 并填充到leafs_p
            for ( int i = 0; i < kidscount; i ++ ) {
                for ( int j=0; j < *cursor; j ++ ) {
                    if ( leafs_p[j] == objlist[i] ) {   // 容错.如果该对象已经存在于 leafs_p, 格式有问题，一个叶子页面对象不可能分属于多个页面
                        free( objlist );
                        free(buf);
                        return -1;
                    }
                }
                ret = getPageleaf( fm_p, xref_p, objlist[i], leafs_p, cursor );
                if ( ret < 0 ) {
                    free( objlist );
                    free(buf);
                    return ret;
                }
            }
        } else if( strstr( buf, "Contents" ) ){ //只有含有 Contents 的对象才是真正的叶子对象
            //printf("叶子对象:%s\n", buf);
            
            for ( int i=0; i < *cursor; i ++ ) {
                if ( leafs_p[i] == obj ) {   // 容错.如果该对象已经存在于 leafs_p, 格式有问题，一个叶子页面对象不可能分属于多个页面    
                    if ( objlist )
                        free( objlist );
                    free(buf);
                    return -1;
                }
            } 
            leafs_p[*cursor] = obj;
            //printf("找到一个叶子对象%d(第%d页)\n", obj, *cursor+1 );
            *cursor += 1;
        }

        free( buf );
        if ( objlist )
            free( objlist );
        return ret;
    }
    
    /*Root 对象的内容例子如下:  (注意， 下面的例子实际上是没有换行的， 这儿为了方便阅读分行了)
     *       1 0 obj

        <</AcroForm 51135 0 R/Lang(zh-CN)/MarkInfo<</Marked true>>
        /Metadata 51137 0 R/OCProperties<</D<</AS[<</Category[/View]/Event/View/OCGs[50601 0 R]>>
        <</Category[/Print]/Event/Print/OCGs[50601 0 R]>>
        <</Category[/Export]/Event/Export/OCGs[50601 0 R]>>]
        /ON[50601 0 R]/Order[]/RBGroups[]>>/OCGs[50601 0 R]>>
        /Outlines 607 0 R/Pages 2 0 R/StructTreeRoot 1066 0 R/Type/Catalog>>
     *          
     *          通过上面的Root信息, 可以获得pages 对象信息， 然后通过这个对象编号在获得
     *          所有的页面对应的原始对象的编号(该对象未必是内容对象，即不是叶子对象)
     *          (下面的例子中Kids中的内容省略了很多， 不然太长)
     *          
     *          Pages 对象含有Kids 项, 该项包含的就是页面对象。 该页面对象有可能还会含有Kids项.  
     *          只有含有content的对象才是叶子对象， 否则都应该含有Kids项。 
     *          
            2 0 obj
            <</Count 266/Kids[3 0 R。。。298 0 R 299 0 R]/Type/Pages>>
     *
     *          另外: 含有Content的叶子对象， 可能有多个stream 对象。 即一页对应多个内容对象。如下:
     *          5272||<</Contents[5278 0 R 5279 0 R 5280 0 R 5281 0 R 5282 0 R 5283 0 R 5291 0 R 5292 0 R]/CropBox[9 0 603 792]/MediaBox[0 0 612 792]/Parent 5252 0 R/Resources 5273 0 R/Rotate 0/Type/Page>>
     *
     *              返回的是Pages 对象编号
     *          参见:
     *              page信息.txt    --- 一个英文PDF文件的Root, Pages, Kids, Content 对象的信息罗列。
                注意: 
                    如果是全英文pdf 文件， 可能没有type0 对象, 也就是没有cmap 对象列表
     */
    int     getPages( FILEMAP * fm_p, XREF * xref_p,  PAGES *pages_p, int Root )
    {
        int         pagesobj;
        char    *   buf;
        char    *   item;
        int         ret = -1;

        buf = getItemOfObj( fm_p, xref_p, Root, "Pages" );
        if ( !buf )
            return -1;

        int i = 1;
        item = (char *)strtok( buf, " " );   // 解析 Pages 2 0 R
        while ( item != NULL ) {
            if ( i == 2 ) {                      // 页面对象对应的是第二项数字2
                pagesobj = atoi( item );
                break;
            }

            item = (char *) strtok( NULL, " " );
            i ++;
        }
        free( buf );
        // 如果i > 2 说明没有找到Pages对象编号, 也就是出错了
        if ( i > 2 ) 
            return -1;

        // 下面获取所有内容页面的对象信息 
        printf("pages obj = %d\n", pagesobj);

        // 获取Pages  对象中的页面对象的数据(主要是页面数量, 页面内容对象通过嵌套函数getPageleaf()获取 )
        buf = getObjContent( fm_p, xref_p, pagesobj );      
        printf( "Pages content =%s\n", buf );
        item = (char *)strtok( buf, "/" );
        while ( item != NULL ) {
            if ( strstr( item, "Count" ) ) {     // 找到了包含Count 的项目 Count 2
                pages_p->total = atoi( item + 6 );    // +6是为了跳过 "Count "
            }
            if ( strstr( item, "Kids" ) ) {     // Kids[3 0 R 12 0 R]  这就是所有的页面对象
                pages_p->leafs_p = (int *)malloc( sizeof(int)* pages_p->total + 1);
                memset( pages_p->leafs_p, 0, sizeof(int)* pages_p->total + 1 );
                
                ret = getObjList( item, pages_p->total, pages_p->leafs_p );   // 注意 这儿pages_p->leafs_p 还不是最终的页面叶子对象
            }
            item = (char *)strtok( NULL, "/" );
        }

        //printPages( pages_p );
        for ( int j = 0; j < pages_p->total; j ++ ) {
            ret = getPageleaf( fm_p, xref_p, pages_p->leafs_p[j], pages_p->leafs_p, &pages_p->cursor );
            if ( ret < 0 ) 
                return ret;
        }
        printPages( pages_p );

        return ret;
    }

    void freeXREF( XREF * xref_p ) 
    {
        if ( xref_p->objpos_p )
            free( xref_p->objpos_p );

        free( xref_p );
    }

    void freePAGES( PAGES * pages_p ) 
    {
        printf("--\n");
        if ( pages_p->leafs_p != NULL )
            free(  pages_p->leafs_p );
        printf("--\n");

        free( pages_p );
    }

    void  freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p )
    {
        if ( posmap_p )
            free( posmap_p );

        if ( trailer_p )
            free( trailer_p );

        freeXREF( xref_p );

        freePAGES( pages_p );

        if ( fm_p )
            freeFileMap( fm_p );
    }


    // 下面的函数为测试函数, 用来把相关信息输出到文件中, 便于查阅
    void printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename )
    {
        char        * buf;

        FILE        * fp;

        if ( !fm_p || !xref_p || !filename )
            return;

        if ( !( fp=fopen( filename, "wb") )) 
            return;
        
        
        for ( int i =0; i < xref_p->objTotal; i ++ ) {
            buf = getObjContent( fm_p, xref_p, i );

            if ( buf ) {
                printf( "\n%d||%s\n", i, buf);
                fprintf( fp, "\n%d||%s\n", i, buf);
                free( buf );
            }
        }
        printf( "--------------------------------we------------we---------\n");

        fclose( fp );
        return;
    }

    void printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename )
    {
        return;
    }


    // ----------- 下面是 CMAP  相关处理
    //
    // writeRawData()
    //     调试函数, 把stream中的字节流写入到文件中, 便于查看
    void writeRawData( char * filename, uchar *bytes, int len )
    {
        FILE        * fp;
        int         l;

        if ( !( fp=fopen( filename, "wb") )) 
            return;

        l = fwrite( bytes, len, 1, fp );
        printf("\n实际写入文件长度l=%d, 应该写入 len=%d\n", l, len);
        fclose( fp );
        
        return ;
    }

    // 根据解码后的数据流 获取cmap 的编码
    int getCMAP( char *stream, CMAP * cmap_p )
    {
        return 0;
    }
    //
    // procType0Stream()
    int procType0Stream( FILEMAP *fm_p, XREF *xref_p, int obj, CMAP *cmap_p )
    {
        int     ret;
        // 1. F052  是里面有对钩的方括号, 无法print(), 也无法在txt文件中显示. 
        char    * codeFilter[] = {"F052", "F0FE"};

        char    *buf;
        uchar   *bytes;
        int     len;
        
        printf( "\nprocType0_1()#####对象编号为：\n" + obj );
        
        //# 有2种情况.
        //    # 50569||<</Filter/FlateDecode/Length 7645>>stream
        //    # 50569||<</Filter/FlateDecode/Length 7645>>\r\nstream        
        //
        buf = getItemOfObj( fm_p, xref_p, obj, "Length" );
        if ( !buf ) {   // 没有找到Length项， 出错了
            printf( "CMAP:obj=%d Error 3, no ToUnicode Length", obj );
            return -1;
        } 
        len = atoi( strsplit( buf, " ", 2 ) );    // 获取 /Length 7645 中的数字 7645, 属于该字符串的第2项, 也就是字节流长度
        free( buf );

        // 下面的处理要小心, 如果上面的content 中包含stream, 就不要再跳过一行了, 如果不包括, 才需要跳过一行。。。！！！！！！
        buf = getObjContent( fm_p, xref_p, obj );
        if ( !buf ) {
            printf("%d 没有获取到数据!\n", obj );
            return -1;
        }

        if ( !strstr( buf, "stream" ) ) {  //用来区分是\r\nstream\r\n 还是stream\r\n
            free( buf );
            buf = fm_readLine( fm_p );      // 跳过 stream\r\n
            free(buf);
        }

        bytes = fm_read( fm_p, len, fm_p->pos );

        writeRawData( "\nF:/F_T_tmp/tmp1/test_raw.dat\n", bytes, len );
        
        free( bytes );
        

        // OK， 现在 buf 中的字节流就是steam 的内容， 下面进行解压, 解压后的内容存放在desbuf
        /*
        if ( "stream" not in content):    # 2016.12.20  用来区分是\r\nstream\r\n 还是stream\r\n
                print("没有stream, 需要再跳过一行--------------------------")
                buf = self.file_tools.readLine()                        # 跳过 stream\r\n
            buf, len1 = self.file_tools.read( len1 )  # 由于通过 getItemOfObj(), 内存文件 正好定位到stream 的起始位置
            if ( obj == "51134") :
                print( "pos=%d; l=%d; len=%d, stream=" % ( self.file_tools.getPosition(),l, len1 ) )
                print( buf )

            with open("f:/F_t_tmp/tmp1/%s_row.dat" % obj, "wb") as fs:
                fs.write(buf)
          */      
    }

    // 计算指定字符在字符串中出现的次数
    int  countChrInStr( char * buf, char ch ) 
    {
        int     i = 0 ;
        int     len = strlen(buf);
        int     n=0;

        printf("buf=%s!\n", buf);
        
        while( i < len )  {
            if ( buf[i] == ch )
                n ++;
            i ++;
        }

        printf("buf=%s,  \'%c\'出现的次数为%d\n", buf, ch, n);

        return i;
    }

    // 判断给定的对象是否是type0 字体, 即是否有CMAP
    bool isType0( FILEMAP *fm_p, XREF * xref_p, int obj ) 
    {
        char    *buf;

        buf = (char *)getObjContent( fm_p, xref_p, obj );

        if ( !buf )
            return false;

        if ( strstr( buf, "Type0" ) ) {     // 如果有Type0 项, 则表明是CMAP 对象
            free( buf );
            return true;
        } else {
            free( buf );
            return false;
        }
    }

    // <</Contents[5278 0 R 5279 0 R 5280 0]/CropBox[9 0 603 792]/MediaBox[0 0 612 792]/Parent 5252 0 R/Resources 5273 0 R/Rotate 0/Type/Page>>
    // 获取叶子对象 对应的内容对象列表  pages_p->c_objlist_p;
    //  pages_p->c_objlist_p[i] 对应的就是第i页的内容对象列表指针
    int getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf;
        int         * leafs_p = pages_p->leafs_p;       // 这个是为了简化代码长度
        int         count;
        
        pages_p->c_objlist_p = (int **)malloc(sizeof(int)*pages_p->total + 1 );
        memset( pages_p->c_objlist_p, 0, sizeof(int)*pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            printf("---------------------------%d-------------------\n", i+1);
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Contents" );
            printf( "叶子对象(第%d页):%d| 内容:%s", i+1, leafs_p[i], buf );

            if ( strchr(buf, '[' ) ) { // 如果有中括号, 表示包含多个内容对象,  Contents[5278 0 R 5279 0 R]
                count = countChrInStr( rtrim(buf), ' ' );  // 先计算字符串中出现的空格数
                count = (count+1)/3;                // 计算出对象数量, +1 是因为第一个对象只有2个空格
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset(  pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );
                ret = getObjList( buf, count, pages_p->c_objlist_p[i] );
            }
            else {
                count =1;                   // 没有中括号, 就1个内容对象。 Contents 7 0 R
                printf("就一个内容对象(buf=%s)\n", buf);
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset( pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = atoi( strsplit( buf, " ", 2 ) );       // 第二项就是内容对象
            }

            
            printf("\nobj %d 的内容对象为:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", pages_p->c_objlist_p[i][j]);
            printf("\n");
            
            free( buf );
        }
        return 0;
    }

    // 获取字体数量, 这个函数是为了简化代码长度, 提高代码可读性剥离出来的一个函数
    int getFontCount( FILEMAP *fm_p, XREF * xref_p, int obj )
    {
        int     count = 0;
        char    * buf, * item;
        bool    flag = false;
            
        buf = (char *)getObjContent( fm_p, xref_p, obj );    // /Font <</FT15 15 0 R /FT20 20 0 R /FT8 8 0 R>>
        printf( "叶子对象%d| font:%s\n",  obj, buf );
            
        item = (char *)strtok( buf, "/" );
        while( item && !flag ){     // flag = true 表示字体已经处理完毕， 退出循环
            if ( ! strstr( item, "Font" ) ) { // 没找到Font 就继续找下一个
                item = (char *)strtok( NULL, "/" );
                continue;
            }
            // 如果找到了"Font", 继续循环处理获取所有字体
            while ( item ) {
                item = (char *)strtok( NULL, "/" );
                count ++;
                if ( strstr( item, ">>") ) {        // 如果碰到">>"表示这是最后一个字体了
                    flag = true;
                    break;
                }
            }
                
        }
        free( buf );
        return count;
    }

    // <</Contents 57 0 R/Resources <</Font <</FT15 15 0 R /FT20 20 0 R /FT8 8 0 R>>>> /Type /Page>>
    // 获取叶子对象 对应的内容对象列表  pages_p->c_objlist_p;
    //  pages_p->fontmap_p 是2级指针, 第一级数组是多少个叶子页面, 第二级是页面的字体数组
    int getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf, * item;
        int         * leafs_p = pages_p->leafs_p;       // 这个是为了简化代码长度
        int         count;
        bool        flag = false;
        
        pages_p->fontmap_p = (FONTMAP **)malloc(sizeof( FONTMAP * ) * pages_p->total + 1 );
        memset( pages_p->fontmap_p, 0, sizeof( FONTMAP * ) * pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            printf("----------------FontMap-%d--(obj=%d)-----------------\n", i+1, leafs_p[i] );
            count = getFontCount( fm_p, xref_p, leafs_p[i] );

            pages_p->fontmap_p[i] = (FONTMAP *) malloc( sizeof( FONTMAP ) * count  + 1 );  // FONTMAP 数组
            memset( pages_p->fontmap_p[i], 0, sizeof( FONTMAP ) * count  + 1 );
            
            buf = (char *)getObjContent( fm_p, xref_p, leafs_p[i] );
            
            printf( "\n获取Font 数据:%s\n", buf);
            item = (char *)strtok( buf, "/" );
            
            while( item && !flag ){     // flag = true 表示字体已经处理完毕， 退出循环
                printf( "item = %s\n", item );
                if ( ! strstr( item, "Font" ) ) { // 没找到Font 就继续找下一个
                    item = (char *)strtok( NULL, "/" );
                    continue;
                }
                
                //     如果找到了"Font", 继续循环处理获取所有字体
                while ( item ) {
                    item = (char *)strtok( NULL, "/" );
                    printf("Font 信息: %s\n", item );
                    if ( strstr( item, ">>") ) {        // 如果碰到">>"表示这是最后一个字体了
                        flag = true;
                        break;
                    }
                }
            }
            free(buf);
        }
        return 0;
    }

    // 根据 Pages_p 来获取每页对应的cmap列表
    // <</Contents[5278 0 R 5279 0 R 5280 0 R 5281 0 R 5282 0 R 5283 0 R 5291 0 R 5292 0 R]/CropBox[9 0 603 792]/MediaBox[0 0 612 792]/Parent 5252 0 R/Resources 5273 0 R/Rotate 0/Type/Page>>
    int getCMAPLIST( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p, CMAPLIST * cmaplist_p )
    {
        int         ret;
        char        * buf, * item;
        int         * leafs_p = pages_p->leafs_p;
        int         * objlist;
        int         count;
        
        for ( int i=0; i < pages_p->total; i ++ ) {
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Contents" );
            printf( "叶子对象(第%d页):%d| 内容:%s", i+1, leafs_p[i], buf );

            count = countChrInStr( rtrim(buf), ' ' );  // 先计算字符串中出现的空格数
            if ( strchr(buf, '[' ) ) {    // 如果有中括号, 表示包含多个内容对象,  Contents[5278 0 R 5279 0 R]
                count = (count+1)/3;                // 计算出对象数量, +1 是因为第一个对象只有2个空格
                
                objlist = (int *)malloc( sizeof(int) * count + 1 );
                memset( objlist, 0, sizeof(int) * count + 1 );
                ret = getObjList( buf, count, * objlist );
            }
            else {
                count =1;                   // 没有中括号, 就1个内容对象。 Contents 7 0 R
                
                objlist = (int *)malloc( sizeof(int) + 1 );
                memset( objlist, 0, sizeof(int) * count + 1 );
                objlist[0] = atoi( strsplit( buf, " ", 2 ) );       // 第二项就是内容对象
            }

            printf("\nobj %d 的内容对象为:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", objlist[j]);
            printf("\n");

            free( buf );

            // 下面处理type0 的对象, 也就是cmap对象
            // 1. 先获取 叶子对象的信息中的Font内容
            // 2. 解析 Font 内容， 获得各个字体对象的对象编号
            // 3. 处理字体对象(依据获得的对象编号获得内容), 如果该对象内容有Type0, 表示是cmap, 处理记录
            // /Font<</C2_0 5 0 R/TT0 10 0 R/TT1 12 0 R/TT2 14 0 R/TT3 17 0 R>>
            // Font 部分是后面<<>>包含字体内容， 但是字体又都是/开始的
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Font" );
            printf( "叶子对象(第%d页):%d| Font内容:%s", i+1, leafs_p[i], buf );
            free(buf);

        }
    }

    int getCMAPS( FILEMAP *fm_p, XREF *xref_p )
    {   
        char    * buf;
    }
/*
 *     typedef struct __cmap__ {
        int         *code;
        int         *decode;
    } CMAP;

    #define L_FONTNAME      64
    typedef struct __cmap__ {
        int         obj;
        char        fontname[L_FONTNAME+1];     // 字体名称
    }CMAPS;

    typedef struct __cmaplist__ {
        int         pageNo;     // 页面编号
        int         count;      // CMAP 数量
        CMAP        * cmaps_p;  // 该页面使用的CMAPs, 可能是多个
    }CMAPLIST;

 */
