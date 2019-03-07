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
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "zlib.h"
    #include "pdf_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    /*
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
    #define L_ID  128             // 设定 Trailer中的ID 最大长度为128， 一般是不会超过这个长度
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
        int         obj;                        // 字体对应的对象编号
        char        fontname[L_FONTNAME+1];     // 字体名称
        int         isType0;                    // 1: 是type0, 0 : 不是type0, 目前没有用, 暂时保留
    }FONTMAP;

    // 下面是 CMAP 部分的数据结构定义
    typedef struct __code_ {        // <0005> <0022>   int 是4字节, 字符串也是4字节, 那个比较起来或代码维护起来方便就用那种方式
        int         code;       // 4位编码   0005
        int         decode;     // 4位解码   0022

    } CODE;
    typedef struct __cmap__ {
        char        fontname[32];   // 一般字体名字不会超过32个字符, 暂时按32字节设计
        int         obj;            // 对于Type0 而言,开始时字体对象编号, 后来存放的是Type0对象对应的Unicode对象编号
        int         total;          // 编码数量
        CODE        * code_p;       // 数组, 多个编码
        float       comprRatio;      // CMAP 压缩率, 这个是用来了解压缩率的, 对于功能实现没有用处
    } CMAP;

    typedef struct __pages__ {
        int         total;      // 总页数
        int         cursor;     // 最后一个叶子页面对象存储在leafs数组中的位置(下标), 这个是为了便于处理
        int     *   leafs_p;        // 数组, 用来存放叶子页面对象编号
        int     **  c_objlist_p;    // leafs_p中的叶子对象包含的内容对象列表 c_objlist_p[total][对象列表], 对象列表第一个是列表数量
        FONTMAP **  fontmap_p;      // fontname[页数量total][多少个字体][FONTMAP]
        int         cmaptotal;      // cmap 的总数量
        CMAP    *   cmaps_p;  // CMAP数组, 所有的cmap都放在数组里, 因为不同的也会复用很多相同的cmap
        float       **comprRatio;      // 每页数据内容压缩率, 这个是用来了解压缩率的, 对于功能实现没有用处, 数组, 对应c_objlist_p, 第二层数组的第一个元素是叶子对象数量
    }PAGES;

    typedef struct __pdf__ {
        POSMAP      * posmap_p;
        FILEMAP     * fm_p;
        XREF        * xref_p;
        TRAILER     * trailer_p;
        PAGES       * pages_p;
    } PDF;
    */

    
    void    initPDF( PDF *pdf_p );
    
    // 下面函数是内容解析部分
    void processTd( char * buf, CUR_XY * cur_xy_p, TM * tm_p );
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ); 
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p );
    void processTm( char *buf, CUR_XY * cur_xy_p, TM * tm_p );
    void processCm( char * buf, CM * cm_p );
    void processTstar( char * buf, CUR_XY * cur_xy_p );

    bool hasTxtPosition( char * buf );
    void processTxtPosition( char *buf, CUR_XY * cur_xy_p, TM * tm_p );

    char * processBT( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processBDC( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processRE( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processTf( char * tmpbuf, DECODE * decode_p );
    
    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p );
    char * processTJ( char *desbuf, char *srcbuf, CMAP * cmap_p );

    bool   hasText( char * buf );
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p );
    
    char * decode( PAGES * pages_p, char * buf, uLong dlen, char * filename ) ;

    CELL *  getCELLbyID( CELL * cellMap_p, int id );

    int     fillCellMap( DECODE * decode_p, CELL * cp );
    int     fillTextMap( char * buf, DECODE * decode_p );

    char *  procText4Table( TEXT * textMap_p, DECODE * decode_p );
    

    #define CHECK_ERR(err, msg) { \
        if (err != Z_OK) { \
            fprintf(stderr, "%s error: %d\n", msg, err); \
            exit(1); \
        } \
    }
    #define L_TMPBUF        256
        
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
    //	    这个接口仅是将pdf文件解析, 并没有提取数据. 提取数据接口为: 
    //	        1. getSpecPage( )
    //	        2. getSpecKeyLine( )
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
    PDF * DLL_EXPORT parsePDF1( char * srcfile )
    {
        int             ret;
        long            xrefpos;                // xref 起始位置
        PDF         *   pdf_p;

        pdf_p = (PDF *)calloc( sizeof(PDF), 1 );
        //pdf_p = (PDF *)malloc( sizeof(PDF) );
        //memset( pdf_p, 0, sizeof(PDF) );
        
        initPDF( pdf_p );

        pdf_p->fm_p = initFileMap( srcfile );
        print_fm( pdf_p->fm_p );

        xrefpos   = getStartXrefPos( pdf_p->fm_p );          // 获取startxref 的内容, 也就是第一个xref 的位置
        if ( xrefpos < 0 ) {
            printf("出错了!\n");
            freeALL2( pdf_p );
            return NULL; 
        }

        // 1. 获取XREF 和 Trailer
        ret = getXREF( pdf_p->fm_p, pdf_p->xref_p, xrefpos, pdf_p->posmap_p, pdf_p->trailer_p );

        if ( pdf_p->xref_p->retcode < 0 || pdf_p->trailer_p->retcode < 0 ) {
            printf("出错了\n");
            freeALL2( pdf_p );
            return NULL;
        }
        print_trailer( pdf_p->trailer_p ); 
        printXREFContent( pdf_p->fm_p, pdf_p->xref_p, "F:/F_T_tmp/tmp1/xrefAll_c.txt" );

        // 2. 下面获取页面 叶子对象信息 PAGES->leafs_p
        ret = getPages( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p, pdf_p->trailer_p->Root );

        // 下面是CMAP 相关的处理
        // 3. 获取每页对应的内容对象列表pages_p->c_objlist_p 和 字体列表 pages_p->fontmap_p
        getContentMap( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );
        getFontMap( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );

        // 3.1 处理字体列表 pages_p->fontmap_p, 获取 cmap 列表
        getCMAPS( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );
        
        return pdf_p;
    }

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
        // 3. 获取每页对应的内容对象列表pages_p->c_objlist_p 和 字体列表 pages_p->fontmap_p
        getContentMap( fm_p, xref_p, pages_p );
        getFontMap( fm_p, xref_p, pages_p );

        // 3.1 处理字体列表 pages_p->fontmap_p, 获取 cmap 列表
        getCMAPS( fm_p, xref_p, pages_p );
        

        freeAll( posmap_p, trailer_p, xref_p, pages_p, fm_p );

        return 0;
    }

    void initPDF( PDF *pdf_p )
    {
        pdf_p->posmap_p = (POSMAP *)calloc( sizeof(POSMAP), 1 );
        //pdf_p->posmap_p = (POSMAP *)malloc( sizeof(POSMAP) + 1 );
        //memset( pdf_p->posmap_p, 0, sizeof(POSMAP) + 1 );
        
        pdf_p->trailer_p = (TRAILER *) calloc( sizeof(TRAILER), 1  );
        //pdf_p->trailer_p = (TRAILER *) malloc( sizeof(TRAILER) +1  );
        //memset( pdf_p->trailer_p, 0, sizeof(TRAILER) + 1 );

        pdf_p->xref_p = (XREF *)calloc( sizeof(XREF), 1  );
        //pdf_p->xref_p = (XREF *)malloc( sizeof(XREF) + 1 );
        //memset( pdf_p->xref_p, 0, sizeof(XREF) + 1);
            
        pdf_p->pages_p = (PAGES *)calloc( sizeof(PAGES), 1 );
        //pdf_p->pages_p = (PAGES *)malloc( sizeof(PAGES) + 1 );
        //memset( pdf_p->pages_p, 0, sizeof(PAGES) + 1);
    }

    // 获取指定页的内容
    uchar * DLL_EXPORT getSpecPage( PDF *pdf_p, int pageno )
    {
        uchar       *srcbuf, * desbuf;
        char        * buf;
        int         obj;
        int         count;
        int         len;
        char        filename[128];
        int         err;
        uLong       dlen;
        char      * tmpbuf;
        

        if ( !pdf_p || pageno > pdf_p->pages_p->total || pageno < 1 )
            return NULL;
        
        count = pdf_p->pages_p->c_objlist_p[pageno-1][0];           // 第0个元素是叶子页面数量, 有可能是多个
        printf( "第%d页的叶子对象有%d个\n", pageno, count );

        for ( int i = 1; i <= count; i ++ ) {
            obj = pdf_p->pages_p->c_objlist_p[pageno-1][i];
            printf("第%d个叶子内容对象为: %d\n", i, obj );
            buf = (char *)getItemOfObj( pdf_p->fm_p, pdf_p->xref_p, obj, "Length" );    // <</Filter/FlateDecode/Length 1117>>stream
            if ( !buf )     
                return NULL;

            //cutTail( buf );                         // 去除 1117>> 后面的>>
            tmpbuf = (char *)strsplit1( buf, ' ', 2 );
            len = atoi( tmpbuf );     //
            free( tmpbuf );
            
            free( buf );
            buf = (char *)getObjContent( pdf_p->fm_p, pdf_p->xref_p, obj ); 
            if ( !strstr( buf, "stream" ) ) {   // 当前行没有stream的话, 
                free( buf );
                buf = fm_readLine( pdf_p->fm_p );            // 跳过 stream\r\n 这一行非数据
            }
            free( buf );
            srcbuf = (uchar *)fm_read( pdf_p->fm_p, len, pdf_p->fm_p->pos ); // 读取指定长度的stream字节流
        
            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_row_stream.dat", obj );
            writeRawData( filename, srcbuf, len );

            dlen = len * 10;                    // 注意, dlen 必须初始化足够存放解压后的数据, 否则解压就失败 ***** 很重要 *****
            desbuf = (uchar *)calloc( dlen, 1 );
            err = uncompress( desbuf, &dlen, srcbuf, len );
            // 写文件时调试代码， 方便查看， 最终可以删除或注释
            CHECK_ERR(err, "uncompress");    // 奇怪，明明解压成功了, 但是返回的确实data_error (-3), 只能屏蔽该检测了, 原因是dlen没有赋值或赋值太小
            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_stream.txt", obj );
            writeRawData( filename, desbuf, dlen );     // 如果 dlen 没有初始化足够大, 就会解压失败
            printf("=====================解压后的内容存放在文件:%s\n", filename );
            
            // 记录一下压缩率, 了解压缩成都
            pdf_p->pages_p->comprRatio_p[pageno-1][i] = (float)dlen/len;
            printf(" 压缩率为:%2.2f\n", pdf_p->pages_p->comprRatio_p[pageno-1][i] );

            char * retbuf;
            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_decode.dat", obj );
            retbuf = decode( pdf_p->pages_p, desbuf, dlen, filename );

            free( retbuf );
            
            free( srcbuf );
            free( desbuf );
        }

        return desbuf;
    }

    int DLL_EXPORT getPageSum( PDF * pdf_p ) 
    {
        return pdf_p->pages_p->total;
    }


