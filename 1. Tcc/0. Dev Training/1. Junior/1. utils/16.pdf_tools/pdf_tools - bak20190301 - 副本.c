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

    int     getXREF( FILEMAP * fm_p, XREF * xref_p,  long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p );
    
    void    initPDF( PDF *pdf_p );
    
    void    freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p );
    void    freeALL2( PDF * pdf_p );
    void    print_trailer( TRAILER * trailer_p );
    int     getPages( FILEMAP * fm_p, XREF * xref_p,  PAGES *pages_p, int Root );
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );
    void    cutTail( char * buf );

    char *  getItemOfObj( FILEMAP * fm_p, XREF *xref_p, int obj, char *item );
    char *  getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );
    void    writeRawData( char * filename, uchar *bytes, int len );
    
    char *  strsplit2( char * buf, int len, char ch, int n ) ;
    char *  strsplit1( char * buf, char ch, int n );

    int     getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getCMAPS( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    
    // 下面的函数为测试函数, 用来把相关信息输出到文件中, 便于查阅
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

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

    void    printTM( TM * tm_p );
    void    printCUR_XY( CUR_XY * cur_xy_p );

    CELL *  getCELLbyID( CELL * cellMap_p, int id );

    int     fillCellMap( DECODE * decode_p, CELL * cp );
    int     fillTextMap( char * buf, DECODE * decode_p );

    char *  procText4Table( TEXT * textMap_p, CELL * cellMap_p );
    
    void    printTextMap( TEXT * textMap_p );
    void    printCellMap( CELL * cellMap_p );


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

        pdf_p = (PDF *)malloc( sizeof(PDF) );
        memset( pdf_p, 0, sizeof(PDF) );
        
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
        pdf_p->posmap_p = (POSMAP *)malloc( sizeof(POSMAP) + 1 );
        memset( pdf_p->posmap_p, 0, sizeof(POSMAP) + 1 );
        
        pdf_p->trailer_p = (TRAILER *) malloc( sizeof(TRAILER) +1  );
        memset( pdf_p->trailer_p, 0, sizeof(TRAILER) + 1 );

        pdf_p->xref_p = (XREF *)malloc( sizeof(XREF) + 1 );
        memset( pdf_p->xref_p, 0, sizeof(XREF) + 1);
            
        pdf_p->pages_p = (PAGES *)malloc( sizeof(PAGES) + 1 );
        memset( pdf_p->pages_p, 0, sizeof(PAGES) + 1);
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
            buf = getItemOfObj( pdf_p->fm_p, pdf_p->xref_p, obj, "Length" );    // <</Filter/FlateDecode/Length 1117>>stream
            if ( !buf )     
                return NULL;

            //cutTail( buf );                         // 去除 1117>> 后面的>>
            tmpbuf = strsplit1( buf, ' ', 2 );
            len = atoi( tmpbuf );     //
            free( tmpbuf );
            
            free( buf );
            buf = getObjContent( pdf_p->fm_p, pdf_p->xref_p, obj ); 
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

            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_decode.dat", obj );
            buf = decode( pdf_p->pages_p, desbuf, dlen, filename );
            free( srcbuf );
            free( desbuf );
        }

        return desbuf;
    }

    int DLL_EXPORT getPageSum( PDF * pdf_p ) 
    {
        return pdf_p->pages_p->total;
    }

    //以下为内部调用的API
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
                free( buf );
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
        printf("Prev=%d(%08X)\n",     trailer_p->Prev, trailer_p->Prev );
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
        tmpbuf = strsplit1( buf, ' ', 2 );
        *objSum = atoi( tmpbuf );     // 起始对象编号
        free( tmpbuf);

        tmpbuf = strsplit1( buf, ' ', 1 );
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
    
        if ( objSum == 1 ) {
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
        if ( ret < 0 )                     // 返回至不等于0 表示出错了,直接返回xref_p,  xref_p->retcode =-1(初始化de值), 也就是表示出错了
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

    // 获取拆分后的第几个项, 放弃使用strtok， 因为外部有可能会使用
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
                item = (char *)strtok( NULL, "/" );
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
            if ( ret < 0 ) { 
                free( buf );
                return ret;
            }
        }
        free( buf );
        //printPages( pages_p );

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
        if ( pages_p->leafs_p != NULL )
            free(  pages_p->leafs_p );

        free( pages_p );
    }

    void  DLL_EXPORT freeALL2( PDF * pdf_p )
    {
        printf("===================================================freeALL1()===================================================\n");
        if ( pdf_p ) {
            if ( pdf_p->posmap_p )
                free( pdf_p->posmap_p );

            if ( pdf_p->trailer_p )
                free( pdf_p->trailer_p );

            if ( pdf_p->xref_p )
                freeXREF( pdf_p->xref_p );

            if ( pdf_p->pages_p )
                freePAGES( pdf_p->pages_p );

            if ( pdf_p->fm_p )
                freeFileMap( pdf_p->fm_p );

            free( pdf_p );
        }
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
                fprintf( fp, "\n%d(%08X)||%s\n", i, xref_p->objpos_p[i], buf);
                free( buf );
            }
        }

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
        FILE        *   fp;
        size_t          l;

        if ( !( fp=fopen( filename, "wb") )) 
            return;

        l = fwrite( bytes, len, 1, fp );
        printf("\n写成功次数:l=%d, 应该写入 len=%d\n", l, len);
        fclose( fp );
        
        return ;
    }

    // 计算指定字符在字符串中出现的次数
    int  countChrInStr( char * buf, char ch ) 
    {
        int     i = 0 ;
        int     len = strlen(buf);
        int     n=0;

        while( i < len )  {
            if ( buf[i] == ch )
                n ++;
            i ++;
        }

        return i;
    }

    // 判断给定的对象是否是type0 字体, 即是否有CMAP
    int isType0( FILEMAP *fm_p, XREF * xref_p, int obj ) 
    {
        char    *buf;

        buf = (char *)getObjContent( fm_p, xref_p, obj );

        if ( !buf )
            return 0;

        if ( strstr( buf, "Type0" ) ) {     // 如果有Type0 项, 则表明是CMAP 对象
            free( buf );
            return 1;
        } else {
            free( buf );
            return 1;
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
        
        pages_p->c_objlist_p = (int **)malloc(sizeof(int *)*pages_p->total + 1 );
        memset( pages_p->c_objlist_p, 0, sizeof(int *)*pages_p->total + 1  );

        pages_p->comprRatio_p = (float **)malloc(sizeof(float *)*pages_p->total + 1 );
        memset( pages_p->comprRatio_p, 0, sizeof(float *)*pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Contents" );
            //printf( "叶子对象(第%d页):%d| 内容:%s\n", i+1, leafs_p[i], buf );

            if ( strchr(buf, '[' ) ) { // 如果有中括号, 表示包含多个内容对象,  Contents[5278 0 R 5279 0 R]
                count = countChrInStr( rtrim(buf), ' ' );  // 先计算字符串中出现的空格数
                count = (count+1)/3;                // 计算出对象数量, +1 是因为第一个对象只有2个空格

                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset(  pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = count;            // 第一个元素(下标为0) 放叶子内容对象数量
                ret = getObjList( buf, count, &(pages_p->c_objlist_p[i][1]) );
                
                pages_p->comprRatio_p[i] = (float *)malloc( sizeof(float ) * (count + 1) );  // 这三行是压缩率数据部分
                memset(  pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

                pages_p->comprRatio_p[i][0] = count;            // 第一个元素(下标为0) 放叶子内容对象数量
            }
            else {
                count =1;                   // 没有中括号, 就1个内容对象。 Contents 7 0 R
                //printf("就一个内容对象(buf=%s)\n", buf);
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset( pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = 1;                                     // 数字第1项(下标为0)就是叶子内容对象数量
                pages_p->c_objlist_p[i][1] = atoi( strsplit( buf, " ", 2 ) );       // buf 第二项就是内容对象

                pages_p->comprRatio_p[i] = (float *)malloc( sizeof(float ) * (count + 1) );
                memset( pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

                pages_p->comprRatio_p[i][0] = 1;                                     // 数字第1项(下标为0)就是叶子内容对象数量
            }

            /* 
            printf("\nobj %d 的内容对象为:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", pages_p->c_objlist_p[i][j]);
            printf("\n");
            */
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

    char * strsplit1( char * buf, char ch, int n ) 
    {
        int     i = 0 ;
        int     len = strlen(buf);
        int     m=0;
        char    * retbuf;
        int     k =0;       // 记录每一项的开始位置

        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // 找到第几个项的位置了
                    retbuf = (char *)malloc( i-k+1);
                    memset( retbuf, 0, i-k+1);   
                    memcpy( retbuf, buf+k, i-k );
                    return retbuf;
                }
                while( buf[i] == ch )       // 跳过连续的分隔符
                    i ++;
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // 最后一项
            retbuf = (char *)malloc( i-k+1);
            memset( retbuf, 0, i-k+1);   
            memcpy( retbuf, buf+k, i-k );
            return retbuf;
        }
        return NULL;
    }

    char * strsplit3( char * des, int len, char * buf, char ch, int n ) 
    {
        int     i = 0 ;
        int     len = strlen(buf);
        int     m=0;
        int     k =0;       // 记录每一项的开始位置

        if ( !des )
            return NULL;
        
        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // 找到第几个项的位置了
                    memset( des, 0, len);   
                    memcpy( des, buf+k, i-k );
                    printf( "des=%s\n", des );
                    return des;
                }
                while( buf[i] == ch )       // 跳过连续的分隔符
                    i ++;
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // 最后一项
            memset( des, 0, len );   
            memcpy( des, buf+k, i-k );
            return des;
        }
        return NULL;
    }

    // getType0Font()  中间函数, 简化函数长度, 增加可读性
    // type0list  内容格式为: " FT8 8 FT15 15 FT20 20 FT62 62 FT87 87"
    int getType0Font( PAGES * pages_p, int type0count, char * type0list )
    {  
        int     n = 0;  // 空格数量, 1表示第一个空格, type0list 中包含type0 字体名和对应的对象编号, 用空格分隔。 " FT8 8 FT15 15 FT20 20 FT62 62 FT87 87"
        int     k = 0;  // 标记空格的位置, 用来截取type0字体名称和对象编号
        // 遍历一遍type0list  获取type0字体名称与对象编号
        int     len = strlen( type0list );
        char    ttt[8];     // 临时存放字体对象编号, 

        printf( "type0 count=%d, type0 list:{%s}\n", type0count, type0list );
        pages_p->cmaptotal = type0count;
        pages_p->cmaps_p = (CMAP *)malloc( sizeof(CMAP) * type0count + 1);
        memset( pages_p->cmaps_p, 0, sizeof(CMAP)*type0count + 1 );

        for ( int i = 0; i < len && n/2 <= type0count; i ++ ) {
            if ( type0list[i] == ' ' ) {     // 找到空格
                k = i+1;                  // 记录空格后的位置, 找到下一个空格，则2个空格之间就是要找的项目
                n ++;
                i ++;       // 跳过空格
                while( type0list[i] != ' ' && i < len )    // 找下一个空格 
                    i ++;
                if ( n%2 == 1 ) {     // 如果 n % 2 = 1, 表示是字体名称, 则 字体下标为: n/2+n%2-1, 例: 1/2+1%2-1 = 0(简化为 n/2), 因为下标是从0开始的
                    memcpy( pages_p->cmaps_p[ n/2 ].fontname, &(type0list[k]), i-k ); 
                    printf("%d: n=%d, 字体名称:%s\n", n/2, n, pages_p->cmaps_p[n/2].fontname );
                } else if ( n%2 == 0 ) {    // 如果 n%2 = 0, 表示是字体对象编号, 则对应的下标是 n/2+n%2-1, 例如: 2/2+2%2-1=0(简化为 n/2-1)
                    memset( ttt, 0, 8 );
                    memcpy( ttt, type0list+k , i-k );
                    pages_p->cmaps_p[ n/2-1 ].obj = atoi( ttt );
                    
                    printf("%d: n=%d 字体对象编号:%d\n", n/2-1, n, pages_p->cmaps_p[n/2-1].obj );
                } 
                    
                i --;       // i 回退到空格位置, 这样循环才能正常进行
            }
        }

        return 0;

    }


    // 获取叶子对象 对应的字体对象列表  pages_p->fontmap_p;
    //  pages_p->fontmap_p 是2级指针, 第一级数组是多少个叶子页面, 第二级是页面的字体数组
    int getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf, * item;
        int         * leafs_p = pages_p->leafs_p;       // 这个是为了简化代码长度
        int         count;
        bool        flag;
        char        * tmpbuf;
        int         type0count = 0;                     // 用来计数type0 字体个数(是除去重复的之后的数量), 是为了后面为type0字体申请存储空间的
        char        type0list[2048];                    // 用来存放type0 字体名称 "F34 F20 T2"等, 去重后的存储在这儿
        
        pages_p->fontmap_p = (FONTMAP **)malloc(sizeof( FONTMAP * ) * pages_p->total + 1 );
        memset( pages_p->fontmap_p, 0, sizeof( FONTMAP * ) * pages_p->total + 1  );

        memset( type0list, 0, 2048 );       // 初始化

        for ( int i=0; i < pages_p->total; i ++ ) {
            count = getFontCount( fm_p, xref_p, leafs_p[i] );       // 获取字体数量

            pages_p->fontmap_p[i] = (FONTMAP *) malloc( sizeof( FONTMAP ) * count  + 1 );  // FONTMAP 数组
            memset( pages_p->fontmap_p[i], 0, sizeof( FONTMAP ) * count  + 1 );
            
            buf = (char *)getObjContent( fm_p, xref_p, leafs_p[i] );
            
            //printf( "\n获取Font 数据:%s\n", buf);
            item = (char *)strtok( buf, "/" );
            flag = false;
            while( item && !flag ){     // flag = true 表示字体已经处理完毕， 退出循环
                //printf( "item = %s\n", item );
                if ( ! strstr( item, "Font" ) ) { // 没找到Font 就继续找下一个
                    item = (char *)strtok( NULL, "/" );
                    continue;
                }
                
                //     如果找到了"Font", 继续循环处理获取所有字体
                int     n = 0;
                while ( item ) {
                    item = (char *)strtok( NULL, "/" );
                    //printf("Font 信息: %s\n", item );           // Font 信息: FT15 15 0 R
                    
                    tmpbuf = strsplit1( item, ' ', 1 );
                    memcpy( pages_p->fontmap_p[i][n].fontname, tmpbuf, strlen(tmpbuf) );
                    free( tmpbuf );
                    
                    tmpbuf = strsplit1( item, ' ', 2 );
                    pages_p->fontmap_p[i][n].obj = atoi(tmpbuf);
                    free( tmpbuf );
                    
                    // 判断是否是type0
                    if ( isType0( fm_p, xref_p, pages_p->fontmap_p[i][n].obj ) )  {
                        pages_p->fontmap_p[i][n].isType0 = 1;
                        if( !strstr( type0list, pages_p->fontmap_p[i][n].fontname ) ) {  // 如果该type0名称没有在type0list中, 说明是新的type0字体, 添加进type0list, type0count ++
                            sprintf( type0list, "%s %s %d", type0list, pages_p->fontmap_p[i][n].fontname, pages_p->fontmap_p[i][n].obj );
                            type0count ++;
                        }
                    } else
                        pages_p->fontmap_p[i][n].isType0 = 0;

                    n ++;
                    if ( strstr( item, ">>") ) {        // 如果碰到">>"表示这是最后一个字体了
                        flag = true;
                        break;
                    }
                }
            }
            free(buf);
        }
    
        getType0Font( pages_p, type0count, type0list );
        return 0;
    }

    // 局部函数, 仅在 getCMAP() 中使用, 处理字符串， 去除尾部的非数字部分, 一般是>>
    void cutTail( char * buf )
    {
        int     len;
        int     i =1;

        len = strlen( buf );
        while ( !( buf[len-i]>='0' && buf[len-i] <='9' ) ) {
            buf[len-i] = 0;
            i ++;
        }
    }
    
    // 获取CMAP中的编码映射总数量, 本地函数, 仅在procType0Stream() 中调用, 简化代码数量, 增加可读性
    int getCMAPsum( MEMORYMAP * mm_p )
    {
        char    * buf;
        int     mapsum;
        int     total = 0;
        char  * tmpbuf;
        
        while ( 1 ){
            buf = mm_readLine( mm_p );
            if ( !buf )
                break;
            if ( strstr( buf, "endcmap" ) ) {// 增加退出循环处理, 防止有些缓冲区可能有遗留的尾巴
                free( buf );
                break;
            }

            /*
                # 2 beginbfrange
                #    <00B2> <00B2> [<2014>]
                #    <00B3> <00B4> <201C>
                #    endbfrange
                #    这种格式的需要额外处理， 及解码有中括号
                # 还有个begincodespacerange  .... 这个可能是限定编码范围的
            */

            if ( strstr( buf, "beginbfchar" ) ) {   // 单个编码映射  100 beginbfchar
                tmpbuf =  strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                
                total += mapsum;

                // <1440> <5E10> , 单行是6+1+6+1 = 14个字节  0A 结尾的
                mm_seek( mm_p, mm_p->pos + 14 * mapsum );       // 跳过 mapsum 个字符, 也就是编码映射部分
                //free( buf );    // 前面free()过了
                buf = mm_readLine( mm_p );                      // 跳过  endbfchar
                free( buf );
                continue;
            }
            
            if ( strstr( buf, "beginbfrange" ) ) {   // 范围编码映射  2 beginbfrange
                tmpbuf =  strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // 跳过 mapsum 行即可
                    buf = mm_readLine( mm_p );
                    free( buf );
                    buf = NULL;
                }
                continue;
            }
            
            //if ( buf )
            free( buf );     // 如果执行到这儿, 肯定需要free()
        }

        printf("-----------------------------------------cmap total=%d\n", total);
        return  total;
    }

    void printCMAP( CMAP * cmap_p )
    {
        if ( !cmap_p )
            return;

        printf("\n----------------------------------------------------------------------------------------------\n");
        printf( "cmap_p->fontname=%s\n", cmap_p->fontname );
        printf( "cmap_p->obj=%d\n", cmap_p->obj );
        printf( "cmap_p->total=%d\n", cmap_p->total );
        printf( "cmap_p->comprRatio=%.2f\n", cmap_p->comprRatio );
        for ( int i = 0; i < cmap_p->total; i ++ ) {
            printf( "%d: <%04X>   <%04X>\n", i+1,  cmap_p->code_p[i].code, cmap_p->code_p[i].decode );
        }
        printf("\n----------------------------------------------------------------------------------------------\n");
    }


    // 临时函数, 仅在getCMAP() 调用, 处理 type0 字节流, 获取cmap
    // desbuf 中存放的是已经解压后的字节流,  
    // dlen   是 desbuf 中的字节流的长度
    // 编码映射似乎可以根据key值用函数算出 value, 但是需要多个函数, 暂时简单处理, 所有编码单独匹配, 未来动态创建映射函数来提高处理性能
    // 注意: 碰到 F052, F0FE , 目前转换成空格(0020), 带对号的方框, 未来可以单独用对号替代
    // 不适用链表方式实现, 维护麻烦, 遍历两次字节流, 第一次是为了计算编码映射数量, 用来申请内存.
    int procType0Stream( CMAP *cmap_p, uchar * desbuf, uLong dlen )
    {
        MEMORYMAP   * mm_p;
        char        * buf;
        int         mapsum;     // 映射数量, 临时记录每个映射单元的编码映射数量
        int         total = 0;  // 所有编码映射数量之和
        int         n;          // cmap_p->code_p[n] 下标
        int         start1, end1, start2;       // beginbfrange 中的起始结束编码
        char        * tmpbuf;    // 用来临时存放编码 547F之类的 
 
        mm_p = initMemoryMap( desbuf, dlen );

        total = getCMAPsum( mm_p );     // 获取CMAP中的编码映射总数量,为cmap_p->code_p 申请空间做准备
        
        cmap_p->code_p = (CODE *)malloc( sizeof(CODE) * total + 1 );
        memset( cmap_p->code_p, 0, sizeof(CODE)*total + 1 );
        
        cmap_p->total = total; 
        // 重新定位到内存映射的头部
        mm_seek( mm_p, 0 );
        while ( 1 ){
            buf = mm_readLine( mm_p );

            if ( !buf )
                break;
            if ( strstr( buf, "endcmap" ) ) {// 增加推出循环处理, 防止有些缓冲区可能有遗留的尾巴
                free( buf );
                break;
            }
            /*
                # 2 beginbfrange
                #    <00B2> <00B2> [<2014>]
                #    <00B3> <00B4> <201C>
                #    endbfrange
                #    这种格式的需要额外处理， 及解码有中括号
                # 还有个begincodespacerange  .... 这个可能是限定编码范围的
            */
            if ( strstr( buf, "beginbfchar" ) ) {           // 单个编码映射  100 beginbfchar
                tmpbuf = strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                for( int i = 0; i < mapsum; i ++ ) {
                    free( buf );
                    buf = mm_readLine( mm_p );              // <1440> <5E10> 
                    DelCharsInString( buf, "<>" );          // 删除 尖括号  "1440 5E10" 
                    
                    tmpbuf = strsplit1( buf, ' ', 1 );
                    cmap_p->code_p[n].code = Hex2Int2( tmpbuf );          // 0x1440=
                    free(tmpbuf);

                    tmpbuf = strsplit1( buf, ' ', 2 );
                    cmap_p->code_p[n].decode = Hex2Int2( tmpbuf );        // 0x5E10=
                    free( tmpbuf );

                    n ++;
                }
                free( buf );
                continue;
            }else if ( strstr( buf, "beginbfrange" ) ) {   // 范围编码映射  2 beginbfrange
                tmpbuf = strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                //printf("beginbfrange sum=%d\n", mapsum);
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // 跳过 mapsum 行即可
                    free( buf );
                    buf = mm_readLine( mm_p );              // <01C3> <01C4> <3001>
                    DelCharsInString( buf, "<>" );          // 删除 尖括号  "01C3 01C4 3001"

                    tmpbuf = strsplit1( buf, ' ', 1 );
                    start1 = Hex2Int2( tmpbuf );          //  0x01C3 =
                    free(tmpbuf);

                    tmpbuf = strsplit1( buf, ' ', 2 );
                    end1 = Hex2Int2( tmpbuf );          //  0x01C4 =
                    free(tmpbuf);

                    tmpbuf = strsplit1( buf, ' ', 3 );
                    start2 = Hex2Int2( tmpbuf );          //  0x3001 =
                    free(tmpbuf);

                    for ( int i = 0; i <= end1-start1; i ++ ) {
                        cmap_p->code_p[n].code = start1 + i;
                        cmap_p->code_p[n].decode = start2 + i;
                        n ++;
                    }
                }
                free( buf );
                continue;
            }
            free( buf );
        }

        //printCMAP( cmap_p );
        freeMemoryMap( mm_p);

        return 0;
    }

    /*
    typedef struct __code_ {        // <0005> <0022>   int 是4字节, 字符串也是4字节, 那个比较起来或代码维护起来方便就用那种方式
        int         code;       // 4位编码   0005
        int         decode;     // 4位解码   0022
    } CODE;
    typedef struct __cmap__ {
        char        fontname[32];   // 一般字体名字不会超过32个字符, 暂时按32字节设计
        int         obj;
        int         total;          // 编码数量
        CODE        * code_p;       // 数组, 多个编码
    } CMAP;

    typedef struct __pages__ {
        int         total;      // 总页数
        int         cursor;     // 最后一个叶子页面对象存储在leafs数组中的位置(下标), 这个是为了便于处理
        int     *   leafs_p;        // 数组, 用来存放叶子页面对象编号
        int     **  c_objlist_p;    // leafs_p中的叶子对象包含的内容对象列表 c_objlist_p[total][对象列表]
        FONTMAP **  fontmap_p;      // fontname[页数量total][多少个字体][FONTMAP]
        int         cmaptotal;      // cmap 的总数量
        CMAP    *   cmaps_p;  // CMAP数组, 所有的cmap都放在数组里, 因为不同的也会复用很多相同的cmap
    }PAGES;
     * */

    // 处理Type0 对象, 获取 CMAP 编码
    // <</Filter /FlateDecode /Length 279>>，
    int getCMAP( FILEMAP *fm_p,  XREF *xref_p, CMAP *cmap_p )
    {
        char    *   buf, * tmpbuf;
        int         len;
        uchar   *   ubuf,  * desbuf ;
        int         ret;
        
        buf = (char*)getItemOfObj( fm_p, xref_p, cmap_p->obj, "Length" );    //  Length 279>>
        if ( !buf )
            return -1;

        cutTail( buf );                         // 去除 279>> 后面的>>
        tmpbuf = ( char *) strsplit1( buf, ' ', 2 );
        len = atoi( tmpbuf );         // 取279 数字， 也就是字节流长度
        free( tmpbuf );
        
        printf( "字节流长度为%d\n", len );
        // 下面的处理要小心, 如果上面的对象的Content中包含stream, 就不要再跳过一行了, 如果不包括, 才需要跳过一行。。。！！！！！！
        // 
        free(buf);
        buf = (char*)getObjContent( fm_p, xref_p, cmap_p->obj );

        if ( !strstr( buf, "stream" ) ) {            // 用来区分是\r\nstream\r\n 还是stream\r\n
            free( buf );
            buf = (char *)fm_readLine( fm_p );      // 跳过一行
            free( buf );
        }

        ubuf = (uchar *)fm_read( fm_p, len, fm_p->pos );       // 读取指定长度的字节流

        // 现在ubuf 中的字节流就是stream的内容， 下面进行解压, 解压后的内容存放在desbuf, 放在文件中的部分为调试代码, 最后注释或删除了
        char        filename[128];
        sprintf( filename, "f:/F_t_tmp/cmap/c_%d_stream.txt", cmap_p->obj );
        writeRawData( filename, ubuf, len );

        // 解压
        int     err;
        uLong   dlen = len * 6;

        //desbuf = (uchar *)malloc( len * 6 );
        //memset( desbuf, 0, len * 6 );
        desbuf = (uchar *)calloc( dlen, 1 );
        err = uncompress( desbuf, &dlen, ubuf, len );
        CHECK_ERR(err, "uncompress");  

        cmap_p->comprRatio = (float)dlen/len;

        printf("cmap_p->obj=%d, 压缩率为=%2.2f", cmap_p->obj, cmap_p->comprRatio );

        // 写文件时调试代码， 方便查看， 最终可以删除或注释
        sprintf( filename, "f:/F_t_tmp/cmap/c_%d_cmap.txt", cmap_p->obj );
        writeRawData( filename, desbuf, dlen );

        printf("dlen=%d, len=%d\n", dlen, len );

        ret = procType0Stream( cmap_p, desbuf, dlen );

        free( ubuf );
        free( desbuf );

        return ret;
    }


    // 处理pages_p->cmaps_p 中的type0字体对象, 获取CMAP 并存放着在cmap_p
    //  type0 对象里面有个unicode对象, 里面只有stream流, 内容才是真正的CMAP信息
    int getCMAPS( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        char    *   buf, *tmpbuf;
        int         obj;
        
        for( int i=0; i < pages_p->cmaptotal; i ++ ) {
            buf = (char*)getItemOfObj( fm_p, xref_p, pages_p->cmaps_p[i].obj, "ToUnicode" );    //  如果没有 ToUnicode, 那么就是出错了  ToUnicode 88 0 R
            if ( !buf )
                return -1;

            tmpbuf = (char *)strsplit1( buf, ' ', 2 ) ;         // ToUnicode 88 0 R 第二项就是Unicode 对象编号
            obj = atoi( tmpbuf );
            free( tmpbuf );                                     // strsplit1() 内部申请了内存,所以需要释放
            free( buf );                                        // getItemOfObj() 内部申请了内存,所以需要释放

            pages_p->cmaps_p[i].obj = obj;                      // type0 字体对象编号 更新为  Unicode 对象编号
            getCMAP( fm_p, xref_p, &(pages_p->cmaps_p[i]) );
        }
            
        return 0;
    }



    //###########################################################################
    //#  以下部分为解析 解压后 页面内容部分
    /*
     * 15. decode()
     *      对给定的字节流进行解码
     * 思路:
     *      1. 将每一部分的处理都模块化
     *      2. 最高一级(I级)是 processBDC, processBT， 所有的处理都在这BDC/EMC, BT/ET 之间， 
     *         这2中之间也会互相嵌套
     *      3. II级 处理是: 
     *          processTD, processTd, processTm, processT*        文本位置
     *          processTJ, processTj, processQuote, processDblQuote         文本显示, 后两个是单引号, 双引号的处理
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  文本状态
     *          表格处理？？？
     *      2016.12.08:
     *          采用虚拟纸张的技术来进行内容编排, 即 处理后的数据存放在一个虚拟纸张的内存中，然后输出这个数据即可.
     *          virtual_Page={["Y坐标值": 文字内容],}
     *          然后输出的时候只要对virtual_Page 按照关键字排序后进行输出即可.
     *          坐标处理: 当前坐标如果与已有坐标中的某个坐标相差小于1, 则可视为同一行 。 因为最小的字体也会大于1个像素
     *          {Y:["文字内容","长度", 单位长度]},
     *               单位长度是文字的字体的长度, 用来判断下一文字是否需要与该文字之间有分隔符
     *               如果下一段文字的起始x坐标与该文字的x+len 小于单位长度, 则不需要分隔符.
     *      2016.12.21:
     *          之前的处理, 还是有很多不完善的地方, 主要是没有按照pdf 的环境的设计思路. 下面完善设计思路, 增加 图形状态stack 实现.
     *            用GraphStateStack 记录当前压栈的所有 图形状态, cur_graph_state 用来记录当前的图形状态. 考虑到只处理文本,
     *            图形状态只记录 tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */

    /*
     * # 坐标信息, 表格信息(初始化页眉是没有的, -1用于页眉编号), 文本信息, 转换坐标
    // 坐标信息
    typedef struct __cur_xy__ {
        int     x;
        int     y;
    }CUR_XY;

    typedef struct __cellMap__ {
        // 单元格信息
    }CELLMAP;

    typedef struct __cellIndexMap__{
        // 单元格索引信息
    }CELLINDEXMAP;

    typedef struct __textMap__ {
        // 文本映射表
    } TEXTMAP;

    typedef struct __tm__ {
        // tm 信息
    }TM:

    typedef struct __tf__ {
        // tf 信息
    }TF;
    */

    /*
     * 15. decode()
     *      对给定的字节流进行解码
     * 思路:
     *      1. 将每一部分的处理都模块化
     *      2. 最高一级(I级)是 processBDC, processBT， 所有的处理都在这BDC/EMC, BT/ET 之间， 
     *         这2中之间也会互相嵌套
     *      3. II级 处理是: 
     *          processTD, processTd, processTm, processT*        文本位置
     *          processTJ, processTj, processQuote, processDblQuote         文本显示, 后两个是单引号, 双引号的处理
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  文本状态
     *          表格处理？？？
     *      2016.12.08:
     *          采用虚拟纸张的技术来进行内容编排, 即 处理后的数据存放在一个虚拟纸张的内存中，然后输出这个数据即可.
     *          virtual_Page={["Y坐标值": 文字内容],}
     *          然后输出的时候只要对virtual_Page 按照关键字排序后进行输出即可.
     *          坐标处理: 当前坐标如果与已有坐标中的某个坐标相差小于1, 则可视为同一行 。 因为最小的字体也会大于1个像素
     *          {Y:["文字内容","长度", 单位长度]},
     *               单位长度是文字的字体的长度, 用来判断下一文字是否需要与该文字之间有分隔符
     *               如果下一段文字的起始x坐标与该文字的x+len 小于单位长度, 则不需要分隔符.
     *      2016.12.21:
     *          之前的处理, 还是有很多不完善的地方, 主要是没有按照pdf 的环境的设计思路. 下面完善设计思路, 增加 图形状态stack 实现.
     *            用GraphStateStack 记录当前压栈的所有 图形状态, cur_graph_state 用来记录当前的图形状态. 考虑到只处理文本,
     *            图形状态只记录 tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */
    char * decode( PAGES * pages_p, char * buf, uLong dlen, char * filename ) 
    {
        char        *   retbuf;
        char        *   tmpbuf;       
        MEMORYMAP   *   mm_p;
        DECODE      *   decode_p;

        retbuf = (char *)calloc( dlen, 1 ); // 确保申请的内存足够用
        
        decode_p = (DECODE *)calloc( sizeof( DECODE ), 1 );
        
        mm_p = initMemoryMap( buf, dlen );

        while( 1 ) {    // 注意: 下面的判断不是if else, 因为有可能一行包括多种操作符
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf( "decode() : tmpbuf = %s\n", tmpbuf );
            
            if ( strstr( tmpbuf, "BT" ) ) {         // 如果是 包含"BT"/"ET"
                processBT( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            if ( strstr( tmpbuf, "BDC" ) ) { // 如果包含"BDC"/"EMC"
                processBDC( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            
            // re 的处理应该在BT/ET, BDC/EDC 之外, 也就是单元格.
            if ( strstr( tmpbuf, "re" )  ) {   //包含"re"
                processRE( mm_p,  &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            }
            
            if ( strstr( tmpbuf, "Tf") ) {
                processTf( tmpbuf, decode_p );
            }
            
            if ( strstr(tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) {
                memset( decode_p->tm, 0, sizeof(TM) );
            }
            
            free( tmpbuf );
        }
        // 下面来处理上面解析出来的内容, 包括表格的重新构建
        //
        //
        printf("----------------Hello decode()--------------------------\n");
        printCellMap( decode_p->cellMap_p );
        printTextMap( decode_p->textMap_p );
        
        retbuf = procText4Table( decode_p->textMap_p, decode_p->cellMap_p );

        printCellMap( decode_p->cellMap_p );
        printTextMap( decode_p->textMap_p );
        /*
        */
        return retbuf;
    }


    /*
     * 16. processBT()
     *      处理 BT/ET 内的内容
     * 入口参数:
     *      mm-p           字节流映射文件 哈希表, 读取内容的时候需要.
     *      pages_p        pages_p里面包含字符映射表 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      无
     * 说明:
     *      处理过程基本同BDC 一致， 就是内部不包含BT的判断, 而是包含了BDC处理的嵌套
     *      处理后的内容存放在哪儿？？？？？ 文件？？
     * 2016.12.12:
     *      Tm 的内容是转换坐标系. [a b c d e f], 定义一个list Tm, 存放最新的转换坐标
     *      Td/TD 的坐标实际为
     *           x' = a * x + c * y + e
     *           y' = b * x + d * y + f
     * depends on:
     *       processBDC(), processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     *      
    # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {编号:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {编号:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # 一直存放的是最新的tm数据
     * 
     */
    char * processBT( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p )
    {
        char    * tmpbuf;
        CUR_XY  * cur_xy_p  = & decode_p->cur_xy;
        TM      * tm_p      = & decode_p->tm;

        printf("processBT() buf = %s\n", srcbuf );
        while ( 1 ) {
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf("processBT() tmpbuf = %s\n", tmpbuf );
            
            //# 1. 过滤 页眉，页脚， 水印
            //#    判断是否包含  /Artifact && （/Top   /Bottom   /watermark）任一 
            //#     如果包含， 则过滤后面的BDC, EMC 之间的内容
            //# 2. 即便是页眉或页脚, 里面的坐标也要处理, 否则后面的坐标会出现错误.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # 页眉  , BDC 是起始标识
                while( tmpbuf ) {
                    printf("页眉部分. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# 页眉结束的标识
                        break;
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );

                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    else if ( strstr( tmpbuf, "Tm" )  )
                        processTm( tmpbuf, cur_xy_p, tm_p );                        
                }
            }   

            else if ( strstr( tmpbuf, "/Bottom" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // 页脚的处理
                while ( 1 ) {       // # 下面的循环判断比上面的循环判断可读性要高一点
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# 页脚结束的标识
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BDC" ) ) {       // # 一段文字  处理 BDC/EMC  文字部分
                processBDC( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # 如果 有 re 信息， 表示是表格, 获取cell 信息, 如果碰到Tm, 会置cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # 如有有q 信息, 那么表示保存当前的图形状态信息, 包括坐标信息, 然后到Q 再恢复
            /* 暂时不处理q/Q, 因为re 使用的是绝对坐标
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */

            //# 处理字体信息,这个字体就是当前解码需要的type0字体
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # 如果buf 有 文本位置信息, 则处理后获得当前的xy 坐标
                # 2016.12.13:
                #   不单独处理位置, 这儿只处理Tm 也就是转换坐标信息, 保证最新的转换坐标起作用. 文本的坐标在文本内部处理
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm 处理与tm 处理相同, 调用Tm的处理方法既可
            //#    self.processCm( tmpbuf, tm )
            
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );

            //# 如果buf 有文本信息 (这个时候应该没有BDC/BT, 否则下面的desbuf 应该替换为&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "ET" ) ) {  // # 正文结束的标识
                printf("碰到ET, BT/ET 处理结束. tmpbuf=%s\n", tmpbuf );
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }
    
    /*
     * 17. processBDC()
     *      处理 BDC/EMC 内的内容
     * 入口参数:
     *      streambuf           字节流映射文件 哈希表, 读取内容的时候需要.
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      无
     * 说明:
     *      1. 过滤页眉/页脚, 如果有的话
     *      2. 如果有Td/TD/Tm/T*, 就获取位置信息, 并与之前处理的位置信息进行比较, 来判断是否要换行
     *      3. 如果有Tj,TJ, ', " , 表示文本内容, 摘取文本内容, 如果需要解码则进行解码
     *      4. 如果有re, 一般会对应f*, 也就是画矩形操作  
     *          x, y, width, height    re   // x,y 是左下角的坐标
     * depends on:
     *       self.processBT(), .processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     * 
     */
    char * processBDC( MEMORYMAP *mm_p, char * desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p  )
    {
        char    * tmpbuf;
        CUR_XY  * cur_xy_p  = & decode_p->cur_xy;
        TM      * tm_p      = & decode_p->tm;

        printf("processBDC() buf = %s\n", srcbuf );
        while ( 1 ) {
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf("processBDC() tmpbuf = %s\n", tmpbuf );
            
            //# 1. 过滤 页眉，页脚， 水印
            //#    判断是否包含  /Artifact && （/Top   /Bottom   /watermark）任一 
            //#     如果包含， 则过滤后面的BDC, EMC 之间的内容
            //# 2. 即便是页眉或页脚, 里面的坐标也要处理, 否则后面的坐标会出现错误.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # 页眉  , BDC 是起始标识
                while( tmpbuf ) {
                    printf("页眉部分. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# 页眉结束的标识
                        break;
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );

                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    else if ( strstr( tmpbuf, "Tm" )  )
                        processTm( tmpbuf, cur_xy_p, tm_p );                        
                }
            }   

            else if ( strstr( tmpbuf, "/Bottom" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // 页脚的处理
                while ( 1 ) {       // # 下面的循环判断比上面的循环判断可读性要高一点
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# 页脚结束的标识
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BT" ) ) {       // # 一段文字  处理 BT/ET  文字部分
                processBT( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # 如果 有 re 信息， 表示是表格, 获取cell 信息, 如果碰到Tm, 会置cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # 如有有q 信息, 那么表示保存当前的图形状态信息, 包括坐标信息, 然后到Q 再恢复
            /* 暂时不处理q/Q, 因为re 使用的是绝对坐标
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );


            //# 处理字体信息,这个字体就是当前解码需要的type0字体
            if ( strstr( tmpbuf, "Tf" ) ) {
                //processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # 如果buf 有 文本位置信息, 则处理后获得当前的xy 坐标
                # 2016.12.13:
                #   不单独处理位置, 这儿只处理Tm 也就是转换坐标信息, 保证最新的转换坐标起作用. 文本的坐标在文本内部处理
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm 处理与tm 处理相同, 调用Tm的处理方法既可
            //#    self.processCm( tmpbuf, tm )
            
            //# 如果buf 有文本信息 (这个时候应该没有BDC/BT, 否则下面的desbuf 应该替换为&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "EMC" ) ) {  // # 正文结束的标识
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }

    /*
     * 23. processRE()
     *      处理 re 内的内容
     * 入口参数:
     *      buf                 当前包含TD 的一行信息
     * 出口参数:
     *      cur_cell            哈希表, 记录单元格坐标 "x":x坐标值, "y":y坐标值, "w":width, "h":height
     * 返回值:
     *      无
     * 说明:
     *      如果有re, 则说明是表格, 记录Tj/TJ/'/" 之前的最新的re 的数据为最新的cell 信息。
     *  如果连续有多个re, 那么处理最新的re信息后, cur_cell 也存放的是最新的re信息
     *      如果碰到Tm, 会置cur_cell=[:], pre_cell=[:]
     *      x, y, width, height  re   ,  画一个矩形. x,y 是左下角坐标.
     *      RE 返回的是CELL 的词典, {'re1':[x,y,w,h],'re2',}
     * 2016.12.10:
     *      改变思路, 每次处理re的时候将获取的cell信息存放在一个cellMap中, 所有内容解析出来后,
     *      再结合 textMap和cellMap来处理换行以及跨表格文字的换行问题.
     *      1. 有些pdf文档中re 的x是负数, w 高达好几千, 例如:
     *              -1638.4 72 3795.5 39.1 re
     *      可能是用来控制的, 与内容无关, 可以过滤掉, 未来如果需要进行显示再处理
     *      2. 如果高度小于1, 可能是用来分隔页眉的, 例如:
     *          76.55 55.2 442.2 0.72 re
     *        这种高度时不可能有字体放入的.
     *      3. 获取所有的表格信息后, 还要进行一次过滤, 把包含表格的cell过滤掉, 目前发现只有一种情况会包含表格.
     *        整个页面边框有个表格, 这个表格会包含其它所有的合法表格(控制类re除外).
     *        目前简单处理, 第一个re 就是这种情况, 直接把第一个re过滤即可.
     *          0 0 595.3 841.9 re
     * 2016.12.21:
     *      re 也需要根据Tm 来进行坐标转换, 否则文本位置与表格位置就无法判断正确了.  不需要转换, 每次都是绝对坐标, 似乎与q/Q有关. 会清除之前的坐标转换信息???
     *      cellIndexMap[cellid] = {"maxlen":,"col":, "row":, "txtList":[]}
     * 2016.12.22:
     *      对一种极端情况进行容错. 当前cell完全在另一个有效cell之内, 则丢弃该cell
     * 2017.01.15:
     *      如果当前cell包含之前的某个有效cell, 则用当前cell 替代之前的有效cell
     */
    char * processRE( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p  )
    {
        CELL            *   cp;
        char            *   tmpbuf;
        int                 len;
        int                 i, j, k, n;
        char        buf[128];
        
        printf( "-------------------------------------------------------------------------------processRE(), srcbuf=%s\n", srcbuf );
        if ( !srcbuf ) {
            return NULL;
        }

        //cp = (CELL *)calloc( sizeof(CELL), 1 );
        cp = (CELL *)malloc( sizeof(CELL) );
        memset( cp, 0, sizeof(CELL) );

        len = strlen( srcbuf );
        j = 0;
        n = 1;
        for ( i = 0; i < len; i ++ ){
            if ( srcbuf[i] == ' ' ) {
                memset( buf, 0, 128 );
                memcpy( buf, srcbuf+j, i-j );
                printf( "buf=%s, i=%d, j=%d\n", buf, i, j );
                while ( buf[i] == ' ' )
                    i ++;
                j = i;      // 下一项的起始位置做准备
                
                switch ( n ) {
                    case 1:
                            cp->x = atof( buf );      // 单元格的x坐标 
                            n ++; 
                            break;
                    case 2:
                            cp->y = atof( buf );      // 单元格的y坐标  
                            n ++;
                            break;
                    case 3:
                            cp->w = atof( buf );      // 单元格的w坐标 
                            n ++;
                            break;
                    case 4: 
                            cp->h = atof( buf );      // 单元格的h坐标  
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        //# x 坐标小于0可能是控制用的, h,w数值小于1可能是用来做分割线的, (x,y)=(0,0)从原点来时的表格也是控制用(页眉都不可能在原点)
        //# 把页眉也保留, 用来判断页眉文本
        printf( "-------------------2----------processRE(), srcbuf=%s, x=%.2f, y=%.2f, w=%.2f, h=%.2f, cp=%p\n", srcbuf, cp->x, cp->y, cp->w, cp->h, cp );
        if ( cp->x < 0 || cp->y < 0 || cp->w < 1 || ( cp->x ==0 && cp->y == 0 ) )  //# (0,0)仍然过滤, 暂时没想到怎么处理
            return NULL;

        //# 2017.01.15:  过滤或替代无效的cell
        printf( "----------------------------------------3--------------------------processRE(), srcbuf=%s, cp=%p\n", srcbuf, cp );

        fillCellMap( decode_p, cp );
        
        printCellMap( decode_p->cellMap_p );
        
        return NULL;
    }

    /*
     * 23.1. filterCell()
     *       过滤无效的cell
     *  说明:
     *      有些cell 是无效的, 自身就在另一个有效cell之内。 丢弃即可
     *  2017.01.15:
     *      1. 当前cell 已经在之前的有效cell 内部, 则丢弃
     *      2. 如果当前cell 包含之前的 有效cell内部, 则用当前的cell 替代之前的有效cell   
     */

    int intsertCell( CELL * cellMap_p, CELL cur_cell ) 
    {
        return 0;
    }

    int insteadCell( CELL * cellMap_p, CELL cur_cell, int cellID)
    {
        CELL    *   cp;
        cp = cellMap_p;
        while( cp ) {
            if ( cp->id == cellID ) {
                
                return 0;
            }
        }
        return -1;
    }

    // 查找指定ID的单元格cell
    CELL * findCell( CELL * cellMap_p, int cellID )
    {
        CELL  * cp = cellMap_p;  // 从头开始遍历

        while ( cp ) {
            if ( cp->id == cellID )     // 找到了
                return cp;
            
            cp = cp->next;
        }

        return NULL;    // 遍历完还没有找到就返回NULL
    }


    // 保存cell信息时不区分是否是页眉, 保证数据完整性
    // 处理cell的时候, 判断出是页眉(h<1)页脚再处理或过滤即可
    // 这儿只过滤一种cell:  误差不超过1的, 视作一个cell
    int fillCellMap( DECODE *decode_p, CELL *cp )
    {
        printf("-------------------fileterCell()--cellMap_p=%p----cp=%p---\n", decode_p->cellMap_p, cp );
        printCellMap( decode_p->cellMap_p );

        assert( decode_p );
        printf("after assert(decode_p)------------------------------------------------------------------------------aaaaaaaaaaaaaaaaa\n");
        float       cx, cy, ch, cw; 
        float       px, py, ph, pw;     // 链表中的cell的x, y, w, h
        CELL    *   cpt;                // 链表的最后一个cell


        if ( !cp )              // 要添加的cell为空, 则不操作
            return 0;

        cx = cp->x;
        cy = cp->y;
        cw = cp->w;
        ch = cp->h;


        if ( !decode_p->cellMap_p ) {       // 如果cellMap_p为空, 则这是第一个cell, 直接添加
            printf("cellMap_p 为空,直接添加cp-----------------------------------------111  , \n");
            cp->id = 1;
            cp->last = cp;
            cp->prev = NULL;
            cp->next = NULL;
            decode_p->cellMap_p = cp;
            return 0;
        }
        // 只与最后一个cell比较, 因为每一个新加入的cell都自己的前一个比较(最后一个), 相当于全比较了
        // 因为当前坐标一直在变化, 如果与最后一个cell不一样, 也肯定不会与之前的cell一样了

        printf("----------------2--------------cellMap_p 不为空的处理, cellMap_p=%p, cellMap_p->last=%p\n", decode_p->cellMap_p, decode_p->cellMap_p->last);

        assert( decode_p->cellMap_p->last );
printf("================asf=============\n");        
        cpt = decode_p->cellMap_p->last;       

        px = cpt->x,   py = cpt->y, pw = cpt->w, ph = cpt->h;
            
        if ( !( abs( px - cx ) < 1 && abs( py - cy ) < 1 && abs( px+pw-cx-cw) < 1 && abs( py+ph-cy-ch) < 1 ) )  {
            // 该单元格 没有  被其他cell包含 或者 包含其他单元格, 并且误差小于1
            cpt->next   = cp;
            cp->prev    = cpt;
            cp->id      = cpt->id + 1;
            printf("-2222222222- cpt=%p, cp=%p\n", cpt, cp );
            assert( decode_p );
            printf("222111222\n");
            assert( decode_p->cellMap_p);
            printf("12121212\n");
            assert( decode_p->cellMap_p->last );  
            printf("-33333 cpt=%p, cp=%p\n", cpt, cp );
            decode_p->cellMap_p->last = cp;

            return 0;
        }

        free( cp );     // 该cp 是个无用的, 丢弃, 释放内存

        return 0;
    }


    char * processTf( char * buf, DECODE * decode_p )
    {
        char    * tmpbuf;

        tmpbuf = strsplit1( buf, ' ', 1 );      // /FT15 180 Tf  格式
        strcpy( decode_p->tf, tmpbuf +1 );      // +1 是为了跳过'/'   
        
        free( tmpbuf );                         // 因为strsplit1() 里面申请了内存
        
        return decode_p->tf;
    }

    /*
     *  18. hasText( buf )
     *          判断当前行是否有文本信息标识符  Tj / TJ/ ' / "
     *  入口参数
     *      buf     待处理的字符串
     *  出口参数
     *      无
     *  返回值
     *      False       没有包含文本信息标识符
     *      True        包含文本信息标识符
     *          
     */
    bool hasText( char *buf )
    {
        if ( strstr( buf, "Tj" ) || strstr( buf, "TJ" ) || strstr( buf, "\'" ) || strstr ( buf, "\"") ) 
            return true;
        else
            return false;
    }

    /*
     *  19. processText( buf, curxy, pre_xy, cur_cell, pre_cell )
     *          处理文本信息, 判断是否需要换行, 是否需要解码
     *  入口参数:
     *      
     *  出口参数:
     *      
     *  返回值:
     *      
     *  说明:
     *      1. 判断cur_cell 与 pre_cell, 如果不为空, Y 相同, 则在同一行, 否则换行
     *         1.1 判断 cur_xy 是否在 cur_cell 中, 
     *      2. 只有处理文本以前, 才调用ifCRLF(),然后将cur_xy , cur_cell 保存到pre_xy, pre_cell,
     *         表示上一次处理文本的位置保存了。 才ifCRLF() 中处理位置相关的变量
     *  2016.12.10:
     *      改变实现思路. 新建一个textMap 字典, key:value=起始坐标:文本内容
     *      在re的处理中, 将结果存放在CellMap 字典中.
     *      等所有文本都解析出来后, 处理textMap 以及cellMap, 来进行文本合并以及换行的处理.
     * 2016.12.23:
     *      增加tf信息,实际上也就是字体名称.
     *      下一步用stateStack, cur_state的时候, 字体信息作为状态的一部分. 目前状态包括: 坐标, 字体.
     * depends on:
     *       self.ifCRLF(), .processTj(), .processTJ()
     *          
     */
    //# cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    //# textMap = {编号:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    //# cellMap = {编号:{'x':,'y':,'w':,'h':}}
    //# tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # 一直存放的是最新的tm数据
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p ) 
    {
        // cur_xy, textMap, cellMap, cellIndexMap, tm, tf
        CMAP    * cmap_p;

        if ( !srcbuf || !decode_p || !pages_p )
            return NULL;

        // 找到字体对应的cmap
        for ( int i = 0; i < pages_p->cmaptotal; i ++ ) {
            if ( strcmp( pages_p->cmaps_p[i].fontname, decode_p->tf ) == 0 ) {  // 找到字体对应的CMAP
                cmap_p = &( pages_p->cmaps_p[i] );
                break;
            }
        }
        printf("找到CMAP, fontname=%s, 编码数量:%d\n", cmap_p->fontname, cmap_p->total );
        // 下面开始解码
        
        if ( strstr( srcbuf, "Tj" ) )               // Tj
            processTj( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "TJ" ) )          // TJ
            processTJ( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "\'" ) )  {         // '  单引号, 等同于T*  Tj 也就是移到下一行起始位置输出文本
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf );
        }
        else if ( strstr( srcbuf, "\"" ) )  {         // "   双引号,  使用了字间隔与字符间隔, 内容而言与单引号一样
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf); 
        }
        
        printf( "解码前:%s\n" , srcbuf );
        printf( "解码后:%s|\n", desbuf );
        //printCUR_XY( cur_xy_p );
        //
        // 将文本信息填充到 textMap 中, 便于未来格式处理, 同时判断该文本属于哪个cell, 记录在CELL信息中
        // 
        fillTextMap( desbuf, decode_p );
        
        return NULL;
    }   

    // 将解码后的文本保存在 textMap 以及 cellMap 中
    int fillTextMap( char * buf, DECODE * decode_p )
    {
        float       ox, oy;                   // 文本的x,y 坐标, 为了简化代码中的指针结构名称长度, 增加可读性
        float       cx, cy, cw,ch;          // cell的x,y,w,h 为了简化代码中的指针结构名称长度, 增加可读性
        TEXT    *   tp;                     // 存放新的文本映射单元， 并链接到文本映射表中
        CELL    *   cp;                     // 指向 decode_p->cellMap_p->last
        int         id;
        
        if ( !buf || !decode_p )
            return -1;
    
        tp = ( TEXT * )calloc( sizeof(TEXT), 1 );
        //tp = ( TEXT * )malloc( sizeof(TEXT) );
        //memset( tp, 0, sizeof(TEXT) );

        if ( !decode_p->textMap_p ) {         // 如果 最后一个文本映射为空, 表示这是解析出来的第一个文本串
            tp->id      = 1;
            tp->last    = tp;                          // 对于新建的文本映射表而言, 最后一个也就是第一个
            tp->prev    = NULL;                // 首个节点的prev (前一个节点) 为空NULL
            decode_p->textMap_p = tp;
        } else {
            decode_p->textMap_p->last->next = tp;
            tp->prev = decode_p->textMap_p->last;   // 新建text的前一个text就是之前的last text
            tp->id = tp->prev->id + 1;
            decode_p->textMap_p->last = tp;         // 更新last 指针到新建的 TEXT 节点
        }

        ox = tp->ox = decode_p->cur_xy.ox;            // 文本 绝对坐标x
        oy = tp->oy = decode_p->cur_xy.oy;            // 文本绝对坐标y

        tp->len = strlen( buf );
        tp->buf = (char *)calloc( sizeof(char), tp->len + 1);
        memcpy( tp->buf, buf, tp->len );
        printf("fillTextMap(), tp->buf=%s, tp->id=%d\n", tp->buf, tp->id );

        // 下面来比较文本与单元格的相对位置, 如果文本在单元格中, 则记录相关信息
        // 只判断最后一个单元格, 因为这个坐标与当前的文本坐标最接近, 如果这个不包含, 之前的不可能包含
        if ( !decode_p->cellMap_p ) {     // cell 映射表为空, 表示该文本不在cell中, 直接返回
            return 0;
        }
        cp = decode_p->cellMap_p->last;

        if ( !cp )
            return 0;

        cx = cp->x,  cy = cp->y, cw = cp->w, ch = cp->h;    // cell 的坐标信息
        printf("1===========================cp=%p, cx=%.2f, cy=%.2f, cw=%.2f, ch=%.2f, txtTotal=%d\n", cp, cx, cy, cw, ch, cp->txtTotal );
        
        if ( ( ox >= cx ) && ( ox <= cx+cw ) && ( oy > cy ) && ( oy <= cy+ch ) ) {   // 该文本在cell里面
            if ( tp->len > cp->maxlen )     // 更新cell的最大长度
                cp->maxlen = tp->len; 
            
            tp->cellID = cp->id;
            cp->txtTotal ++;                // 这儿只是计数 cell 中的文本数量, 最后处理完所有文本后, 
                                            // 根据该数字来申请cp->txtIDs_p空间, 再处理一次textMap_p
                                            // 将该cell中的文本的id 填充到cp->txtIDs_p 数组中
        }
        
        return 0;
    }

    /*
     * 20. processTj( desbuf, srcbuf, cmap )
     *      处理含有 Tj 标识 的文字部分
     * 入口参数:
     *      buf     待处理的包含Tj 的未解码文本
     *      cmap    用于本页所有文本解码使用的cmap 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      解码后的文本
     * 说明:
     *      这儿只解析文本内容， 返回解码后的文本
     * 例子:
     <547F>Tj 482.391 0 TD<097E>Tj 482.391 0 TD<0D2D>Tj 480 0 TD<41F1>Tj 482.391 0 TD
<4AD2>Tj 482.391 0 TD<4AF1>Tj 482.391 0 TD<3E7C>Tj 480 0 TD<044A>Tj 482.391 0 TD
<1937>Tj 482.391 0 TD<1BA0>Tj 482.391 0 TD<0836>Tj 482.391 0 TD<34D7>Tj 480 0 TD
<41F1>Tj 482.391 0 TD<0868>Tj 482.391 0 TD<18C5>Tj 482.391 0 TD<4374>Tj 480 0 TD
<0E2A>Tj
     */
    // srcbuf = "547F"  或多个编码  len  = 4 或4的倍数
    //
    int decodeR( char *desbuf, char * srcbuf, int len, CMAP * cmap_p )
    {
        wchar_t     wstr[len/4+1];      // srcbuf 是 "547f"格式, 4字节对应的1个 unicode是2字节 0x547F， 
        char        buf[4+1];
        int         min, max, mid;
        int         count;              // 记录unicode 转换字符串的字节数

        if ( !srcbuf || !cmap_p || len%4 != 0 )   // len除余4 不为0表示格式有问题. 
            return -1;

        min = 0;
        max = cmap_p->total-1;      // 下标是从0开始的

        int     wlen = len/4;      // 
        memset( wstr, 0, sizeof(wstr) );

        for ( int i = 0; i < wlen; i ++ ) {
            memset( buf, 0, 4+1);
            memcpy( buf, &srcbuf[i * 4], 4 );
            printf("buf=%s\n", buf);
            
            wstr[i] = Hex2Int2( buf );     // 这儿wstr[i] 是复用了,先存放编码, 比如<4C75> <9645> 中的0x4c75
            
            // 找编码对应的解码  0x0645, 用折半查找法, 因为 cmap 是从小到大排序的
            while( min + 1 != max ) {
                mid = (min + max )/2;
                if ( cmap_p->code_p[mid].code == wstr[i] ) {
                    wstr[i] = cmap_p->code_p[mid].decode;
                    break;
                }
                if ( wstr[i] > cmap_p->code_p[mid].code )
                    min = mid;
                else
                    max = mid; 
            }
            
        }
        // wstr 中存放的是解码后的内容, 也就是最终的内容, 转换为单字节字符串
        count = unicode2str( desbuf, wlen * 2, wstr );
        printf( "\n---------------转换后的内容为:%s(len=%d)\n", desbuf, count );
        
       return count;     
    }

    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p )
    {
        char    tmpbuf[L_TMPBUF];
        int     i, j, k, n;            // 用来标记下标，获取<> 或() 内的内容, n 用来记录目标缓冲区下标
        int     len;

        if ( !srcbuf )
            return NULL;

        len = strlen( srcbuf );
        printf( "processTj() Begin:%s, len=%d\n", srcbuf, len );

        j = k = n = 0;
        for ( i = 0; i < len; i ++ ) {
            if ( srcbuf[i] == '(' ) {          // ()不需要解码 
                j = i+1;          // j 用来记录截取内容的起始位置
                while( i < len && srcbuf[i] != ')' )   // 找到')'
                    i ++;
                if ( srcbuf[i] != ')' )        // 容错处理
                    return NULL;
                            
                memcpy( &desbuf[n], &srcbuf[j], i-j ); // 复制() 里面的内容到目标缓冲区
                n += i-j;
            } else if ( srcbuf[i] == '<' ) {   // <>表示需要解码   
                j = i+1;                      // j 用来记录截取内容的起始位置
                while( i < len && srcbuf[i] != '>' )   // 找到'>'
                    i ++;
                if ( srcbuf[i] != '>' )        // 容错处理
                    return NULL;
                memset( tmpbuf, 0, L_TMPBUF );            
                memcpy( tmpbuf, &srcbuf[j], i-j ); // 复制<> 里面的内容到目标缓冲区

                // 开始解码
                printf("-------------tmpbuf=%s, %d-%d=%d, n=%d\n", tmpbuf, i, j, i-j, n );
                k = decodeR( &desbuf[n], tmpbuf, i-j, cmap_p );
                if ( k > 0 )
                    n += k;
                
            }       
        }
        return desbuf;
    }

    /*
     * 21. processTJ( buf, cmap )
     *      处理含有 TJ 标识 的文字部分
     * 入口参数:
     *      buf     待处理的包含Tj 的未解码文本
     *      cmap    用于本页所有文本解码使用的cmap 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      解码后的文本
     * 说明:
     *    1. TJ 类型, 表示有文字， 里面包含多个格式的文本
     * 例子:
     *    1.  [<41F7086E>-3<05192E37>-3<03C6>]TJ
     */
    char * processTJ( char * desbuf, char *srcbuf, CMAP * cmap_p )
    {
        return processTj( desbuf, srcbuf, cmap_p );
    }

    /*
     * 24. processQ( streambuf )
     *      处理q/Q 的信息, q 保存图形状态, Q 恢复图形状态
     * 入口参数
     * 出口参数
     * 返回值
     * 说明:
     *      碰到q 后, 直接过滤后面的信息, 直到碰到Q. 主要是图形的处理, 对于本系统而言, 
     *      只关注内容信息, 所以可以过滤掉这些信息
     *      
     *      不行， 不能简单的过滤q/Q中间的内容, q/Q 中间可能会嵌套q/Q, 而且里面也可能有文字内容
     *      用list 来实现堆栈，堆栈里存放那个 pre_cell, pre_xy
     *      经过仔细分析页面信息, 发现q/Q 本质上用处不大, 因为re 都是绝对坐标
     *      暂时不处理q/Q
     */
    void processQ( MEMORYMAP * mm_p )
    {
        char    * buf;

        buf = mm_readLine( mm_p );
        
        while ( !strstr( buf, "Q") ) {
            //# 如果没有碰到Q, 什么也不做, 继续读取下一行。 过滤掉q/Q之间的内容
            free( buf );
            buf = mm_readLine( mm_p );
        }
        free( buf );

        return;
    }

    /*
     * 25. hasTxtPosition()
     *      判断目标字符串是否包含位置信息
     * 入口参数
     *      buf         待处理的字符串
     * 出口参数
     *      无
     * 返回值
     *      false       不包含位置信息
     *      true        包含位置信息
     * 注意:
     *      有些内容也可能包含 Td, TD 等操作符， 应该区别， 即文本内容里面的不进行处理 !hasText( buf )
     * 2016.12.10:
     *      我晕, 上面的处理逻辑错误了..... 在没有编码的情况下, buf 中不可能含有Td等, 而是编码信息,修改逻辑, 删除!hasText() 相关判断
     *      缺少了T* 和 Tm的逻辑判断 - -！
     */
    bool hasTxtPosition( char * buf )
    {    
        if ( strstr( buf, "Td" ) || strstr( buf, "TD" ) || strstr( buf, "\'" ) || strstr( buf, "\"" ) || strstr( buf, "Tm" ) || strstr( buf, "T*" ) ) {
            return true;
        }
        else {
            return false;
        }
    }

    /*
     * 26. precessTxtPosition( buf, cur_xy )
     *      处理包含位置信息的字符串, 将位置信息记录在cur_xy 哈希表中, "x":x坐标值， "y":y坐标值
     * 入口参数
     *      buf         包含位置信息的字符串
     * 出口参数
     *      cur_xy      哈希表 "x":x坐标值， "y":y坐标值
     * 返回值
     *      无
     * 说明:
     */
    void processTxtPosition( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    { 
        if ( strstr( buf, "Td" ) ) {
            processTd( buf, cur_xy_p, tm_p );
            return;
        }
        else if ( strstr( buf, "TD" ) ) {
            processTD( buf, cur_xy_p, tm_p );
            return;
        } else if ( strstr( buf, "Tm" ) ) {
            processTm( buf, cur_xy_p, tm_p );
            return;
        } else if ( strstr( buf, "cm" ) ) {
            processCm( buf, (CM *)tm_p );       // 暂时不处理
            return;
        } else if ( strstr( buf, "T*" ) ) {
            processTstar( buf, cur_xy_p );
            return;
        }
        
        printf("-------------------------s-------------\n");
    }

    /*
     * 27. processTd()
     *      处理 Td 内的内容
     * 入口参数:
     *      buf                 当前包含Td 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      Td 有多种可能性, 0 Tc 0 Tr 265 0 Td 或者 -135.68 -14.78 Td
     *      1. -135.68 -14.78 Td 在 51134 L115, 这个应该是相对坐标。。。
     *      2. 
     *      奇怪, Td 里面的数据到底是相对坐标？还是绝对坐标？？？
     *      3. 0 Tc 0 Tr 48.26 0 Td            50617  L41
     *      4. 0 Tr 144.54 0 Td                 50617 L44
     *      5. /TT0 9 Tf                        50617 L29
     *          529.68 51.72 Td
     *          (74)Tj
     *          1 0 0 1 538.68 51.72 Tm
     *          ( )Tj
     *      6. 不修改cur_cell, pre_cell 的内容.
     * 2016.12.12:
     *      修正实现方式, Td/TD中的坐标必须用Tm 中的矩阵进行转换, 例如:
     *        0.05 0 0 -0.05 80.52 146.45 Tm

     *        <0E2A>Tj 208.797 0 TD<4801>Tj 211.188 0 TD<31B0>Tj 208.797 0 TD<3020>Tj
     *      那么: 转换矩阵对应的[a b c d e f]=[0.05 0 0 -0.05 80.52 146.45]
     *      根据转换公式: (参见: PDF 1.7 Reference P208)
     *          x' = a*x + c*y + e
     *          y' = b*x + d*y + f
     *      则208.797 0 TD 实际的坐标为:
     *        x' = 0.05 * 208.797 + 0*0 + 80.52 = 184.9165 约184.917
     *        y' = 0 * 208.797 + (-0.05)*0 + 146.45 = 146.45
     *      现在判断文字前面的表格
     *         75.15 134.4 130 16.1 re
     *       75.15 < x' < 75.15+130=205.15
     *       134.4 < y' < 134.4+16.1 = 150.5
     * 2016.12.26:
     *      Td 的处理比较复杂.
     *      1. 如果Tm为空,  如果之前没有坐标, 则(ox,oy) = (x,y); 如果之前有坐标, 则(ox,oy) = (x,y)+(x',y')
     *      2. 如果Tm 不为空, (ox, oy) = (x,y) * tm, 然后 tm 置空
     *
     * depends on:
     *      self.getOriginXY()
     *   
     * 
     */
    // getXYofTd() 函数是本地函数, 简化代码的,仅在processTd() 中调用
    // 0 Tc 0 Tr 48.26 0 Td
    // 获取Td 之前的xy 坐标 
    int getXYofTd( char * buf, CUR_XY * cur_xy_p )
    {
        int     ret;
        char    buf1[12], buf2[12], buf3[12];

        int     i = 0,j = 0;
        int     len;
       
        if ( !buf )
            return -1; 
        
        len = strlen( buf );

        memset( buf1, 0, 12 );
        memset( buf2, 0, 12 );
        memset( buf3, 0, 12 );
        
        while ( i <= len ) {  // 直到尾零
            if ( buf[i] == ' ' || buf[i] == 0 ) {  // 碰到空格或结尾
                memset( buf1, 0, 12 );
                strcpy( buf1, buf2 );       // 第一次循环到这儿, 其实是没有内容的,是空串拷贝, 后续就有内容了
                
                memset( buf1, 0, 12 );
                strcpy( buf2, buf3 );

                memset( buf3, 0, 12 );
                memcpy( buf3, &buf[j], i-j );
                if ( strstr(buf3, "Td" ) )
                    break;
                j = i+1;  // 空格后下一个字符
            }
            i ++;
        }

        cur_xy_p->x = atof( buf1 );
        cur_xy_p->y = atof( buf2 );

        return 0;
    }

    void processTd( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        getXYofTd( buf, cur_xy_p );     // 获取 Td 之前的xy坐标
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "Td" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // 全为0表示没有Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]赋值
            memset( tm_p, 0, sizeof(TM) );      // 重置tm_p的内容为0
        }
    }

    /*
     * 28. processTD()
     *      处理 TD 内的内容, 获取(x,y) 坐标信息
     * 入口参数:
     *      buf                 当前包含TD 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *
     *  2016.12.10:
     *     原来的处理有问题, :<0016>Tj 91.188 0 TD<0015>Tj
     *     <547F>Tj 180 0 TD<097E>Tj 180 0 TD<4AD2>Tj 180 0 TD<4AF1>Tj 180 0 TD<0836>Tj 180 0 TD<34D7>TjET
     *     这种情况要嵌套处理， 直到一行处理结束(比如找到最后一个操作符 ET)
     *  2019.02.14:
     *     碰到Tj(如果里面有TD)或TD 要互相嵌套处理一整行,确保所有内容都处理完全.
     *     不需要处理TD后面的所有TD, 只需要处理第一个TD即可. 因为所有的TD的y 坐标相同, 之所有时多个TD, 是因为X坐标不同
     *
     * 注意: 
     *     这儿如果是表格内的内容, 可能需要处理最后一个TD来获取最新的位置信息. 待调试！！
     *
     */
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ) 
    {
        getXYofTd( buf, cur_xy_p );     // 获取 Td 之前的xy坐标
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "TD" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // 全为0表示没有Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]赋值
            memset( tm_p, 0, sizeof(TM) );      // 重置tm_p的内容为0
        }

    }    

    /*
     * 29. getOriginXY()
     *      获取Td/TD 坐标的Matrics 坐标，因为Td/TD 大部分情况是相对坐标. 
     * 说明:
     *      1. 该方法只有processTd()和 processTD() 调用
     *      2. Tm, re 本身是绝对坐标, 不会调用该方法
     *      3. T* 也不用调用该方法, 因为它直接换行了, 不用进行判断了。
     * 
     *      
     */
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p )
    {
        printf("getOriginXY():-------------======================================\n");
        //print(cur_xy)
        //print(tm)

        // 下面来获取ox, oy, 也就是参考坐标的原始坐标值(参照物), 如果x,y 本身就是绝对坐标,则ox,oy = 0,0
        if ( !tm_p ) {      //# 如果没有tm, 也就是没有转换坐标, 则x,y 就是绝对坐标
            if ( cur_xy_p->ox == 0 ){   //   # 如果该坐标是第一个坐标
                cur_xy_p->ox = cur_xy_p->x;
                cur_xy_p->oy = cur_xy_p->y;
            } else {
                cur_xy_p->ox += cur_xy_p->x;
                cur_xy_p->oy += cur_xy_p->y;
            }
        } else {    // 如果有tm, 要进行坐标转换
            //# x' = a*x + c*y+e,  y'=b*x+d*y+f
            cur_xy_p->ox = tm_p->a * cur_xy_p->x + tm_p->c * cur_xy_p->y + tm_p->e;
            cur_xy_p->oy = tm_p->b * cur_xy_p->x + tm_p->d * cur_xy_p->y + tm_p->f;
        }
    }            

    /*
     * 30. processTm()
     *      处理 Tm 内的内容 0.05 0 0 -0.05 233.759 92.57 Tm
     * 入口参数:
     *      buf                 当前包含 Tm 位置信息 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      特别需要注意的是, Tm 是绝对坐标, 处理Tm时, pre_cell, cur_cell 就可以置为[;], preT_xy 也置空[:]
     *      也就是说后面需要以 Tm 的坐标为基准进行计算与判断
     * 2016.12.21:
     *      增加了cur_xy参数. 因为一旦碰到Tm, 那么之前的坐标都没用了. 需要按照新的转换坐标来计算
     *      碰到Tm后, 初始化 (x,y)= (0,0)*Tm= (e,f)
     *      似乎所有的Tm 都在BT/ET 中间. 或许 ET之后, 该Tm 就失效了.
     */
    void processTm( char *srcbuf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        char    buf[12];
        int     i, j, k, n;
        int     len;

        printf("==========================-----------------processTm(),srcbuf=%s\n",srcbuf);

        len = strlen( srcbuf );
        j = 0;
        n = 1;
        for ( i = 0; i < len; i ++ ){
            if ( srcbuf[i] == ' ' ) {
                memset( buf, 0, 12 );
                memcpy( buf, srcbuf+j, i-j );
                printf( "--------processTm()------------buf=%s, i=%d, j=%d\n", buf, i, j );
                while ( srcbuf[i] == ' ' )
                    i ++;
                j = i;      // 下一项的起始位置做准备
                

                switch ( n ) {
                    case 1:
                            tm_p->a = atof( buf );      //第1项是 tm_p->a 的值 
                            n ++; 
                            break;
                    case 2:
                            tm_p->b = atof( buf );      //第2项是 tm_p->b 的值 
                            n ++;
                            break;
                    case 3:
                            tm_p->c = atof( buf );      //第3项是 tm_p->c 的值 
                            n ++; 
                            break;
                    case 4: 
                            tm_p->d = atof( buf );      //第4项是 tm_p->d 的值 
                            n ++;
                            break;
                    case 5: 
                            tm_p->e = atof( buf );      //第5项是 tm_p->e 的值 
                            n ++;
                            break;
                    case 6: 
                            tm_p->f = atof( buf );      //第6项是 tm_p->f 的值 
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        
        cur_xy_p->x = cur_xy_p->ox = tm_p->e;       // 重置坐标值x
        cur_xy_p->y = cur_xy_p->oy = tm_p->f;       // 重置坐标值x

        memset( cur_xy_p->opr, 0, L_OPR );
        strcpy( cur_xy_p->opr, "Tm" );

        //printTM( tm_p );
        //printCUR_XY( cur_xy_p );

        printf("--------processTM() end !srcbuf=%s\n", srcbuf );
        return;
    }

    /*
    # 30.1 processCm()
    #     转换矩阵, 目前看作用和Tm类似, 回头再深研究
    */
    void processCm( char * buf, CM * cm_p )
    {        
        printf("=================-略过------------------------processTm(),%s\n", buf );
        // 暂时略过
        return;
    }
    
    /*
     * 31. processTstar()
     *      处理 T* 内的内容
     * 入口参数:
     *      streambuf           字节流映射文件 哈希表, 读取内容的时候需要.
     *      buf                 当前包含T* 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      T*   直接到下一行的起始位置
     *2016.12.13:
     *    这个处理需要完善？？？？？
     *    如果找到T*, 那么前面i-2, i-1就是x,y 
     */
    void processTstar( char * buf, CUR_XY * cur_xy_p )
    {
        printf("processTstar() buf=%s\n", buf);
        cur_xy_p->x = 0;         
        cur_xy_p->y += 1;           //# 设+1 表示与上次处理的位置相比, 下移了, 那么就是下一行
        strcpy( cur_xy_p->opr, "T*" );
        /*
        # 注意， 上面的操作是假设T* 不会是第一个位置操作符， 目前测试也
        # 确实没有第一个的情况, 但是如果真有T*是第一个位置操作符, 就会出异常
        # 因为这个时候pre_xy["x"]等访问是无效的。
        */
        return;
    }
    
    void printTM( TM * tm_p )
    {
        if ( ! tm_p )
            return;

        printf( "----------tm_p->a=%.3f\n", tm_p->a );
        printf( "----------tm_p->b=%.3f\n", tm_p->b );
        printf( "----------tm_p->c=%.3f\n", tm_p->c );
        printf( "----------tm_p->d=%.3f\n", tm_p->d );
        printf( "----------tm_p->e=%.3f\n", tm_p->e );
        printf( "----------tm_p->f=%.3f\n", tm_p->f );
    }

    void printCUR_XY( CUR_XY * cur_xy_p )
    {
        if ( ! cur_xy_p ) 
            return;

        printf( "----------------cur_xy_p->x=%.3f\n", cur_xy_p->x );
        printf( "----------------cur_xy_p->y=%.3f\n", cur_xy_p->y );
        printf( "----------------cur_xy_p->ox=%.3f\n", cur_xy_p->ox );
        printf( "----------------cur_xy_p->oy=%.3f\n", cur_xy_p->oy );
        printf( "----------------cur_xy_p->opr=%s\n", cur_xy_p->opr );
    }

    //====================================================================
    // 下面的部分是对文本及表格的后续处理， 还原格式
    //
    //
    void printCellMap( CELL * cellMap_p )
    {
        CELL    * cp = cellMap_p;
        
        printf("--------printCellMap()------------------\n");

        if ( !cp ) { 
            printf("------------------cellMap 是 空的?-----------------------------------------\n");
            return;
        }

        while ( cp ) {
            printf( "-------------cell id=%d--address=%p---------\n", cp->id, cp );
            printf( "x=%.2f,y=%.2f,w=%.2f, h=%.2f\n", cp->x, cp->y, cp->w, cp->h );
            printf( "rows=%d, cols=%d\n",  cp->rows, cp->cols );
            printf( "maxlines=%d, maxlen=%d, txtTotal=%d\n", cp->maxlines, cp->maxlen, cp->txtTotal );
            for ( int i = 0; i < cp->txtTotal; i ++ )
                printf( "%d ", cp->txtIDs_p[i] ); 

            printf("\n");
            cp = cp->next;
        }
    }

    void printTextMap( TEXT * textMap_p )
    {
        TEXT    * tp = textMap_p;
        
        if ( !tp ) 
            return;

        while ( tp ) {
            printf( "-------------text id=%d-----------\n", tp->id );
            printf( "ox=%.2f, oy=%.2f, cellID=%d\n", tp->ox, tp->oy, tp->cellID );
            printf( "len=%d, buf=%s\n",  tp->len, tp->buf );
            printf( "tp->prev=%p, tp=%p,tp->next=%p\n", tp->prev, tp, tp->next );
            tp = tp->next;
            printf("------\n");
        }

        printf("printTextMap() End\n");
    }

    //
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p );
    void preProcTxtMap4Cell( TEXT * textMap_p, CELL * cellMap_p );
    /*
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap );
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    */
    char * procText4Table( TEXT * textMap_p, CELL * cellMap_p )
    {
        // # 1. 处理textMap, 将不在表格中的文本进行处理, 如果是同一行的就进行合并, 同时将cell中的txtIDs_p 补充完整
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
        //preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        /*       
        // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
        //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
        self.preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
        
        // # 4. 解析文本与cell的关系, 拆分出table 出来, 创建tableMap={1:[cellid_list]}， 同时在cellMap 中添加tableid 属性
        tableMap = {}
        self.buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print("--after buildTableMap()---cellIndexMap-----------")
        print(cellIndexMap)
        print(tableMap)

        // # 5. 输出文本. 已经处理过同行多文本的拼接, 不再cell中的文本就是单独一行文本
        retbuf = self.buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print(retbuf)
        
        return retbuf
        */
        return NULL;
    }

    
    // 删除当前的节点
    void delCurTxt( TEXT * tp )
    {
        TEXT    * tpp, * tpn;    // 当前节点的前一个节点和后一个节点
        
        if ( !tp ) 
            return;
        
        tpp = tp->prev;
        tpn = tp->next;

        free( tp->buf );
        free( tp );

        if ( !tpp ) {       // tpp 为空, 说明要删除的tp是第一个节点, 则后一个节点变成第一个
            tp = tpn;
            tp->prev = NULL;
            return;
        }
        
        if ( !tpn ) {       // 如果tpn为空, 说明要删除的tp 是最后一个节点
            tpp->next = NULL;
            return;
        }

        tpp->next = tpn;
        tpn->prev = tpp;

    }    

    // 合并2个文本节点中的文本内容到第一个文本节点的buf中, 
    // 只是为了简化代码，增加可读性而剥离出来的函数
    void combineBuf( TEXT * tp1, TEXT * tp2 )
    {
        char        * buf;      // 用来申请空间并存放合并后的文本信息

        if ( ! tp1 || !tp2 ) {
            printf("有空指针！！！\n");
            return ;
        }

        printf("合并前, tp1->buf=%s, tp1->len=%d, tp1->id=%d, tp2->buf=%s, tp2->len=%d, tp2->id=%d\n", 
                        tp1->buf, tp1->len, tp1->id, tp2->buf, tp2->len, tp2->id );

        // 先为合并后的文本重新申请空间并保存合并后的文本, 然后释放之前的2个文本内存空间
        buf = ( char * )calloc( tp1->len + tp2->len + 1, 1 );   // +1 是为了存放尾0
        memcpy( buf, tp1->buf, tp1->len );
        memcpy( buf+tp1->len, tp2->buf, tp2->len );

        free( tp1->buf );
        
        tp1->buf = buf;
        tp1->len += tp2->len;
        
        printf("合并后的内容为:%s(textID=%d), len=%d\n", tp1->buf,tp1->id, tp1->len);
    }


    // # 1. 预处理textMap, 将不再表格中的文本进行处理, 如果是同一行的就进行合并
    //# 15.3 preProcTxtMap()
    //#     预处理textMap, 仅限于不在单元格里面的文本进行处理,  将原本在一行的拼接在一行, 删除多余的文本编号, 修正链表
    //#     页眉内的文本也进行拼接处理
    //# depends on:
    //#       delTxt( item, textMap, cellIndexMap )
    //# 2017.01.10:
    //#     更改实现方式, 循环按照 oy 坐标从上往下处理
    //# 2019.02.24
    //      对于C语言版本， 增加cellMap_p 的预处理, 也就是为由文本的cell的txtIds_p 申请空间
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点
        CELL        * cp;

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标

        if ( !textMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID != 0 ) {   // 不等于0, 表示该文本在cell中, 先跳过不处理
                tp = tp->next;
                continue;        
            }
            
            if ( !tp->prev )  {          // 如果没有前一个文本，就不处理, 也就是第一个文本
                tp = tp->next;
                continue;
            }

            tpp = tp->prev;             // 用tpp 是为了提高代码可读性, 不然多级指针看起来费劲

            ox = tp->ox,        oy = tp->oy;
            px = tpp->ox,       py = tpp->oy;

            printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
            if ( abs( py - oy ) < 1 ) {     //  # 两个Y坐标小于1视为同一行

                
                if ( px <= ox ) {        // 留小不留大, 小的在前面
                    // tpp 小, 保留tp->prev
                    combineBuf( tpp, tp );

                    delCurTxt( tp );                    // 删除tp 节点
                    tp = tpp->next;                     // tp 重新定位到下一个需要处理的节点
                } else {
                    // tp 小，保留 tp
                    combineBuf( tp, tpp );


                    delCurTxt( tpp );                    // 删除tpp 节点
                    
                    tp = tp->next;                      // tp 重新定位到下一个需要处理的节点 
                }
            } else {
                printf("不用合并！\n");
                tp = tp->next;
            }
        }     

        // 2. 下面为单元格 的文本编号记录申请空间(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal != 0 )
                cp->txtIDs_p = (int *)calloc( sizeof(int), cp->txtTotal );   // 为存放记录cell中的文本编号申请空间
            cp = cp->next;
        }

        printf("preProcTxtMap() end \n");
    }

    // 从当前节点往前查找指定ID的 TEXT
    TEXT * searchForward( TEXT * textMap_p, int id )
    {
        TEXT        * tp;

        if ( !textMap_p ) 
            return NULL;

        tp = textMap_p;

        while ( tp ) {
            if ( tp->id == id )
                return tp;

            tp = tp->prev;
        }

        // 向前遍历完后没找到
        return NULL;
    }

    // 遍历完后没有找到指定id的CELL
    CELL *getCellByID( CELL * cellMap_p, int id )
    {
        CELL        * cp;

        if ( !cellMap_p ) 
            return NULL;

        cp = cellMap_p;

        while ( cp ) {
            if ( cp->id == id )
                return cp;

            cp = cp->next;
        }

        // 遍历完后没找到
        return NULL;
    }

    // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
    // 处理思路:
    //    1. 遍历一遍所有的text节点, 跳过不在cell中的节点
    //    2. 对于在cell中的文本节点, 找到对应的cell, 然后与该cell中的之前的文本进行比较, 看是否需要合并文本(同一行)
    void preProcTxtMap4Cell( TEXT *textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp, * tpn;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点, tpn 是下一个节点
        CELL        * cp;
        int         cell_id;   
        int         text_id;  

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标

        int         i;                  // 数组下标

        if ( !textMap_p || !cellMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID == 0 ) {   // 等于0, 表示该文本没有在cell中, 跳过不处理
                tp = tp->next;
                continue;        
            }

            cp = getCellByID( cellMap_p, tp->cellID );
            // 此时, 该文本的id 并没有存放在cell 中的txtIDs_p 数组中, 之前的处理仅仅是申请了足够的存放空间
            // 在这儿便处理便将处理后的文本的id存放到txtIDs_p 数组中
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->txtIDs_p[i] == 0 )   // 碰到结尾了
                    break;
            
            if ( i == 0 ) { // 表示该文本是cell 中的第一个文本, 记录在txtIDs_p[0] 中, 不做合并文本处理
                cp->txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {   // 容错处理,表示预申请空间已经存满了, 那么当前的这个文本就不应该在该cell中
                return NULL;
            } else  {
                text_id = cp->txtIDs_p[i-1];
                tpp = searchForward( tp, text_id );
                if ( !tpp )                     // 容错处理 
                    return NULL;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {     //  # 两个Y坐标小于1视为同一行
                    // 合并文本, 保留 tpp, 因为tpp已经记录了在cell中了
                    combineBuf( tpp, tp );
                    if ( tpp->len > cp->maxlen )    // 更新cell中的最大文本长度
                        cp->maxlen = tpp->len;

                    tpn = tp->next;
                    delCurTxt( tp );                    // 删除tpp 节点
                    
                    tp = tpn;                      // tp 重新定位到下一个需要处理的节点 
                } else {    // 不需要合并
                    cp->txtIDs_p[i] = tp->id;       // 添加cell中的文本记录
                    tp = tp->next;
                }
            }

        }


        return;
    }
    /*
    // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
    colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
    {
        return;
    }
    
    // # 4. 解析文本与cell的关系, 拆分出table 出来, 创建tableMap={1:[cellid_list]}， 同时在cellMap 中添加tableid 属性
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }

    // # 5. 输出文本. 已经处理过同行多文本的拼接, 不再cell中的文本就是单独一行文本
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }
*/
