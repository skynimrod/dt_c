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
    // ����Ľ������ļ��ڲ�ʹ��
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
    //      #     ����ķ���ȫ�����Ǵ������ݵ�, ��������, ��ʽ������
    //      #  2016.12.21:
    //      #      ɾ����IFCTRL1() �жϻ��е�ʵ�ַ�ʽ������֮ǰ�ķ���.
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
        int     cursor;                 // �α�, ���һ���Ǽǵ�xref��ַλ�ú����λ��
        long    xrefpos[128];           // ������������128��xref��ַ
    }POSMAP;

    // �������ñ�  XREF ��Ϣ
    typedef struct __xref__ {
        int                 retcode;
        int                 objTotal;   // ��������
        long              * objpos_p;       // �����ַָ��, objpos_p[0]�����ݵ����ݾ��ǵ�1������ĵ�ַ
    } XREF;

    // trailer ��Ϣ�а���: ��object��ţ�object����,  startxref( xref λ����Ϣ)
    #define L_ID  128             // �趨 Trailer�е�ID ��󳤶�Ϊ128�� һ���ǲ��ᳬ���������
    typedef struct __trailer__ {
        int         Root;       // ������ID
        int         Size;       // ��������. �ȶ�������Ŵ�1. ���Ϊ0�Ķ��� �������ݶ���.
        // Encrypt    // ����ļ���������
        long        Info;       // ��ѡ. ��¼�ĵ���Ϣ�Ķ���ID.   ������Ϣ�ȵ�
        char        ID[ID_LEN+1];         // ��ѡ�� Ϊ�˱�֤�ļ���Ψһ��, ǿ�ҽ����и�����. 
        long        Prev;       // ǰһ��Trailer�ĵ�ַ.
        long        xrefStm;    // ���õ�xref��ַ
        int         retcode;    // ������ӵ�����, �����жϸ�Trailer�Ƿ���Ч
    } TRAILER;

    #define L_FONTNAME      64
    typedef struct __fontmap__ {    // һ������ռ��һ��FONTMAP
        int         obj;                        // �����Ӧ�Ķ�����
        char        fontname[L_FONTNAME+1];     // ��������
        int         isType0;                    // 1: ��type0, 0 : ����type0, Ŀǰû����, ��ʱ����
    }FONTMAP;

    // ������ CMAP ���ֵ����ݽṹ����
    typedef struct __code_ {        // <0005> <0022>   int ��4�ֽ�, �ַ���Ҳ��4�ֽ�, �Ǹ��Ƚ����������ά����������������ַ�ʽ
        int         code;       // 4λ����   0005
        int         decode;     // 4λ����   0022

    } CODE;
    typedef struct __cmap__ {
        char        fontname[32];   // һ���������ֲ��ᳬ��32���ַ�, ��ʱ��32�ֽ����
        int         obj;            // ����Type0 ����,��ʼʱ���������, ������ŵ���Type0�����Ӧ��Unicode������
        int         total;          // ��������
        CODE        * code_p;       // ����, �������
        float       comprRatio;      // CMAP ѹ����, ����������˽�ѹ���ʵ�, ���ڹ���ʵ��û���ô�
    } CMAP;

    typedef struct __pages__ {
        int         total;      // ��ҳ��
        int         cursor;     // ���һ��Ҷ��ҳ�����洢��leafs�����е�λ��(�±�), �����Ϊ�˱��ڴ���
        int     *   leafs_p;        // ����, �������Ҷ��ҳ�������
        int     **  c_objlist_p;    // leafs_p�е�Ҷ�Ӷ�����������ݶ����б� c_objlist_p[total][�����б�], �����б��һ�����б�����
        FONTMAP **  fontmap_p;      // fontname[ҳ����total][���ٸ�����][FONTMAP]
        int         cmaptotal;      // cmap ��������
        CMAP    *   cmaps_p;  // CMAP����, ���е�cmap������������, ��Ϊ��ͬ��Ҳ�Ḵ�úܶ���ͬ��cmap
        float       **comprRatio;      // ÿҳ��������ѹ����, ����������˽�ѹ���ʵ�, ���ڹ���ʵ��û���ô�, ����, ��Ӧc_objlist_p, �ڶ�������ĵ�һ��Ԫ����Ҷ�Ӷ�������
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
    
    // ���溯�������ݽ�������
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
    //  �������� : parsePDF()
    //  ��    �� :
    //		���� pdf�ļ�, ��������Ľ�������desfile�ļ���.
    //  ��ڲ��� :
    //       desfile :      ����������ݴ�ŵ��ļ���
    //       srcfile :      ��Ҫ������pdf�ļ�
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         int         <0  ��ʾ����, -1 ��ʾ�ĸ�ʽ����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //	    ����ӿڽ��ǽ�pdf�ļ�����, ��û����ȡ����. ��ȡ���ݽӿ�Ϊ: 
    //	        1. getSpecPage( )
    //	        2. getSpecKeyLine( )
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr() 
    //         �õ��ı��ļ��ڵ�API
    //             1. getStartxrefPos()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    PDF * DLL_EXPORT parsePDF1( char * srcfile )
    {
        int             ret;
        long            xrefpos;                // xref ��ʼλ��
        PDF         *   pdf_p;

        pdf_p = (PDF *)calloc( sizeof(PDF), 1 );
        //pdf_p = (PDF *)malloc( sizeof(PDF) );
        //memset( pdf_p, 0, sizeof(PDF) );
        
        initPDF( pdf_p );

        pdf_p->fm_p = initFileMap( srcfile );
        print_fm( pdf_p->fm_p );

        xrefpos   = getStartXrefPos( pdf_p->fm_p );          // ��ȡstartxref ������, Ҳ���ǵ�һ��xref ��λ��
        if ( xrefpos < 0 ) {
            printf("������!\n");
            freeALL2( pdf_p );
            return NULL; 
        }

        // 1. ��ȡXREF �� Trailer
        ret = getXREF( pdf_p->fm_p, pdf_p->xref_p, xrefpos, pdf_p->posmap_p, pdf_p->trailer_p );

        if ( pdf_p->xref_p->retcode < 0 || pdf_p->trailer_p->retcode < 0 ) {
            printf("������\n");
            freeALL2( pdf_p );
            return NULL;
        }
        print_trailer( pdf_p->trailer_p ); 
        printXREFContent( pdf_p->fm_p, pdf_p->xref_p, "F:/F_T_tmp/tmp1/xrefAll_c.txt" );

        // 2. �����ȡҳ�� Ҷ�Ӷ�����Ϣ PAGES->leafs_p
        ret = getPages( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p, pdf_p->trailer_p->Root );

        // ������CMAP ��صĴ���
        // 3. ��ȡÿҳ��Ӧ�����ݶ����б�pages_p->c_objlist_p �� �����б� pages_p->fontmap_p
        getContentMap( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );
        getFontMap( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );

        // 3.1 ���������б� pages_p->fontmap_p, ��ȡ cmap �б�
        getCMAPS( pdf_p->fm_p, pdf_p->xref_p, pdf_p->pages_p );
        
        return pdf_p;
    }

    int DLL_EXPORT parsePDF( char * desfile,  char * srcfile )
    {
        int             ret;
        FILEMAP       * fm_p;
        long            xrefpos;                // xref ��ʼλ��

        POSMAP      *   posmap_p;               //  ��Ϊ�޷�ȷ��ʵ���м���xref, ������¼�Ѿ��������xref, ��ֹ������ѭ���� ���128��, 
        XREF        *   xref_p = NULL;          //  �����Ч��xref ��Ϣ.
        TRAILER     *   trailer_p ;             //  ��� ��Ч��trailer��Ϣ

        PAGES       *   pages_p;                //  ���ҳ���Ӧ���ݴ�ŵĶ�����Ϣ(Content����)

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        xrefpos   = getStartXrefPos( fm_p );          // ��ȡstartxref ������, Ҳ���ǵ�һ��xref ��λ��

        if ( xrefpos < 0 ) {
            freeFileMap( fm_p );
            return -1; 
        }
       
        // posmap_p ��ʼ��
        posmap_p = (POSMAP *)malloc( sizeof(POSMAP) + 1 );
        memset( posmap_p, 0, sizeof(POSMAP) + 1 );
        
        trailer_p = (TRAILER *) malloc( sizeof(TRAILER) +1  );
        memset( trailer_p, 0, sizeof(TRAILER) + 1 );

        xref_p = (XREF *)malloc( sizeof(XREF) + 1 );
        memset( xref_p, 0, sizeof(XREF) + 1);

        // 1. ��ȡXREF �� Trailer
        ret = getXREF( fm_p, xref_p, xrefpos, posmap_p, trailer_p );

        if ( xref_p->retcode < 0 || trailer_p->retcode < 0 ) {
            printf("������\n");
            freeAll( posmap_p, trailer_p, xref_p, pages_p, fm_p );
            return -1;
        }
        print_trailer( trailer_p ); 
        printXREFContent( fm_p, xref_p, "F:/F_T_tmp/tmp1/xrefAll_c.txt" );

        // 2. �����ȡҳ�� Ҷ�Ӷ�����Ϣ PAGES->leafs_p
        pages_p = (PAGES *)malloc( sizeof(PAGES) + 1 );
        memset( pages_p, 0, sizeof(pages_p) + 1);

        ret = getPages( fm_p, xref_p, pages_p, trailer_p->Root );

        // ������CMAP ��صĴ���
        // 3. ��ȡÿҳ��Ӧ�����ݶ����б�pages_p->c_objlist_p �� �����б� pages_p->fontmap_p
        getContentMap( fm_p, xref_p, pages_p );
        getFontMap( fm_p, xref_p, pages_p );

        // 3.1 ���������б� pages_p->fontmap_p, ��ȡ cmap �б�
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

    // ��ȡָ��ҳ������
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
        
        count = pdf_p->pages_p->c_objlist_p[pageno-1][0];           // ��0��Ԫ����Ҷ��ҳ������, �п����Ƕ��
        printf( "��%dҳ��Ҷ�Ӷ�����%d��\n", pageno, count );

        for ( int i = 1; i <= count; i ++ ) {
            obj = pdf_p->pages_p->c_objlist_p[pageno-1][i];
            printf("��%d��Ҷ�����ݶ���Ϊ: %d\n", i, obj );
            buf = (char *)getItemOfObj( pdf_p->fm_p, pdf_p->xref_p, obj, "Length" );    // <</Filter/FlateDecode/Length 1117>>stream
            if ( !buf )     
                return NULL;

            //cutTail( buf );                         // ȥ�� 1117>> �����>>
            tmpbuf = (char *)strsplit1( buf, ' ', 2 );
            len = atoi( tmpbuf );     //
            free( tmpbuf );
            
            free( buf );
            buf = (char *)getObjContent( pdf_p->fm_p, pdf_p->xref_p, obj ); 
            if ( !strstr( buf, "stream" ) ) {   // ��ǰ��û��stream�Ļ�, 
                free( buf );
                buf = fm_readLine( pdf_p->fm_p );            // ���� stream\r\n ��һ�з�����
            }
            free( buf );
            srcbuf = (uchar *)fm_read( pdf_p->fm_p, len, pdf_p->fm_p->pos ); // ��ȡָ�����ȵ�stream�ֽ���
        
            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_row_stream.dat", obj );
            writeRawData( filename, srcbuf, len );

            dlen = len * 10;                    // ע��, dlen �����ʼ���㹻��Ž�ѹ�������, �����ѹ��ʧ�� ***** ����Ҫ *****
            desbuf = (uchar *)calloc( dlen, 1 );
            err = uncompress( desbuf, &dlen, srcbuf, len );
            // д�ļ�ʱ���Դ��룬 ����鿴�� ���տ���ɾ����ע��
            CHECK_ERR(err, "uncompress");    // ��֣�������ѹ�ɹ���, ���Ƿ��ص�ȷʵdata_error (-3), ֻ�����θü����, ԭ����dlenû�и�ֵ��ֵ̫С
            sprintf( filename, "f:/F_t_tmp/tmp1/c_%d_stream.txt", obj );
            writeRawData( filename, desbuf, dlen );     // ��� dlen û�г�ʼ���㹻��, �ͻ��ѹʧ��
            printf("=====================��ѹ������ݴ�����ļ�:%s\n", filename );
            
            // ��¼һ��ѹ����, �˽�ѹ���ɶ�
            pdf_p->pages_p->comprRatio_p[pageno-1][i] = (float)dlen/len;
            printf(" ѹ����Ϊ:%2.2f\n", pdf_p->pages_p->comprRatio_p[pageno-1][i] );

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


