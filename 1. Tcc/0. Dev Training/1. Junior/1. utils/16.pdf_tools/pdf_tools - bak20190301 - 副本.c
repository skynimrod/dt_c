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
    
    // ����ĺ���Ϊ���Ժ���, �����������Ϣ������ļ���, ���ڲ���
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

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

        pdf_p = (PDF *)malloc( sizeof(PDF) );
        memset( pdf_p, 0, sizeof(PDF) );
        
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
        pdf_p->posmap_p = (POSMAP *)malloc( sizeof(POSMAP) + 1 );
        memset( pdf_p->posmap_p, 0, sizeof(POSMAP) + 1 );
        
        pdf_p->trailer_p = (TRAILER *) malloc( sizeof(TRAILER) +1  );
        memset( pdf_p->trailer_p, 0, sizeof(TRAILER) + 1 );

        pdf_p->xref_p = (XREF *)malloc( sizeof(XREF) + 1 );
        memset( pdf_p->xref_p, 0, sizeof(XREF) + 1);
            
        pdf_p->pages_p = (PAGES *)malloc( sizeof(PAGES) + 1 );
        memset( pdf_p->pages_p, 0, sizeof(PAGES) + 1);
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
            buf = getItemOfObj( pdf_p->fm_p, pdf_p->xref_p, obj, "Length" );    // <</Filter/FlateDecode/Length 1117>>stream
            if ( !buf )     
                return NULL;

            //cutTail( buf );                         // ȥ�� 1117>> �����>>
            tmpbuf = strsplit1( buf, ' ', 2 );
            len = atoi( tmpbuf );     //
            free( tmpbuf );
            
            free( buf );
            buf = getObjContent( pdf_p->fm_p, pdf_p->xref_p, obj ); 
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

    //����Ϊ�ڲ����õ�API
    //**************************************************************************
    //  �������� : getStartXrefPos()
    //  ��    �� :
    //		��ȡxref λ����Ϣ
    //  ��ڲ��� :
    //       FILEMAP    * fm_p
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         long         <0  ��ʾ����, -1 ��ʾ�ļ�ӳ��ָ�벻����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
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
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    long getStartXrefPos( FILEMAP * fm_p )
    {
        uchar     * buf = NULL;
        long        startxrefpos = -3;
         
        if ( !fm_p )       // ����ļ�ӳ��ָ��Ϊ���򷵻�-1
            return -1;
         
        // �ȶ�λ�������ļ�β48���ֽڴ�, һ��xref���ļ�β
        fm_seek( fm_p, fm_p->filelen - 48 );   

        while ( 1 ) {
            buf = fm_readLine( fm_p );
            if ( !buf )        
                return -2;             // ��ȡ����xref,�ļ���ʽ���ܲ���ȷ

            if ( strstr( buf, "startxref" ) ) {  // ���û�оͻ᷵�ؿ�ָ��, �����Ļ��ͻ᷵��λ��
                free( buf );       // �ͷ�buf�ڴ棬�ظ�����
                buf = fm_readLine( fm_p);   // "startxref" �����е���һ�о���ʵ�ʵ�xref���ڵ�λ����Ϣ

                if ( !buf )             // ���buf ΪNULL, ���ļ���ʽ����ȷ
                    return -2;
                
                if ( !IsNumberStr( buf ) ) {     // ������������ַ���, ����Ȼ���ļ���ʽ����ȷ, ע��, ���Ҫ�ͷ��ڴ�
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
    //  �������� : getTrailer()
    //  ��    �� :
    //		��ȡ Trailer ��Ϣ
    //  ��ڲ��� :
    //       FILEMAP    * fm_p
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         long         <0  ��ʾ����, -1 ��ʾ�ļ�ӳ��ָ�벻����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		xref ������������   xref + trailer.  ����ж��xref, ��ô���ж��trailer.	             
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
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    int getTrailer( char * buf, TRAILER * trailer_p )
    {
        char        *   item;
        int             len;

        if ( !buf )             // �ݴ���, ��ָ�벻����
            return -1;

        trailer_p->retcode = -1;        // ��ʼ��Ϊ-1, ���費�ܻ�ȡtrailer �ɹ�

        item = (char *)strtok( buf , "/ " );     //  ��һ�������"trailer<<"��"<<", ����
        int     i = 0;
        while( item != NULL ) {
            printf("\n��%d����Ŀ:%s\n", i+1, item );

            if ( memcmp( item, "Size", 4 ) == 0 ) {  // 1. Size ��Ŀ
                item = (char *)strtok( NULL, "/ " );          
                printf("Size ��ֵΪ:%s\n", item );
                trailer_p->Size = atoi( item);
            }

            if ( memcmp( item, "Root", 4 ) == 0 ) {  // 2. Root ��Ŀ
                item = (char *)strtok( NULL, "/ " );          
                printf("Root ��ֵΪ:%s\n", item );
                trailer_p->Root = atoi( item);

                item = (char *)strtok( NULL, "/ " );    // ���������" 1 R"
                item = (char *)strtok( NULL, "/ " );    // 
            }

            if ( memcmp( item, "Info", 4 ) == 0 ) {  // 3. Info ��Ŀ
                item = (char *)strtok( NULL, "/ " );          
                printf("Info ��ֵΪ:%s\n", item );
                trailer_p->Info = atoi( item);
            }

            if ( memcmp( item, "ID", 2 ) == 0 ) {  // 4. ID ��Ŀ ID [<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]>>
                item = (char *)strtok( NULL, "/ " );          
                printf("ID ��ֵΪ:%s\n", item );

                len = 0;
                while ( len < strlen( item ) && !( item[len] == '>' && item[len+1] == '>' ) ) //  ����β����">>" 
                    len ++;
                
                memset( trailer_p->ID, 0, L_ID + 1 );     
                memcpy( trailer_p->ID, item, len );       // [<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]
            }

            if ( memcmp( item, "Prev", 4 ) == 0 ) {  // 5. Prev ��Ŀ
                item = (char *)strtok( NULL, "/ " );          
                printf("Prev ��ֵΪ:%s\n", item );
                trailer_p->Prev = atoi( item);
            }

            if ( memcmp( item, "xrefStm", 4 ) == 0 ) {  // 6. xrefStm ��Ŀ
                item = (char *)strtok( NULL, "/ " );          
                printf("xrefStm ��ֵΪ:%s\n", item );
                item[ strlen(item) -2 ] = 0;            // ���˵�β����">>"
                trailer_p->xrefStm = atoi( item);
            }

            item = (char *)strtok( NULL, "/ ");
            i ++;
        } 

        trailer_p->retcode = i;         // ��õ���Ч��Ŀ������
        return 0;
    }

    // �����ú���
    void print_trailer( TRAILER * trailer_p )
    {
        if ( !trailer_p ) {
            printf( "\n������trailer_p �ǿ�ָ�룡\n" );
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
    //  �������� : getXREF()
    //  ��    �� :
    //		��ȡ XREF ��Ϣ
    //  ��ڲ��� :
    //       FILEMAP    * fm_p
    //  ���ڲ��� :
    //       
    //  �� �� ֵ :
    //         long         <0  ��ʾ����, -1 ��ʾ�ļ�ӳ��ָ�벻����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		xref ������������   xref + trailer.  ����ж��xref, ��ô���ж��trailer.	 
    //		�������ʱǶ�׺���, ��Ϊ�п����ж��xref, ֻ���ҵ���Ч���Ǹ�xref����.           
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    //  ifXrefPosProcessed()  ���ڲ�����, �����ж�xrefpos��ַ��Ӧ��xref �Ƿ��Ѿ��������, ��ֹѭ������ͬһ��xref�����ѭ��
    bool    ifXrefPosProcessed( long xrefpos, POSMAP * posmap_p )
    {   
        int         i = 0 ;
        
        if ( posmap_p == NULL )       // ����λ����Ϣ����������
            return false;
        
        while ( i < posmap_p->cursor ) {    // 
            if ( xrefpos == posmap_p->xrefpos[i] )
                return true;
            i ++;
        }

        // �������������Ҳû���ҵ�, ˵���õ�ַû����������, ����Ӧ��xrefû�д����
        return false;

    }

    // XREF ��������һ�еĴ���, �ж��Ƿ��Ѿ�������õ�ַָ���xref(�ݴ���, ��ֹ��ѭ��)
    int procFirstLine( FILEMAP *fm_p, long xrefpos, XREF * xref_p, POSMAP * posmap_p )
    {
        char        * buf;
        
        xref_p->retcode = -1;

        if( ifXrefPosProcessed( xrefpos, posmap_p ) )   // ����õ�ַ�Ѿ������,  ��ʾ�ļ���ʽ������, ֱ�ӷ���xref_p (retcode=-1)
            return -1;
        
        // �õ�ַû�д����, �Ѹõ�ַ��ӵ�����β��
        posmap_p->xrefpos[ posmap_p->cursor ] = xrefpos;
        posmap_p->cursor += 1;

        fm_seek( fm_p, xrefpos );

        buf = fm_readLine( fm_p );

        if ( !buf || !strstr( buf, "xref" ) ) {     // ��һ��û��"xref", ��ʽ����
            xref_p->retcode = -40421;
            if ( buf )
                free( buf );
            return -1;
        }

        free( buf );          // ���� fm_readLine() �ڲ��������ڴ�, �����Ҫ���ͷ�֮ǰ������ڴ�
        
        xref_p->retcode = 0;

        return 0;
    }

    // XREF �������ڶ��еĴ���, ��ȡ ��������, ��һ��������
    // ������:  -1  ��ʾ����,  0 ��ʾ��ȡ�ɹ�
    int procSecondLine( FILEMAP * fm_p, int   * firstObj, int * objSum, TRAILER * trailer_p ) 
    {
        char    *   buf;
        char    *   item;
        int         ret;
        char    *   tmpbuf;

        buf = fm_readLine( fm_p);

        if ( !buf )         // ��ȡ���ݳ���
            return -1;

        // �������trailer, ��ʾ��Ҫ������trailerѰ����һ��xrefpos, Ȼ���ٽ���xref
        if ( strstr( buf, "trailer" ) ) {     // ����trailer 
            printf("--------- �ڶ�����trailer....\n");
            ret =  getTrailer( buf, trailer_p );
            free( buf );
            return 1;
        }

        // ��ʼ����xref�������ڶ��� 1 5267 ֮��ĸ�ʽ(��ʼ������ ��������), ����� 0 1 ��˵������Ҫ���ݺ���trailer�ĵ�ַ��������
        tmpbuf = strsplit1( buf, ' ', 2 );
        *objSum = atoi( tmpbuf );     // ��ʼ������
        free( tmpbuf);

        tmpbuf = strsplit1( buf, ' ', 1 );
        *firstObj = atoi( tmpbuf );   // ����������
        free( tmpbuf );

        free( buf );            // �ͷ��ڴ�

        printf( "\nfirstObj=%d, objSum=%d\n", *firstObj, *objSum);
        
        return 0;
    }

    // ����objSum <= 1 �����.  �ݴ���, �п�����0 0  ����  0 1. �����0  1, ��Ҫ������һ��
    void procObjSumLT1( FILEMAP *fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p, int objSum )
    {
        char    *   buf;
        int         ret;

        xref_p->retcode = -40424;       // Ԥ��һ��������
        buf = fm_readLine( fm_p );
    
        if ( objSum == 1 ) {
            free( buf );
            buf = fm_readLine( fm_p );
        }

        if ( !buf )                    // ��Ч��ʽ
            return xref_p;
        
        if ( !strstr( buf, "trailer" ) ) {   // û��"trailer"�Ļ�������Ч��ʽ
            free( buf );
            return xref_p;
        }

        if ( !strstr( buf, "Root" ) ) { // ���trailer.<<...>>��ʽ�Ļ�, trailer������ʵ��������һ��
            free( buf );
            buf = fm_readLine( fm_p );
            if ( !buf )                 // ���û����һ������, ��Ч��ʽ
                return xref_p;     
        }
        
        ret = getTrailer( buf, trailer_p );

        if ( trailer_p->retcode < 0 || trailer_p->Prev <= 0 ) { 
            // ��Ч��xref֮���trailer��Ч��û�а�����Ч��self.xref ��ַ, �������
            free( buf );
            return xref_p;
        }

        free( buf );

        xref_p->retcode = 0;

        ret = getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
    }

    // ��ȡÿ�������λ����Ϣ, 
    // ע��:
    //      �����Ŵ�1��ʼ, ��һ����: 0000000000 65535 f   ����ʵ�ʶ���, ���Ƕ�����Ϣ˵����ʾ�����65535������
    long * getOBJPOS( FILEMAP * fm_p, int objSum )
    {
        char        *   buf;
        char        *   item;
        long        *   objpos_p;

        objpos_p = ( long * )calloc( sizeof(long) * objSum, 1 );       // ָ��һ��long����, ����δobjSum

        // ֻ�� ������ < ��������  �Ž��д���
        for ( int i =0; i < objSum; i ++ ) {
            buf = fm_readLine( fm_p );      // ��ȡ��Ӧ���� 0000000016 00000 n���Ƹ�ʽ

            item = (char *)strtok( buf, " " );          // ��ʵֻҪ��ȡ��һ��Ϳ���, ��һ���ǵ�ַ�� ȫ����ȡ��Ϊ���ݴ�
            int   j = 0;
            while ( item ) {
                if ( j == 0 || j == 1 ) {           // ��һ,����Ӧ��������, �ݴ���
                    if ( !IsNumberStr( item ) )  {
                        free( buf );
                        return NULL;
                    }
                }
            
                if ( j == 0 )
                    objpos_p[i] = atoi( item );              // �������ݵ�λ��, i Ϊ ������-1, ��Ϊi ��0��ʼ, �����Ŵ�1��ʼ
            
                item = (char *)strtok( NULL, " " );
                j ++;
            }

            if (  j > 3 )   {   // �������3� ��ʾ��ʽ����ȷ
                free( buf );
                return NULL;
            }

            free( buf );            // �ͷ��ڴ�, ���滹��Ҫ��ʹ��bufָ��
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

        if ( !strstr( buf, "Root" ) ) { // ���trailer.<<...>>��ʽ�Ļ�, trailer������ʵ��������һ��
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
        int         firstObj;       // xref �е�һ�м�¼�ĵ�һ��������
        int         objSum;         //                    ����������
        int         ret;
                
        if ( fm_p == NULL )
            return -1;

        // 1. ��һ�еĴ���. �ж��Ƿ��Ѿ�������õ�ַָ���xref(�ݴ���, ��ֹ��ѭ��), �ݴ��Ƿ���"xref"
        ret = procFirstLine( fm_p, xrefpos, xref_p, posmap_p );
        if ( ret != 0 ) 
            return -1;

        // 2. �ڶ��еĴ���.  ��ȡ ��������, ��һ��������
        ret = procSecondLine( fm_p, &firstObj, &objSum, trailer_p );       // Ϊ�����ӿɶ���, ����һ�еĴ����װ��һ��������
        if ( ret < 0 )                     // ������������0 ��ʾ������,ֱ�ӷ���xref_p,  xref_p->retcode =-1(��ʼ��deֵ), Ҳ���Ǳ�ʾ������
            return -1;
        if ( ret == 1 ) {               // �������1, ��ʾ�ҵ�trailer��, ˵����Ҫ��������һ��xref, Ƕ�׵���getXREF(), ����Ĵ�������
            if ( !trailer_p || trailer_p->retcode < 0 ) {            // ���trailer������, �����
                return -1;
            }
            return getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
        }

        // 3. objSum < 1 ������ݴ��� ��������п���, doc�ļ�ת��pdf��, ��һ��xref ����0 0, 
        // �����trailerָ���µ�xref, ���Ӧ�ü�������trailer�� ���û��Prev �ʹ���      
        if ( objSum <= 1 ) {
            printf( "objSum<=1 , objSum=%d\n", objSum );
            procObjSumLT1( fm_p, xref_p, posmap_p, trailer_p, objSum );
            return 0;
        }

        xref_p->objTotal = objSum;
        // 4. �����xref����������ÿ�������λ����Ϣ, �����Ŵ�1��ʼ
        xref_p->objpos_p = getOBJPOS( fm_p, objSum );
        if ( xref_p->objpos_p == NULL ) {
            printf("λ����Ϣû���أ�\n");
            xref_p->retcode = -40425;       // ����λ����Ϣ ��ʽ����
            return -1;
        }

        /*
        for ( int i=0; i < xref_p->objTotal; i ++ ) {
            printf("xref_p->objpos_p[%d]=%016d\n",i, xref_p->objpos_p[i] );
        }
        */
        
        // 5. ��ȡ�� ����λ�����ݺ�, ����trailer ��������
        ret = procTrailer( fm_p, xref_p, posmap_p, trailer_p );

        //print_trailer(trailer_p);

        if ( !trailer_p )
            printf("û����Ч����trailer\n");

        return 0;
        
    }

    // ------------��ȡ���е�ҳ ��Ӧ�Ķ������Ϣ
    // ��ȡָ�������е�ָ��item������
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
            if ( strstr( tmpbuf, item ) ) {     // �ҵ��˰���item ����Ŀ 
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

    /* ���ݶ����Ż�ȡ���������, ʹ���ڴ�ӳ���ļ� 
     * ��ڲ���:
     *      obj     �������ַ��� �� ��Ӧ���Ƕ�����
     *      self.xref    self.xref��ϣ�� ��ŵ��� ������:��������λ��
     * ����:
     *      ����������������ݲ���.
     * ˵��:
     *      ������������֣� ��һ���� 3 0 R , Ҳ���Ƕ����ŵ���Ϣ, ֱ�ӹ��˼���. 
     *      �ڶ��в��Ƕ�����������ı�����.
     * 2016.12.19:
     *      ����, ������ \xae ���Ƶ�����, ��Ҫ���⴦��.
     * ���õ��ⲿ������:
     *      1. seek() ------------------>util_fileService
     *      2. readLine() -------------->util_fileService
     * ���õ�������ڲ��ķ�����:
     *      1. getObjPosInself.xref()
     */
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo )
    {
        char    * buf;

        if ( !fm_p || !xref_p || objNo <= 0 )       // ��Ч����
            return NULL;

        fm_seek( fm_p, xref_p->objpos_p[objNo] );       //���������±��Ǵ�0��ʼ�� ����1��Ӧ���±���0
        
        buf = fm_readLine( fm_p );      // ��һ����  3 0 R  Ҳ���Ƕ�����
        if ( !buf ) 
            return NULL;

        free( buf );
        
        buf = fm_readLine( fm_p );      // �ڶ��в��Ƕ����Ӧ������
        if ( !buf )
            return NULL;
       
        return buf;
    }
    // ������ҳ�������Ϣ�Ĵ���i

    void printPages( PAGES * pages_p )
    {
        if ( pages_p == NULL ) {
            printf("\npages_p �ǿ�ָ�룡\n");
            return ;
        }

        printf( "pages_p->total = %d\n", pages_p->total );

        for ( int i = 0; i < pages_p->total; i ++ )
            printf("pages_p->leafs_p[%d]=%d\n", i, pages_p->leafs_p[i] ); 
    }
    //

    // getPagesObjList() ����  getPages() ��ʹ��, ��Ϊ�˼򻯴��볤�ȣ� �����Ķ����롣
    // ���� "Kids[3 0 R 12 0 R]" �ַ��� , ����ǰ�Ĵ����Ѿ�ȷ�����ַ�������ȷ��, ���Բ������ݴ���
    // ע��Ҳ������"Kids [6 0 R ....]"
    int getObjList( char * item, int count, int *objlist )
    {
        int         i, j, k;        // i  ������ǵ�ǰ�±�, j ��������ո���,  k ������Ƕ�����֮ǰλ�õ��±�
        int         n=0;              // �������pageslist�±꣬ ���ܳ���count
        int         len;
        char        buf[16];            // ������ȡ�������ַ���

        len = strlen( item );
        
        j = 0;      // Kids[3 0 R 12 0 R], �����ڱ����ַ����Ǽ����������, j ��������ո���, 3�ı�������һ������
        n = 0;
        for ( i = 0; i < len && n < count ; i ++ ) {
            if ( item[i] == '[' ) {         //  ����(�ո���+1)/3��������ҳ�����Ӷ�����ռ�, ���ж�'R'������Щ�ļ����ֶλ�仯
                k = i;
                while( item[i] != ' ' && item[i] != ']' && i < len && n < count )    // '['����ĵ�һ�������ǵ�һ��ҳ�������, i < len ���ݴ�, ��ֹ����(�����ʽ����ȷ)
                    i ++;
                if ( i>= len || n >= count ) {
                    printf("����ˣ���\n");
                    return -1;
                }

                memset( buf, 0, 16 );
                memcpy( buf, &item[k+1], i-(k+1) );
                objlist[n] = atoi(buf);

                n ++;
            }
            if ( item[i] == ' ' )  {         // ֻҪ�����ո�, ��j ++ ,   "Kids[3 0 R 12 0 R]" 
                if ( n > 0 )                // ��ֹ "Kids [" ����������еĿո񲻸ü���
                    j ++;
                if ( j == 3 ) {          // ���j=3��k �ͼ���i ��ֵ, ��Ϊ��һ�����Ƕ�����
                    k = i;
                    i ++;               // ��������ո�  
                    while( item[i] != ' ' && item[i] != ']'  && i < len && n < count )    // ���崦��ͬ��
                        i ++;

                    if ( i>=len || n >= count) 
                        return -1;

                    j = 1;   // ����������һ��������֮ǰ�Ŀո���, ��Ϊ����ո�ҲҪ����
                    memset( buf, 0, 16 );
                    memcpy( buf, &item[k+1], i-(k+1) );
                    objlist[n] = atoi(buf);

                    n ++;
                }
            }

        }

        return 0;
    }

    // ��ȡ��ֺ�ĵڼ�����, ����ʹ��strtok�� ��Ϊ�ⲿ�п��ܻ�ʹ��
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

    // getPageleaf() ��������ȡ���е�Ҷ��ҳ�����, Ҳ���Ǵ���Content�Ķ�����Ϣ, �ö���Ҳ����ҳ�����ݶ���
    // ÿ��ҳ�����ʵ����Ҳ���Ƕ�Ӧһ��ҳ�������, leafs �Ǹ�long ������
    // cursor ���������leafs_p��λ�Ƶ�
    int getPageleaf( FILEMAP * fm_p, XREF * xref_p, int obj, int *leafs_p, int *cursor )
    {
        char        * buf;
        char        * item;
        int         kidscount;
        int         * objlist;
        int         ret;


        // # 5251||<</Count 100/Kids[5252 0 R 5253 0 R 5254 0 R 5255 0 R 5256 0 R 5257 0 R 5258 0 R 5259 0 R 5260 0 R 5261 0 R]/Parent 5250 0 R/Type/Pages>>
        buf = getObjContent( fm_p, xref_p, obj );
        
        if ( strstr( buf, "Kids" ) ) {      // �����"Kids", ˵������Ҷ��ҳ��, ��Ҫ����Ƕ�ױ���kidsҳ��, ֱ���ҵ����յ�����ҳ��
            item = (char *) strtok( buf, "/" );
            while( item ) {
                if ( strstr( item, "Count" ) ) {    // kids ����  //Count 100
                    kidscount = atoi( strsplit( item, " ", 2 ) );
                }
                if ( strstr( item, "Kids" ) ) {
                    objlist = (int *)malloc( sizeof(int) * kidscount + 1 );
                    memset( objlist, 0, sizeof(int)*kidscount + 1 );
                    ret = getObjList( item, kidscount, objlist );
                }
                item = (char *)strtok( NULL, "/" );
            }
            // �������ҳ�����, �ҵ�Ҷ�Ӷ���, ����䵽leafs_p
            for ( int i = 0; i < kidscount; i ++ ) {
                for ( int j=0; j < *cursor; j ++ ) {
                    if ( leafs_p[j] == objlist[i] ) {   // �ݴ�.����ö����Ѿ������� leafs_p, ��ʽ�����⣬һ��Ҷ��ҳ����󲻿��ܷ����ڶ��ҳ��
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
        } else if( strstr( buf, "Contents" ) ){ //ֻ�к��� Contents �Ķ������������Ҷ�Ӷ���
            //printf("Ҷ�Ӷ���:%s\n", buf);
            
            for ( int i=0; i < *cursor; i ++ ) {
                if ( leafs_p[i] == obj ) {   // �ݴ�.����ö����Ѿ������� leafs_p, ��ʽ�����⣬һ��Ҷ��ҳ����󲻿��ܷ����ڶ��ҳ��    
                    if ( objlist )
                        free( objlist );
                    free(buf);
                    return -1;
                }
            } 
            leafs_p[*cursor] = obj;
            //printf("�ҵ�һ��Ҷ�Ӷ���%d(��%dҳ)\n", obj, *cursor+1 );
            *cursor += 1;
        }

        free( buf );
        if ( objlist )
            free( objlist );
        return ret;
    }
    
    /*Root �����������������:  (ע�⣬ ���������ʵ������û�л��еģ� ���Ϊ�˷����Ķ�������)
     *       1 0 obj

        <</AcroForm 51135 0 R/Lang(zh-CN)/MarkInfo<</Marked true>>
        /Metadata 51137 0 R/OCProperties<</D<</AS[<</Category[/View]/Event/View/OCGs[50601 0 R]>>
        <</Category[/Print]/Event/Print/OCGs[50601 0 R]>>
        <</Category[/Export]/Event/Export/OCGs[50601 0 R]>>]
        /ON[50601 0 R]/Order[]/RBGroups[]>>/OCGs[50601 0 R]>>
        /Outlines 607 0 R/Pages 2 0 R/StructTreeRoot 1066 0 R/Type/Catalog>>
     *          
     *          ͨ�������Root��Ϣ, ���Ի��pages ������Ϣ�� Ȼ��ͨ������������ڻ��
     *          ���е�ҳ���Ӧ��ԭʼ����ı��(�ö���δ�������ݶ��󣬼�����Ҷ�Ӷ���)
     *          (�����������Kids�е�����ʡ���˺ܶ࣬ ��Ȼ̫��)
     *          
     *          Pages ������Kids ��, ��������ľ���ҳ����� ��ҳ������п��ܻ��Ậ��Kids��.  
     *          ֻ�к���content�Ķ������Ҷ�Ӷ��� ����Ӧ�ú���Kids� 
     *          
            2 0 obj
            <</Count 266/Kids[3 0 R������298 0 R 299 0 R]/Type/Pages>>
     *
     *          ����: ����Content��Ҷ�Ӷ��� �����ж��stream ���� ��һҳ��Ӧ������ݶ�������:
     *          5272||<</Contents[5278 0 R 5279 0 R 5280 0 R 5281 0 R 5282 0 R 5283 0 R 5291 0 R 5292 0 R]/CropBox[9 0 603 792]/MediaBox[0 0 612 792]/Parent 5252 0 R/Resources 5273 0 R/Rotate 0/Type/Page>>
     *
     *              ���ص���Pages ������
     *          �μ�:
     *              page��Ϣ.txt    --- һ��Ӣ��PDF�ļ���Root, Pages, Kids, Content �������Ϣ���С�
                ע��: 
                    �����ȫӢ��pdf �ļ��� ����û��type0 ����, Ҳ����û��cmap �����б�
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
        item = (char *)strtok( buf, " " );   // ���� Pages 2 0 R
        while ( item != NULL ) {
            if ( i == 2 ) {                      // ҳ������Ӧ���ǵڶ�������2
                pagesobj = atoi( item );
                break;
            }

            item = (char *) strtok( NULL, " " );
            i ++;
        }
        free( buf );
        // ���i > 2 ˵��û���ҵ�Pages������, Ҳ���ǳ�����
        if ( i > 2 ) 
            return -1;

        // �����ȡ��������ҳ��Ķ�����Ϣ 
        printf("pages obj = %d\n", pagesobj);

        // ��ȡPages  �����е�ҳ����������(��Ҫ��ҳ������, ҳ�����ݶ���ͨ��Ƕ�׺���getPageleaf()��ȡ )
        buf = getObjContent( fm_p, xref_p, pagesobj );      
        printf( "Pages content =%s\n", buf );
        item = (char *)strtok( buf, "/" );
        while ( item != NULL ) {
            if ( strstr( item, "Count" ) ) {     // �ҵ��˰���Count ����Ŀ Count 2
                pages_p->total = atoi( item + 6 );    // +6��Ϊ������ "Count "
            }
            if ( strstr( item, "Kids" ) ) {     // Kids[3 0 R 12 0 R]  ��������е�ҳ�����
                pages_p->leafs_p = (int *)malloc( sizeof(int)* pages_p->total + 1);
                memset( pages_p->leafs_p, 0, sizeof(int)* pages_p->total + 1 );
                
                ret = getObjList( item, pages_p->total, pages_p->leafs_p );   // ע�� ���pages_p->leafs_p ���������յ�ҳ��Ҷ�Ӷ���
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


    // ����ĺ���Ϊ���Ժ���, �����������Ϣ������ļ���, ���ڲ���
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


    // ----------- ������ CMAP  ��ش���
    //
    // writeRawData()
    //     ���Ժ���, ��stream�е��ֽ���д�뵽�ļ���, ���ڲ鿴
    void writeRawData( char * filename, uchar *bytes, int len )
    {
        FILE        *   fp;
        size_t          l;

        if ( !( fp=fopen( filename, "wb") )) 
            return;

        l = fwrite( bytes, len, 1, fp );
        printf("\nд�ɹ�����:l=%d, Ӧ��д�� len=%d\n", l, len);
        fclose( fp );
        
        return ;
    }

    // ����ָ���ַ����ַ����г��ֵĴ���
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

    // �жϸ����Ķ����Ƿ���type0 ����, ���Ƿ���CMAP
    int isType0( FILEMAP *fm_p, XREF * xref_p, int obj ) 
    {
        char    *buf;

        buf = (char *)getObjContent( fm_p, xref_p, obj );

        if ( !buf )
            return 0;

        if ( strstr( buf, "Type0" ) ) {     // �����Type0 ��, �������CMAP ����
            free( buf );
            return 1;
        } else {
            free( buf );
            return 1;
        }
    }


    // <</Contents[5278 0 R 5279 0 R 5280 0]/CropBox[9 0 603 792]/MediaBox[0 0 612 792]/Parent 5252 0 R/Resources 5273 0 R/Rotate 0/Type/Page>>
    // ��ȡҶ�Ӷ��� ��Ӧ�����ݶ����б�  pages_p->c_objlist_p;
    //  pages_p->c_objlist_p[i] ��Ӧ�ľ��ǵ�iҳ�����ݶ����б�ָ��
    int getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf;
        int         * leafs_p = pages_p->leafs_p;       // �����Ϊ�˼򻯴��볤��
        int         count;
        
        pages_p->c_objlist_p = (int **)malloc(sizeof(int *)*pages_p->total + 1 );
        memset( pages_p->c_objlist_p, 0, sizeof(int *)*pages_p->total + 1  );

        pages_p->comprRatio_p = (float **)malloc(sizeof(float *)*pages_p->total + 1 );
        memset( pages_p->comprRatio_p, 0, sizeof(float *)*pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Contents" );
            //printf( "Ҷ�Ӷ���(��%dҳ):%d| ����:%s\n", i+1, leafs_p[i], buf );

            if ( strchr(buf, '[' ) ) { // �����������, ��ʾ����������ݶ���,  Contents[5278 0 R 5279 0 R]
                count = countChrInStr( rtrim(buf), ' ' );  // �ȼ����ַ����г��ֵĿո���
                count = (count+1)/3;                // �������������, +1 ����Ϊ��һ������ֻ��2���ո�

                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset(  pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = count;            // ��һ��Ԫ��(�±�Ϊ0) ��Ҷ�����ݶ�������
                ret = getObjList( buf, count, &(pages_p->c_objlist_p[i][1]) );
                
                pages_p->comprRatio_p[i] = (float *)malloc( sizeof(float ) * (count + 1) );  // ��������ѹ�������ݲ���
                memset(  pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

                pages_p->comprRatio_p[i][0] = count;            // ��һ��Ԫ��(�±�Ϊ0) ��Ҷ�����ݶ�������
            }
            else {
                count =1;                   // û��������, ��1�����ݶ��� Contents 7 0 R
                //printf("��һ�����ݶ���(buf=%s)\n", buf);
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset( pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = 1;                                     // ���ֵ�1��(�±�Ϊ0)����Ҷ�����ݶ�������
                pages_p->c_objlist_p[i][1] = atoi( strsplit( buf, " ", 2 ) );       // buf �ڶ���������ݶ���

                pages_p->comprRatio_p[i] = (float *)malloc( sizeof(float ) * (count + 1) );
                memset( pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

                pages_p->comprRatio_p[i][0] = 1;                                     // ���ֵ�1��(�±�Ϊ0)����Ҷ�����ݶ�������
            }

            /* 
            printf("\nobj %d �����ݶ���Ϊ:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", pages_p->c_objlist_p[i][j]);
            printf("\n");
            */
            free( buf );
        }
        return 0;
    }

    // ��ȡ��������, ���������Ϊ�˼򻯴��볤��, ��ߴ���ɶ��԰��������һ������
    int getFontCount( FILEMAP *fm_p, XREF * xref_p, int obj )
    {
        int     count = 0;
        char    * buf, * item;
        bool    flag = false;
            
        buf = (char *)getObjContent( fm_p, xref_p, obj );    // /Font <</FT15 15 0 R /FT20 20 0 R /FT8 8 0 R>>
        printf( "Ҷ�Ӷ���%d| font:%s\n",  obj, buf );
            
        item = (char *)strtok( buf, "/" );
        while( item && !flag ){     // flag = true ��ʾ�����Ѿ�������ϣ� �˳�ѭ��
            if ( ! strstr( item, "Font" ) ) { // û�ҵ�Font �ͼ�������һ��
                item = (char *)strtok( NULL, "/" );
                continue;
            }
            // ����ҵ���"Font", ����ѭ�������ȡ��������
            while ( item ) {
                item = (char *)strtok( NULL, "/" );
                count ++;
                if ( strstr( item, ">>") ) {        // �������">>"��ʾ�������һ��������
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
        int     k =0;       // ��¼ÿһ��Ŀ�ʼλ��

        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // �ҵ��ڼ������λ����
                    retbuf = (char *)malloc( i-k+1);
                    memset( retbuf, 0, i-k+1);   
                    memcpy( retbuf, buf+k, i-k );
                    return retbuf;
                }
                while( buf[i] == ch )       // ���������ķָ���
                    i ++;
                k = i;  // ��¼Ѱ����֮ǰ������ķָ���λ��
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // ���һ��
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
        int     k =0;       // ��¼ÿһ��Ŀ�ʼλ��

        if ( !des )
            return NULL;
        
        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // �ҵ��ڼ������λ����
                    memset( des, 0, len);   
                    memcpy( des, buf+k, i-k );
                    printf( "des=%s\n", des );
                    return des;
                }
                while( buf[i] == ch )       // ���������ķָ���
                    i ++;
                k = i;  // ��¼Ѱ����֮ǰ������ķָ���λ��
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // ���һ��
            memset( des, 0, len );   
            memcpy( des, buf+k, i-k );
            return des;
        }
        return NULL;
    }

    // getType0Font()  �м亯��, �򻯺�������, ���ӿɶ���
    // type0list  ���ݸ�ʽΪ: " FT8 8 FT15 15 FT20 20 FT62 62 FT87 87"
    int getType0Font( PAGES * pages_p, int type0count, char * type0list )
    {  
        int     n = 0;  // �ո�����, 1��ʾ��һ���ո�, type0list �а���type0 �������Ͷ�Ӧ�Ķ�����, �ÿո�ָ��� " FT8 8 FT15 15 FT20 20 FT62 62 FT87 87"
        int     k = 0;  // ��ǿո��λ��, ������ȡtype0�������ƺͶ�����
        // ����һ��type0list  ��ȡtype0���������������
        int     len = strlen( type0list );
        char    ttt[8];     // ��ʱ������������, 

        printf( "type0 count=%d, type0 list:{%s}\n", type0count, type0list );
        pages_p->cmaptotal = type0count;
        pages_p->cmaps_p = (CMAP *)malloc( sizeof(CMAP) * type0count + 1);
        memset( pages_p->cmaps_p, 0, sizeof(CMAP)*type0count + 1 );

        for ( int i = 0; i < len && n/2 <= type0count; i ++ ) {
            if ( type0list[i] == ' ' ) {     // �ҵ��ո�
                k = i+1;                  // ��¼�ո���λ��, �ҵ���һ���ո���2���ո�֮�����Ҫ�ҵ���Ŀ
                n ++;
                i ++;       // �����ո�
                while( type0list[i] != ' ' && i < len )    // ����һ���ո� 
                    i ++;
                if ( n%2 == 1 ) {     // ��� n % 2 = 1, ��ʾ����������, �� �����±�Ϊ: n/2+n%2-1, ��: 1/2+1%2-1 = 0(��Ϊ n/2), ��Ϊ�±��Ǵ�0��ʼ��
                    memcpy( pages_p->cmaps_p[ n/2 ].fontname, &(type0list[k]), i-k ); 
                    printf("%d: n=%d, ��������:%s\n", n/2, n, pages_p->cmaps_p[n/2].fontname );
                } else if ( n%2 == 0 ) {    // ��� n%2 = 0, ��ʾ�����������, ���Ӧ���±��� n/2+n%2-1, ����: 2/2+2%2-1=0(��Ϊ n/2-1)
                    memset( ttt, 0, 8 );
                    memcpy( ttt, type0list+k , i-k );
                    pages_p->cmaps_p[ n/2-1 ].obj = atoi( ttt );
                    
                    printf("%d: n=%d ���������:%d\n", n/2-1, n, pages_p->cmaps_p[n/2-1].obj );
                } 
                    
                i --;       // i ���˵��ո�λ��, ����ѭ��������������
            }
        }

        return 0;

    }


    // ��ȡҶ�Ӷ��� ��Ӧ����������б�  pages_p->fontmap_p;
    //  pages_p->fontmap_p ��2��ָ��, ��һ�������Ƕ��ٸ�Ҷ��ҳ��, �ڶ�����ҳ�����������
    int getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf, * item;
        int         * leafs_p = pages_p->leafs_p;       // �����Ϊ�˼򻯴��볤��
        int         count;
        bool        flag;
        char        * tmpbuf;
        int         type0count = 0;                     // ��������type0 �������(�ǳ�ȥ�ظ���֮�������), ��Ϊ�˺���Ϊtype0��������洢�ռ��
        char        type0list[2048];                    // �������type0 �������� "F34 F20 T2"��, ȥ�غ�Ĵ洢�����
        
        pages_p->fontmap_p = (FONTMAP **)malloc(sizeof( FONTMAP * ) * pages_p->total + 1 );
        memset( pages_p->fontmap_p, 0, sizeof( FONTMAP * ) * pages_p->total + 1  );

        memset( type0list, 0, 2048 );       // ��ʼ��

        for ( int i=0; i < pages_p->total; i ++ ) {
            count = getFontCount( fm_p, xref_p, leafs_p[i] );       // ��ȡ��������

            pages_p->fontmap_p[i] = (FONTMAP *) malloc( sizeof( FONTMAP ) * count  + 1 );  // FONTMAP ����
            memset( pages_p->fontmap_p[i], 0, sizeof( FONTMAP ) * count  + 1 );
            
            buf = (char *)getObjContent( fm_p, xref_p, leafs_p[i] );
            
            //printf( "\n��ȡFont ����:%s\n", buf);
            item = (char *)strtok( buf, "/" );
            flag = false;
            while( item && !flag ){     // flag = true ��ʾ�����Ѿ�������ϣ� �˳�ѭ��
                //printf( "item = %s\n", item );
                if ( ! strstr( item, "Font" ) ) { // û�ҵ�Font �ͼ�������һ��
                    item = (char *)strtok( NULL, "/" );
                    continue;
                }
                
                //     ����ҵ���"Font", ����ѭ�������ȡ��������
                int     n = 0;
                while ( item ) {
                    item = (char *)strtok( NULL, "/" );
                    //printf("Font ��Ϣ: %s\n", item );           // Font ��Ϣ: FT15 15 0 R
                    
                    tmpbuf = strsplit1( item, ' ', 1 );
                    memcpy( pages_p->fontmap_p[i][n].fontname, tmpbuf, strlen(tmpbuf) );
                    free( tmpbuf );
                    
                    tmpbuf = strsplit1( item, ' ', 2 );
                    pages_p->fontmap_p[i][n].obj = atoi(tmpbuf);
                    free( tmpbuf );
                    
                    // �ж��Ƿ���type0
                    if ( isType0( fm_p, xref_p, pages_p->fontmap_p[i][n].obj ) )  {
                        pages_p->fontmap_p[i][n].isType0 = 1;
                        if( !strstr( type0list, pages_p->fontmap_p[i][n].fontname ) ) {  // �����type0����û����type0list��, ˵�����µ�type0����, ��ӽ�type0list, type0count ++
                            sprintf( type0list, "%s %s %d", type0list, pages_p->fontmap_p[i][n].fontname, pages_p->fontmap_p[i][n].obj );
                            type0count ++;
                        }
                    } else
                        pages_p->fontmap_p[i][n].isType0 = 0;

                    n ++;
                    if ( strstr( item, ">>") ) {        // �������">>"��ʾ�������һ��������
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

    // �ֲ�����, ���� getCMAP() ��ʹ��, �����ַ����� ȥ��β���ķ����ֲ���, һ����>>
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
    
    // ��ȡCMAP�еı���ӳ��������, ���غ���, ����procType0Stream() �е���, �򻯴�������, ���ӿɶ���
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
            if ( strstr( buf, "endcmap" ) ) {// �����˳�ѭ������, ��ֹ��Щ������������������β��
                free( buf );
                break;
            }

            /*
                # 2 beginbfrange
                #    <00B2> <00B2> [<2014>]
                #    <00B3> <00B4> <201C>
                #    endbfrange
                #    ���ָ�ʽ����Ҫ���⴦�� ��������������
                # ���и�begincodespacerange  .... ����������޶����뷶Χ��
            */

            if ( strstr( buf, "beginbfchar" ) ) {   // ��������ӳ��  100 beginbfchar
                tmpbuf =  strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                
                total += mapsum;

                // <1440> <5E10> , ������6+1+6+1 = 14���ֽ�  0A ��β��
                mm_seek( mm_p, mm_p->pos + 14 * mapsum );       // ���� mapsum ���ַ�, Ҳ���Ǳ���ӳ�䲿��
                //free( buf );    // ǰ��free()����
                buf = mm_readLine( mm_p );                      // ����  endbfchar
                free( buf );
                continue;
            }
            
            if ( strstr( buf, "beginbfrange" ) ) {   // ��Χ����ӳ��  2 beginbfrange
                tmpbuf =  strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // ���� mapsum �м���
                    buf = mm_readLine( mm_p );
                    free( buf );
                    buf = NULL;
                }
                continue;
            }
            
            //if ( buf )
            free( buf );     // ���ִ�е����, �϶���Ҫfree()
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


    // ��ʱ����, ����getCMAP() ����, ���� type0 �ֽ���, ��ȡcmap
    // desbuf �д�ŵ����Ѿ���ѹ����ֽ���,  
    // dlen   �� desbuf �е��ֽ����ĳ���
    // ����ӳ���ƺ����Ը���keyֵ�ú������ value, ������Ҫ�������, ��ʱ�򵥴���, ���б��뵥��ƥ��, δ����̬����ӳ�亯������ߴ�������
    // ע��: ���� F052, F0FE , Ŀǰת���ɿո�(0020), ���Ժŵķ���, δ�����Ե����öԺ����
    // ����������ʽʵ��, ά���鷳, ���������ֽ���, ��һ����Ϊ�˼������ӳ������, ���������ڴ�.
    int procType0Stream( CMAP *cmap_p, uchar * desbuf, uLong dlen )
    {
        MEMORYMAP   * mm_p;
        char        * buf;
        int         mapsum;     // ӳ������, ��ʱ��¼ÿ��ӳ�䵥Ԫ�ı���ӳ������
        int         total = 0;  // ���б���ӳ������֮��
        int         n;          // cmap_p->code_p[n] �±�
        int         start1, end1, start2;       // beginbfrange �е���ʼ��������
        char        * tmpbuf;    // ������ʱ��ű��� 547F֮��� 
 
        mm_p = initMemoryMap( desbuf, dlen );

        total = getCMAPsum( mm_p );     // ��ȡCMAP�еı���ӳ��������,Ϊcmap_p->code_p ����ռ���׼��
        
        cmap_p->code_p = (CODE *)malloc( sizeof(CODE) * total + 1 );
        memset( cmap_p->code_p, 0, sizeof(CODE)*total + 1 );
        
        cmap_p->total = total; 
        // ���¶�λ���ڴ�ӳ���ͷ��
        mm_seek( mm_p, 0 );
        while ( 1 ){
            buf = mm_readLine( mm_p );

            if ( !buf )
                break;
            if ( strstr( buf, "endcmap" ) ) {// �����Ƴ�ѭ������, ��ֹ��Щ������������������β��
                free( buf );
                break;
            }
            /*
                # 2 beginbfrange
                #    <00B2> <00B2> [<2014>]
                #    <00B3> <00B4> <201C>
                #    endbfrange
                #    ���ָ�ʽ����Ҫ���⴦�� ��������������
                # ���и�begincodespacerange  .... ����������޶����뷶Χ��
            */
            if ( strstr( buf, "beginbfchar" ) ) {           // ��������ӳ��  100 beginbfchar
                tmpbuf = strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                for( int i = 0; i < mapsum; i ++ ) {
                    free( buf );
                    buf = mm_readLine( mm_p );              // <1440> <5E10> 
                    DelCharsInString( buf, "<>" );          // ɾ�� ������  "1440 5E10" 
                    
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
            }else if ( strstr( buf, "beginbfrange" ) ) {   // ��Χ����ӳ��  2 beginbfrange
                tmpbuf = strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                //printf("beginbfrange sum=%d\n", mapsum);
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // ���� mapsum �м���
                    free( buf );
                    buf = mm_readLine( mm_p );              // <01C3> <01C4> <3001>
                    DelCharsInString( buf, "<>" );          // ɾ�� ������  "01C3 01C4 3001"

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
    typedef struct __code_ {        // <0005> <0022>   int ��4�ֽ�, �ַ���Ҳ��4�ֽ�, �Ǹ��Ƚ����������ά����������������ַ�ʽ
        int         code;       // 4λ����   0005
        int         decode;     // 4λ����   0022
    } CODE;
    typedef struct __cmap__ {
        char        fontname[32];   // һ���������ֲ��ᳬ��32���ַ�, ��ʱ��32�ֽ����
        int         obj;
        int         total;          // ��������
        CODE        * code_p;       // ����, �������
    } CMAP;

    typedef struct __pages__ {
        int         total;      // ��ҳ��
        int         cursor;     // ���һ��Ҷ��ҳ�����洢��leafs�����е�λ��(�±�), �����Ϊ�˱��ڴ���
        int     *   leafs_p;        // ����, �������Ҷ��ҳ�������
        int     **  c_objlist_p;    // leafs_p�е�Ҷ�Ӷ�����������ݶ����б� c_objlist_p[total][�����б�]
        FONTMAP **  fontmap_p;      // fontname[ҳ����total][���ٸ�����][FONTMAP]
        int         cmaptotal;      // cmap ��������
        CMAP    *   cmaps_p;  // CMAP����, ���е�cmap������������, ��Ϊ��ͬ��Ҳ�Ḵ�úܶ���ͬ��cmap
    }PAGES;
     * */

    // ����Type0 ����, ��ȡ CMAP ����
    // <</Filter /FlateDecode /Length 279>>��
    int getCMAP( FILEMAP *fm_p,  XREF *xref_p, CMAP *cmap_p )
    {
        char    *   buf, * tmpbuf;
        int         len;
        uchar   *   ubuf,  * desbuf ;
        int         ret;
        
        buf = (char*)getItemOfObj( fm_p, xref_p, cmap_p->obj, "Length" );    //  Length 279>>
        if ( !buf )
            return -1;

        cutTail( buf );                         // ȥ�� 279>> �����>>
        tmpbuf = ( char *) strsplit1( buf, ' ', 2 );
        len = atoi( tmpbuf );         // ȡ279 ���֣� Ҳ�����ֽ�������
        free( tmpbuf );
        
        printf( "�ֽ�������Ϊ%d\n", len );
        // ����Ĵ���ҪС��, �������Ķ����Content�а���stream, �Ͳ�Ҫ������һ����, ���������, ����Ҫ����һ�С�����������������
        // 
        free(buf);
        buf = (char*)getObjContent( fm_p, xref_p, cmap_p->obj );

        if ( !strstr( buf, "stream" ) ) {            // ����������\r\nstream\r\n ����stream\r\n
            free( buf );
            buf = (char *)fm_readLine( fm_p );      // ����һ��
            free( buf );
        }

        ubuf = (uchar *)fm_read( fm_p, len, fm_p->pos );       // ��ȡָ�����ȵ��ֽ���

        // ����ubuf �е��ֽ�������stream�����ݣ� ������н�ѹ, ��ѹ������ݴ����desbuf, �����ļ��еĲ���Ϊ���Դ���, ���ע�ͻ�ɾ����
        char        filename[128];
        sprintf( filename, "f:/F_t_tmp/cmap/c_%d_stream.txt", cmap_p->obj );
        writeRawData( filename, ubuf, len );

        // ��ѹ
        int     err;
        uLong   dlen = len * 6;

        //desbuf = (uchar *)malloc( len * 6 );
        //memset( desbuf, 0, len * 6 );
        desbuf = (uchar *)calloc( dlen, 1 );
        err = uncompress( desbuf, &dlen, ubuf, len );
        CHECK_ERR(err, "uncompress");  

        cmap_p->comprRatio = (float)dlen/len;

        printf("cmap_p->obj=%d, ѹ����Ϊ=%2.2f", cmap_p->obj, cmap_p->comprRatio );

        // д�ļ�ʱ���Դ��룬 ����鿴�� ���տ���ɾ����ע��
        sprintf( filename, "f:/F_t_tmp/cmap/c_%d_cmap.txt", cmap_p->obj );
        writeRawData( filename, desbuf, dlen );

        printf("dlen=%d, len=%d\n", dlen, len );

        ret = procType0Stream( cmap_p, desbuf, dlen );

        free( ubuf );
        free( desbuf );

        return ret;
    }


    // ����pages_p->cmaps_p �е�type0�������, ��ȡCMAP ���������cmap_p
    //  type0 ���������и�unicode����, ����ֻ��stream��, ���ݲ���������CMAP��Ϣ
    int getCMAPS( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        char    *   buf, *tmpbuf;
        int         obj;
        
        for( int i=0; i < pages_p->cmaptotal; i ++ ) {
            buf = (char*)getItemOfObj( fm_p, xref_p, pages_p->cmaps_p[i].obj, "ToUnicode" );    //  ���û�� ToUnicode, ��ô���ǳ�����  ToUnicode 88 0 R
            if ( !buf )
                return -1;

            tmpbuf = (char *)strsplit1( buf, ' ', 2 ) ;         // ToUnicode 88 0 R �ڶ������Unicode ������
            obj = atoi( tmpbuf );
            free( tmpbuf );                                     // strsplit1() �ڲ��������ڴ�,������Ҫ�ͷ�
            free( buf );                                        // getItemOfObj() �ڲ��������ڴ�,������Ҫ�ͷ�

            pages_p->cmaps_p[i].obj = obj;                      // type0 ��������� ����Ϊ  Unicode ������
            getCMAP( fm_p, xref_p, &(pages_p->cmaps_p[i]) );
        }
            
        return 0;
    }



    //###########################################################################
    //#  ���²���Ϊ���� ��ѹ�� ҳ�����ݲ���
    /*
     * 15. decode()
     *      �Ը������ֽ������н���
     * ˼·:
     *      1. ��ÿһ���ֵĴ���ģ�黯
     *      2. ���һ��(I��)�� processBDC, processBT�� ���еĴ�������BDC/EMC, BT/ET ֮�䣬 
     *         ��2��֮��Ҳ�ụ��Ƕ��
     *      3. II�� ������: 
     *          processTD, processTd, processTm, processT*        �ı�λ��
     *          processTJ, processTj, processQuote, processDblQuote         �ı���ʾ, �������ǵ�����, ˫���ŵĴ���
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  �ı�״̬
     *          ���������
     *      2016.12.08:
     *          ��������ֽ�ŵļ������������ݱ���, �� ���������ݴ����һ������ֽ�ŵ��ڴ��У�Ȼ�����������ݼ���.
     *          virtual_Page={["Y����ֵ": ��������],}
     *          Ȼ�������ʱ��ֻҪ��virtual_Page ���չؼ������������������.
     *          ���괦��: ��ǰ������������������е�ĳ���������С��1, �����Ϊͬһ�� �� ��Ϊ��С������Ҳ�����1������
     *          {Y:["��������","����", ��λ����]},
     *               ��λ���������ֵ�����ĳ���, �����ж���һ�����Ƿ���Ҫ�������֮���зָ���
     *               �����һ�����ֵ���ʼx����������ֵ�x+len С�ڵ�λ����, ����Ҫ�ָ���.
     *      2016.12.21:
     *          ֮ǰ�Ĵ���, �����кܶ಻���Ƶĵط�, ��Ҫ��û�а���pdf �Ļ��������˼·. �����������˼·, ���� ͼ��״̬stack ʵ��.
     *            ��GraphStateStack ��¼��ǰѹջ������ ͼ��״̬, cur_graph_state ������¼��ǰ��ͼ��״̬. ���ǵ�ֻ�����ı�,
     *            ͼ��״ֻ̬��¼ tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */

    /*
     * # ������Ϣ, �����Ϣ(��ʼ��ҳü��û�е�, -1����ҳü���), �ı���Ϣ, ת������
    // ������Ϣ
    typedef struct __cur_xy__ {
        int     x;
        int     y;
    }CUR_XY;

    typedef struct __cellMap__ {
        // ��Ԫ����Ϣ
    }CELLMAP;

    typedef struct __cellIndexMap__{
        // ��Ԫ��������Ϣ
    }CELLINDEXMAP;

    typedef struct __textMap__ {
        // �ı�ӳ���
    } TEXTMAP;

    typedef struct __tm__ {
        // tm ��Ϣ
    }TM:

    typedef struct __tf__ {
        // tf ��Ϣ
    }TF;
    */

    /*
     * 15. decode()
     *      �Ը������ֽ������н���
     * ˼·:
     *      1. ��ÿһ���ֵĴ���ģ�黯
     *      2. ���һ��(I��)�� processBDC, processBT�� ���еĴ�������BDC/EMC, BT/ET ֮�䣬 
     *         ��2��֮��Ҳ�ụ��Ƕ��
     *      3. II�� ������: 
     *          processTD, processTd, processTm, processT*        �ı�λ��
     *          processTJ, processTj, processQuote, processDblQuote         �ı���ʾ, �������ǵ�����, ˫���ŵĴ���
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  �ı�״̬
     *          ���������
     *      2016.12.08:
     *          ��������ֽ�ŵļ������������ݱ���, �� ���������ݴ����һ������ֽ�ŵ��ڴ��У�Ȼ�����������ݼ���.
     *          virtual_Page={["Y����ֵ": ��������],}
     *          Ȼ�������ʱ��ֻҪ��virtual_Page ���չؼ������������������.
     *          ���괦��: ��ǰ������������������е�ĳ���������С��1, �����Ϊͬһ�� �� ��Ϊ��С������Ҳ�����1������
     *          {Y:["��������","����", ��λ����]},
     *               ��λ���������ֵ�����ĳ���, �����ж���һ�����Ƿ���Ҫ�������֮���зָ���
     *               �����һ�����ֵ���ʼx����������ֵ�x+len С�ڵ�λ����, ����Ҫ�ָ���.
     *      2016.12.21:
     *          ֮ǰ�Ĵ���, �����кܶ಻���Ƶĵط�, ��Ҫ��û�а���pdf �Ļ��������˼·. �����������˼·, ���� ͼ��״̬stack ʵ��.
     *            ��GraphStateStack ��¼��ǰѹջ������ ͼ��״̬, cur_graph_state ������¼��ǰ��ͼ��״̬. ���ǵ�ֻ�����ı�,
     *            ͼ��״ֻ̬��¼ tm. 
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

        retbuf = (char *)calloc( dlen, 1 ); // ȷ��������ڴ��㹻��
        
        decode_p = (DECODE *)calloc( sizeof( DECODE ), 1 );
        
        mm_p = initMemoryMap( buf, dlen );

        while( 1 ) {    // ע��: ������жϲ���if else, ��Ϊ�п���һ�а������ֲ�����
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf( "decode() : tmpbuf = %s\n", tmpbuf );
            
            if ( strstr( tmpbuf, "BT" ) ) {         // ����� ����"BT"/"ET"
                processBT( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            if ( strstr( tmpbuf, "BDC" ) ) { // �������"BDC"/"EMC"
                processBDC( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            
            // re �Ĵ���Ӧ����BT/ET, BDC/EDC ֮��, Ҳ���ǵ�Ԫ��.
            if ( strstr( tmpbuf, "re" )  ) {   //����"re"
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
        // ���������������������������, �����������¹���
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
     *      ���� BT/ET �ڵ�����
     * ��ڲ���:
     *      mm-p           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      pages_p        pages_p��������ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      ��
     * ˵��:
     *      ������̻���ͬBDC һ�£� �����ڲ�������BT���ж�, ���ǰ�����BDC�����Ƕ��
     *      ���������ݴ�����Ķ����������� �ļ�����
     * 2016.12.12:
     *      Tm ��������ת������ϵ. [a b c d e f], ����һ��list Tm, ������µ�ת������
     *      Td/TD ������ʵ��Ϊ
     *           x' = a * x + c * y + e
     *           y' = b * x + d * y + f
     * depends on:
     *       processBDC(), processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     *      
    # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {���:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {���:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # һֱ��ŵ������µ�tm����
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
            
            //# 1. ���� ҳü��ҳ�ţ� ˮӡ
            //#    �ж��Ƿ����  /Artifact && ��/Top   /Bottom   /watermark����һ 
            //#     ��������� ����˺����BDC, EMC ֮�������
            //# 2. ������ҳü��ҳ��, ���������ҲҪ����, ���������������ִ���.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # ҳü  , BDC ����ʼ��ʶ
                while( tmpbuf ) {
                    printf("ҳü����. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# ҳü�����ı�ʶ
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
                // ҳ�ŵĴ���
                while ( 1 ) {       // # �����ѭ���жϱ������ѭ���жϿɶ���Ҫ��һ��
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# ҳ�Ž����ı�ʶ
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BDC" ) ) {       // # һ������  ���� BDC/EMC  ���ֲ���
                processBDC( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # ��� �� re ��Ϣ�� ��ʾ�Ǳ��, ��ȡcell ��Ϣ, �������Tm, ����cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # ������q ��Ϣ, ��ô��ʾ���浱ǰ��ͼ��״̬��Ϣ, ����������Ϣ, Ȼ��Q �ٻָ�
            /* ��ʱ������q/Q, ��Ϊre ʹ�õ��Ǿ�������
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */

            //# ����������Ϣ,���������ǵ�ǰ������Ҫ��type0����
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # ���buf �� �ı�λ����Ϣ, ������õ�ǰ��xy ����
                # 2016.12.13:
                #   ����������λ��, ���ֻ����Tm Ҳ����ת��������Ϣ, ��֤���µ�ת������������. �ı����������ı��ڲ�����
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm ������tm ������ͬ, ����Tm�Ĵ������ȿ�
            //#    self.processCm( tmpbuf, tm )
            
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );

            //# ���buf ���ı���Ϣ (���ʱ��Ӧ��û��BDC/BT, ���������desbuf Ӧ���滻Ϊ&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "ET" ) ) {  // # ���Ľ����ı�ʶ
                printf("����ET, BT/ET �������. tmpbuf=%s\n", tmpbuf );
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }
    
    /*
     * 17. processBDC()
     *      ���� BDC/EMC �ڵ�����
     * ��ڲ���:
     *      streambuf           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      ��
     * ˵��:
     *      1. ����ҳü/ҳ��, ����еĻ�
     *      2. �����Td/TD/Tm/T*, �ͻ�ȡλ����Ϣ, ����֮ǰ�����λ����Ϣ���бȽ�, ���ж��Ƿ�Ҫ����
     *      3. �����Tj,TJ, ', " , ��ʾ�ı�����, ժȡ�ı�����, �����Ҫ��������н���
     *      4. �����re, һ����Ӧf*, Ҳ���ǻ����β���  
     *          x, y, width, height    re   // x,y �����½ǵ�����
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
            
            //# 1. ���� ҳü��ҳ�ţ� ˮӡ
            //#    �ж��Ƿ����  /Artifact && ��/Top   /Bottom   /watermark����һ 
            //#     ��������� ����˺����BDC, EMC ֮�������
            //# 2. ������ҳü��ҳ��, ���������ҲҪ����, ���������������ִ���.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # ҳü  , BDC ����ʼ��ʶ
                while( tmpbuf ) {
                    printf("ҳü����. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# ҳü�����ı�ʶ
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
                // ҳ�ŵĴ���
                while ( 1 ) {       // # �����ѭ���жϱ������ѭ���жϿɶ���Ҫ��һ��
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# ҳ�Ž����ı�ʶ
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BT" ) ) {       // # һ������  ���� BT/ET  ���ֲ���
                processBT( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # ��� �� re ��Ϣ�� ��ʾ�Ǳ��, ��ȡcell ��Ϣ, �������Tm, ����cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # ������q ��Ϣ, ��ô��ʾ���浱ǰ��ͼ��״̬��Ϣ, ����������Ϣ, Ȼ��Q �ٻָ�
            /* ��ʱ������q/Q, ��Ϊre ʹ�õ��Ǿ�������
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );


            //# ����������Ϣ,���������ǵ�ǰ������Ҫ��type0����
            if ( strstr( tmpbuf, "Tf" ) ) {
                //processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # ���buf �� �ı�λ����Ϣ, ������õ�ǰ��xy ����
                # 2016.12.13:
                #   ����������λ��, ���ֻ����Tm Ҳ����ת��������Ϣ, ��֤���µ�ת������������. �ı����������ı��ڲ�����
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm ������tm ������ͬ, ����Tm�Ĵ������ȿ�
            //#    self.processCm( tmpbuf, tm )
            
            //# ���buf ���ı���Ϣ (���ʱ��Ӧ��û��BDC/BT, ���������desbuf Ӧ���滻Ϊ&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "EMC" ) ) {  // # ���Ľ����ı�ʶ
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }

    /*
     * 23. processRE()
     *      ���� re �ڵ�����
     * ��ڲ���:
     *      buf                 ��ǰ����TD ��һ����Ϣ
     * ���ڲ���:
     *      cur_cell            ��ϣ��, ��¼��Ԫ������ "x":x����ֵ, "y":y����ֵ, "w":width, "h":height
     * ����ֵ:
     *      ��
     * ˵��:
     *      �����re, ��˵���Ǳ��, ��¼Tj/TJ/'/" ֮ǰ�����µ�re ������Ϊ���µ�cell ��Ϣ��
     *  ��������ж��re, ��ô�������µ�re��Ϣ��, cur_cell Ҳ��ŵ������µ�re��Ϣ
     *      �������Tm, ����cur_cell=[:], pre_cell=[:]
     *      x, y, width, height  re   ,  ��һ������. x,y �����½�����.
     *      RE ���ص���CELL �Ĵʵ�, {'re1':[x,y,w,h],'re2',}
     * 2016.12.10:
     *      �ı�˼·, ÿ�δ���re��ʱ�򽫻�ȡ��cell��Ϣ�����һ��cellMap��, �������ݽ���������,
     *      �ٽ�� textMap��cellMap���������Լ��������ֵĻ�������.
     *      1. ��Щpdf�ĵ���re ��x�Ǹ���, w �ߴ�ü�ǧ, ����:
     *              -1638.4 72 3795.5 39.1 re
     *      �������������Ƶ�, �������޹�, ���Թ��˵�, δ�������Ҫ������ʾ�ٴ���
     *      2. ����߶�С��1, �����������ָ�ҳü��, ����:
     *          76.55 55.2 442.2 0.72 re
     *        ���ָ߶�ʱ����������������.
     *      3. ��ȡ���еı����Ϣ��, ��Ҫ����һ�ι���, �Ѱ�������cell���˵�, Ŀǰ����ֻ��һ�������������.
     *        ����ҳ��߿��и����, �����������������еĺϷ����(������re����).
     *        Ŀǰ�򵥴���, ��һ��re �����������, ֱ�Ӱѵ�һ��re���˼���.
     *          0 0 595.3 841.9 re
     * 2016.12.21:
     *      re Ҳ��Ҫ����Tm ����������ת��, �����ı�λ������λ�þ��޷��ж���ȷ��.  ����Ҫת��, ÿ�ζ��Ǿ�������, �ƺ���q/Q�й�. �����֮ǰ������ת����Ϣ???
     *      cellIndexMap[cellid] = {"maxlen":,"col":, "row":, "txtList":[]}
     * 2016.12.22:
     *      ��һ�ּ�����������ݴ�. ��ǰcell��ȫ����һ����Чcell֮��, ������cell
     * 2017.01.15:
     *      �����ǰcell����֮ǰ��ĳ����Чcell, ���õ�ǰcell ���֮ǰ����Чcell
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
                j = i;      // ��һ�����ʼλ����׼��
                
                switch ( n ) {
                    case 1:
                            cp->x = atof( buf );      // ��Ԫ���x���� 
                            n ++; 
                            break;
                    case 2:
                            cp->y = atof( buf );      // ��Ԫ���y����  
                            n ++;
                            break;
                    case 3:
                            cp->w = atof( buf );      // ��Ԫ���w���� 
                            n ++;
                            break;
                    case 4: 
                            cp->h = atof( buf );      // ��Ԫ���h����  
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        //# x ����С��0�����ǿ����õ�, h,w��ֵС��1�������������ָ��ߵ�, (x,y)=(0,0)��ԭ����ʱ�ı��Ҳ�ǿ�����(ҳü����������ԭ��)
        //# ��ҳüҲ����, �����ж�ҳü�ı�
        printf( "-------------------2----------processRE(), srcbuf=%s, x=%.2f, y=%.2f, w=%.2f, h=%.2f, cp=%p\n", srcbuf, cp->x, cp->y, cp->w, cp->h, cp );
        if ( cp->x < 0 || cp->y < 0 || cp->w < 1 || ( cp->x ==0 && cp->y == 0 ) )  //# (0,0)��Ȼ����, ��ʱû�뵽��ô����
            return NULL;

        //# 2017.01.15:  ���˻������Ч��cell
        printf( "----------------------------------------3--------------------------processRE(), srcbuf=%s, cp=%p\n", srcbuf, cp );

        fillCellMap( decode_p, cp );
        
        printCellMap( decode_p->cellMap_p );
        
        return NULL;
    }

    /*
     * 23.1. filterCell()
     *       ������Ч��cell
     *  ˵��:
     *      ��Щcell ����Ч��, ���������һ����Чcell֮�ڡ� ��������
     *  2017.01.15:
     *      1. ��ǰcell �Ѿ���֮ǰ����Чcell �ڲ�, ����
     *      2. �����ǰcell ����֮ǰ�� ��Чcell�ڲ�, ���õ�ǰ��cell ���֮ǰ����Чcell   
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

    // ����ָ��ID�ĵ�Ԫ��cell
    CELL * findCell( CELL * cellMap_p, int cellID )
    {
        CELL  * cp = cellMap_p;  // ��ͷ��ʼ����

        while ( cp ) {
            if ( cp->id == cellID )     // �ҵ���
                return cp;
            
            cp = cp->next;
        }

        return NULL;    // �����껹û���ҵ��ͷ���NULL
    }


    // ����cell��Ϣʱ�������Ƿ���ҳü, ��֤����������
    // ����cell��ʱ��, �жϳ���ҳü(h<1)ҳ���ٴ������˼���
    // ���ֻ����һ��cell:  ������1��, ����һ��cell
    int fillCellMap( DECODE *decode_p, CELL *cp )
    {
        printf("-------------------fileterCell()--cellMap_p=%p----cp=%p---\n", decode_p->cellMap_p, cp );
        printCellMap( decode_p->cellMap_p );

        assert( decode_p );
        printf("after assert(decode_p)------------------------------------------------------------------------------aaaaaaaaaaaaaaaaa\n");
        float       cx, cy, ch, cw; 
        float       px, py, ph, pw;     // �����е�cell��x, y, w, h
        CELL    *   cpt;                // ��������һ��cell


        if ( !cp )              // Ҫ��ӵ�cellΪ��, �򲻲���
            return 0;

        cx = cp->x;
        cy = cp->y;
        cw = cp->w;
        ch = cp->h;


        if ( !decode_p->cellMap_p ) {       // ���cellMap_pΪ��, �����ǵ�һ��cell, ֱ�����
            printf("cellMap_p Ϊ��,ֱ�����cp-----------------------------------------111  , \n");
            cp->id = 1;
            cp->last = cp;
            cp->prev = NULL;
            cp->next = NULL;
            decode_p->cellMap_p = cp;
            return 0;
        }
        // ֻ�����һ��cell�Ƚ�, ��Ϊÿһ���¼����cell���Լ���ǰһ���Ƚ�(���һ��), �൱��ȫ�Ƚ���
        // ��Ϊ��ǰ����һֱ�ڱ仯, ��������һ��cell��һ��, Ҳ�϶�������֮ǰ��cellһ����

        printf("----------------2--------------cellMap_p ��Ϊ�յĴ���, cellMap_p=%p, cellMap_p->last=%p\n", decode_p->cellMap_p, decode_p->cellMap_p->last);

        assert( decode_p->cellMap_p->last );
printf("================asf=============\n");        
        cpt = decode_p->cellMap_p->last;       

        px = cpt->x,   py = cpt->y, pw = cpt->w, ph = cpt->h;
            
        if ( !( abs( px - cx ) < 1 && abs( py - cy ) < 1 && abs( px+pw-cx-cw) < 1 && abs( py+ph-cy-ch) < 1 ) )  {
            // �õ�Ԫ�� û��  ������cell���� ���� ����������Ԫ��, �������С��1
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

        free( cp );     // ��cp �Ǹ����õ�, ����, �ͷ��ڴ�

        return 0;
    }


    char * processTf( char * buf, DECODE * decode_p )
    {
        char    * tmpbuf;

        tmpbuf = strsplit1( buf, ' ', 1 );      // /FT15 180 Tf  ��ʽ
        strcpy( decode_p->tf, tmpbuf +1 );      // +1 ��Ϊ������'/'   
        
        free( tmpbuf );                         // ��Ϊstrsplit1() �����������ڴ�
        
        return decode_p->tf;
    }

    /*
     *  18. hasText( buf )
     *          �жϵ�ǰ���Ƿ����ı���Ϣ��ʶ��  Tj / TJ/ ' / "
     *  ��ڲ���
     *      buf     ��������ַ���
     *  ���ڲ���
     *      ��
     *  ����ֵ
     *      False       û�а����ı���Ϣ��ʶ��
     *      True        �����ı���Ϣ��ʶ��
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
     *          �����ı���Ϣ, �ж��Ƿ���Ҫ����, �Ƿ���Ҫ����
     *  ��ڲ���:
     *      
     *  ���ڲ���:
     *      
     *  ����ֵ:
     *      
     *  ˵��:
     *      1. �ж�cur_cell �� pre_cell, �����Ϊ��, Y ��ͬ, ����ͬһ��, ������
     *         1.1 �ж� cur_xy �Ƿ��� cur_cell ��, 
     *      2. ֻ�д����ı���ǰ, �ŵ���ifCRLF(),Ȼ��cur_xy , cur_cell ���浽pre_xy, pre_cell,
     *         ��ʾ��һ�δ����ı���λ�ñ����ˡ� ��ifCRLF() �д���λ����صı���
     *  2016.12.10:
     *      �ı�ʵ��˼·. �½�һ��textMap �ֵ�, key:value=��ʼ����:�ı�����
     *      ��re�Ĵ�����, ����������CellMap �ֵ���.
     *      �������ı�������������, ����textMap �Լ�cellMap, �������ı��ϲ��Լ����еĴ���.
     * 2016.12.23:
     *      ����tf��Ϣ,ʵ����Ҳ������������.
     *      ��һ����stateStack, cur_state��ʱ��, ������Ϣ��Ϊ״̬��һ����. Ŀǰ״̬����: ����, ����.
     * depends on:
     *       self.ifCRLF(), .processTj(), .processTJ()
     *          
     */
    //# cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    //# textMap = {���:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    //# cellMap = {���:{'x':,'y':,'w':,'h':}}
    //# tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # һֱ��ŵ������µ�tm����
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p ) 
    {
        // cur_xy, textMap, cellMap, cellIndexMap, tm, tf
        CMAP    * cmap_p;

        if ( !srcbuf || !decode_p || !pages_p )
            return NULL;

        // �ҵ������Ӧ��cmap
        for ( int i = 0; i < pages_p->cmaptotal; i ++ ) {
            if ( strcmp( pages_p->cmaps_p[i].fontname, decode_p->tf ) == 0 ) {  // �ҵ������Ӧ��CMAP
                cmap_p = &( pages_p->cmaps_p[i] );
                break;
            }
        }
        printf("�ҵ�CMAP, fontname=%s, ��������:%d\n", cmap_p->fontname, cmap_p->total );
        // ���濪ʼ����
        
        if ( strstr( srcbuf, "Tj" ) )               // Tj
            processTj( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "TJ" ) )          // TJ
            processTJ( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "\'" ) )  {         // '  ������, ��ͬ��T*  Tj Ҳ�����Ƶ���һ����ʼλ������ı�
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf );
        }
        else if ( strstr( srcbuf, "\"" ) )  {         // "   ˫����,  ʹ�����ּ�����ַ����, ���ݶ����뵥����һ��
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf); 
        }
        
        printf( "����ǰ:%s\n" , srcbuf );
        printf( "�����:%s|\n", desbuf );
        //printCUR_XY( cur_xy_p );
        //
        // ���ı���Ϣ��䵽 textMap ��, ����δ����ʽ����, ͬʱ�жϸ��ı������ĸ�cell, ��¼��CELL��Ϣ��
        // 
        fillTextMap( desbuf, decode_p );
        
        return NULL;
    }   

    // ���������ı������� textMap �Լ� cellMap ��
    int fillTextMap( char * buf, DECODE * decode_p )
    {
        float       ox, oy;                   // �ı���x,y ����, Ϊ�˼򻯴����е�ָ��ṹ���Ƴ���, ���ӿɶ���
        float       cx, cy, cw,ch;          // cell��x,y,w,h Ϊ�˼򻯴����е�ָ��ṹ���Ƴ���, ���ӿɶ���
        TEXT    *   tp;                     // ����µ��ı�ӳ�䵥Ԫ�� �����ӵ��ı�ӳ�����
        CELL    *   cp;                     // ָ�� decode_p->cellMap_p->last
        int         id;
        
        if ( !buf || !decode_p )
            return -1;
    
        tp = ( TEXT * )calloc( sizeof(TEXT), 1 );
        //tp = ( TEXT * )malloc( sizeof(TEXT) );
        //memset( tp, 0, sizeof(TEXT) );

        if ( !decode_p->textMap_p ) {         // ��� ���һ���ı�ӳ��Ϊ��, ��ʾ���ǽ��������ĵ�һ���ı���
            tp->id      = 1;
            tp->last    = tp;                          // �����½����ı�ӳ������, ���һ��Ҳ���ǵ�һ��
            tp->prev    = NULL;                // �׸��ڵ��prev (ǰһ���ڵ�) Ϊ��NULL
            decode_p->textMap_p = tp;
        } else {
            decode_p->textMap_p->last->next = tp;
            tp->prev = decode_p->textMap_p->last;   // �½�text��ǰһ��text����֮ǰ��last text
            tp->id = tp->prev->id + 1;
            decode_p->textMap_p->last = tp;         // ����last ָ�뵽�½��� TEXT �ڵ�
        }

        ox = tp->ox = decode_p->cur_xy.ox;            // �ı� ��������x
        oy = tp->oy = decode_p->cur_xy.oy;            // �ı���������y

        tp->len = strlen( buf );
        tp->buf = (char *)calloc( sizeof(char), tp->len + 1);
        memcpy( tp->buf, buf, tp->len );
        printf("fillTextMap(), tp->buf=%s, tp->id=%d\n", tp->buf, tp->id );

        // �������Ƚ��ı��뵥Ԫ������λ��, ����ı��ڵ�Ԫ����, ���¼�����Ϣ
        // ֻ�ж����һ����Ԫ��, ��Ϊ��������뵱ǰ���ı�������ӽ�, ������������, ֮ǰ�Ĳ����ܰ���
        if ( !decode_p->cellMap_p ) {     // cell ӳ���Ϊ��, ��ʾ���ı�����cell��, ֱ�ӷ���
            return 0;
        }
        cp = decode_p->cellMap_p->last;

        if ( !cp )
            return 0;

        cx = cp->x,  cy = cp->y, cw = cp->w, ch = cp->h;    // cell ��������Ϣ
        printf("1===========================cp=%p, cx=%.2f, cy=%.2f, cw=%.2f, ch=%.2f, txtTotal=%d\n", cp, cx, cy, cw, ch, cp->txtTotal );
        
        if ( ( ox >= cx ) && ( ox <= cx+cw ) && ( oy > cy ) && ( oy <= cy+ch ) ) {   // ���ı���cell����
            if ( tp->len > cp->maxlen )     // ����cell����󳤶�
                cp->maxlen = tp->len; 
            
            tp->cellID = cp->id;
            cp->txtTotal ++;                // ���ֻ�Ǽ��� cell �е��ı�����, ������������ı���, 
                                            // ���ݸ�����������cp->txtIDs_p�ռ�, �ٴ���һ��textMap_p
                                            // ����cell�е��ı���id ��䵽cp->txtIDs_p ������
        }
        
        return 0;
    }

    /*
     * 20. processTj( desbuf, srcbuf, cmap )
     *      ������ Tj ��ʶ �����ֲ���
     * ��ڲ���:
     *      buf     ������İ���Tj ��δ�����ı�
     *      cmap    ���ڱ�ҳ�����ı�����ʹ�õ�cmap ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      �������ı�
     * ˵��:
     *      ���ֻ�����ı����ݣ� ���ؽ������ı�
     * ����:
     <547F>Tj 482.391 0 TD<097E>Tj 482.391 0 TD<0D2D>Tj 480 0 TD<41F1>Tj 482.391 0 TD
<4AD2>Tj 482.391 0 TD<4AF1>Tj 482.391 0 TD<3E7C>Tj 480 0 TD<044A>Tj 482.391 0 TD
<1937>Tj 482.391 0 TD<1BA0>Tj 482.391 0 TD<0836>Tj 482.391 0 TD<34D7>Tj 480 0 TD
<41F1>Tj 482.391 0 TD<0868>Tj 482.391 0 TD<18C5>Tj 482.391 0 TD<4374>Tj 480 0 TD
<0E2A>Tj
     */
    // srcbuf = "547F"  ��������  len  = 4 ��4�ı���
    //
    int decodeR( char *desbuf, char * srcbuf, int len, CMAP * cmap_p )
    {
        wchar_t     wstr[len/4+1];      // srcbuf �� "547f"��ʽ, 4�ֽڶ�Ӧ��1�� unicode��2�ֽ� 0x547F�� 
        char        buf[4+1];
        int         min, max, mid;
        int         count;              // ��¼unicode ת���ַ������ֽ���

        if ( !srcbuf || !cmap_p || len%4 != 0 )   // len����4 ��Ϊ0��ʾ��ʽ������. 
            return -1;

        min = 0;
        max = cmap_p->total-1;      // �±��Ǵ�0��ʼ��

        int     wlen = len/4;      // 
        memset( wstr, 0, sizeof(wstr) );

        for ( int i = 0; i < wlen; i ++ ) {
            memset( buf, 0, 4+1);
            memcpy( buf, &srcbuf[i * 4], 4 );
            printf("buf=%s\n", buf);
            
            wstr[i] = Hex2Int2( buf );     // ���wstr[i] �Ǹ�����,�ȴ�ű���, ����<4C75> <9645> �е�0x4c75
            
            // �ұ����Ӧ�Ľ���  0x0645, ���۰���ҷ�, ��Ϊ cmap �Ǵ�С���������
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
        // wstr �д�ŵ��ǽ���������, Ҳ�������յ�����, ת��Ϊ���ֽ��ַ���
        count = unicode2str( desbuf, wlen * 2, wstr );
        printf( "\n---------------ת���������Ϊ:%s(len=%d)\n", desbuf, count );
        
       return count;     
    }

    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p )
    {
        char    tmpbuf[L_TMPBUF];
        int     i, j, k, n;            // ��������±꣬��ȡ<> ��() �ڵ�����, n ������¼Ŀ�껺�����±�
        int     len;

        if ( !srcbuf )
            return NULL;

        len = strlen( srcbuf );
        printf( "processTj() Begin:%s, len=%d\n", srcbuf, len );

        j = k = n = 0;
        for ( i = 0; i < len; i ++ ) {
            if ( srcbuf[i] == '(' ) {          // ()����Ҫ���� 
                j = i+1;          // j ������¼��ȡ���ݵ���ʼλ��
                while( i < len && srcbuf[i] != ')' )   // �ҵ�')'
                    i ++;
                if ( srcbuf[i] != ')' )        // �ݴ���
                    return NULL;
                            
                memcpy( &desbuf[n], &srcbuf[j], i-j ); // ����() ��������ݵ�Ŀ�껺����
                n += i-j;
            } else if ( srcbuf[i] == '<' ) {   // <>��ʾ��Ҫ����   
                j = i+1;                      // j ������¼��ȡ���ݵ���ʼλ��
                while( i < len && srcbuf[i] != '>' )   // �ҵ�'>'
                    i ++;
                if ( srcbuf[i] != '>' )        // �ݴ���
                    return NULL;
                memset( tmpbuf, 0, L_TMPBUF );            
                memcpy( tmpbuf, &srcbuf[j], i-j ); // ����<> ��������ݵ�Ŀ�껺����

                // ��ʼ����
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
     *      ������ TJ ��ʶ �����ֲ���
     * ��ڲ���:
     *      buf     ������İ���Tj ��δ�����ı�
     *      cmap    ���ڱ�ҳ�����ı�����ʹ�õ�cmap ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      �������ı�
     * ˵��:
     *    1. TJ ����, ��ʾ�����֣� ������������ʽ���ı�
     * ����:
     *    1.  [<41F7086E>-3<05192E37>-3<03C6>]TJ
     */
    char * processTJ( char * desbuf, char *srcbuf, CMAP * cmap_p )
    {
        return processTj( desbuf, srcbuf, cmap_p );
    }

    /*
     * 24. processQ( streambuf )
     *      ����q/Q ����Ϣ, q ����ͼ��״̬, Q �ָ�ͼ��״̬
     * ��ڲ���
     * ���ڲ���
     * ����ֵ
     * ˵��:
     *      ����q ��, ֱ�ӹ��˺������Ϣ, ֱ������Q. ��Ҫ��ͼ�εĴ���, ���ڱ�ϵͳ����, 
     *      ֻ��ע������Ϣ, ���Կ��Թ��˵���Щ��Ϣ
     *      
     *      ���У� ���ܼ򵥵Ĺ���q/Q�м������, q/Q �м���ܻ�Ƕ��q/Q, ��������Ҳ��������������
     *      ��list ��ʵ�ֶ�ջ����ջ�����Ǹ� pre_cell, pre_xy
     *      ������ϸ����ҳ����Ϣ, ����q/Q �������ô�����, ��Ϊre ���Ǿ�������
     *      ��ʱ������q/Q
     */
    void processQ( MEMORYMAP * mm_p )
    {
        char    * buf;

        buf = mm_readLine( mm_p );
        
        while ( !strstr( buf, "Q") ) {
            //# ���û������Q, ʲôҲ����, ������ȡ��һ�С� ���˵�q/Q֮�������
            free( buf );
            buf = mm_readLine( mm_p );
        }
        free( buf );

        return;
    }

    /*
     * 25. hasTxtPosition()
     *      �ж�Ŀ���ַ����Ƿ����λ����Ϣ
     * ��ڲ���
     *      buf         ��������ַ���
     * ���ڲ���
     *      ��
     * ����ֵ
     *      false       ������λ����Ϣ
     *      true        ����λ����Ϣ
     * ע��:
     *      ��Щ����Ҳ���ܰ��� Td, TD �Ȳ������� Ӧ������ ���ı���������Ĳ����д��� !hasText( buf )
     * 2016.12.10:
     *      ����, ����Ĵ����߼�������..... ��û�б���������, buf �в����ܺ���Td��, ���Ǳ�����Ϣ,�޸��߼�, ɾ��!hasText() ����ж�
     *      ȱ����T* �� Tm���߼��ж� - -��
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
     *      �������λ����Ϣ���ַ���, ��λ����Ϣ��¼��cur_xy ��ϣ����, "x":x����ֵ�� "y":y����ֵ
     * ��ڲ���
     *      buf         ����λ����Ϣ���ַ���
     * ���ڲ���
     *      cur_xy      ��ϣ�� "x":x����ֵ�� "y":y����ֵ
     * ����ֵ
     *      ��
     * ˵��:
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
            processCm( buf, (CM *)tm_p );       // ��ʱ������
            return;
        } else if ( strstr( buf, "T*" ) ) {
            processTstar( buf, cur_xy_p );
            return;
        }
        
        printf("-------------------------s-------------\n");
    }

    /*
     * 27. processTd()
     *      ���� Td �ڵ�����
     * ��ڲ���:
     *      buf                 ��ǰ����Td ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      Td �ж��ֿ�����, 0 Tc 0 Tr 265 0 Td ���� -135.68 -14.78 Td
     *      1. -135.68 -14.78 Td �� 51134 L115, ���Ӧ����������ꡣ����
     *      2. 
     *      ���, Td ��������ݵ�����������ꣿ���Ǿ������ꣿ����
     *      3. 0 Tc 0 Tr 48.26 0 Td            50617  L41
     *      4. 0 Tr 144.54 0 Td                 50617 L44
     *      5. /TT0 9 Tf                        50617 L29
     *          529.68 51.72 Td
     *          (74)Tj
     *          1 0 0 1 538.68 51.72 Tm
     *          ( )Tj
     *      6. ���޸�cur_cell, pre_cell ������.
     * 2016.12.12:
     *      ����ʵ�ַ�ʽ, Td/TD�е����������Tm �еľ������ת��, ����:
     *        0.05 0 0 -0.05 80.52 146.45 Tm

     *        <0E2A>Tj 208.797 0 TD<4801>Tj 211.188 0 TD<31B0>Tj 208.797 0 TD<3020>Tj
     *      ��ô: ת�������Ӧ��[a b c d e f]=[0.05 0 0 -0.05 80.52 146.45]
     *      ����ת����ʽ: (�μ�: PDF 1.7 Reference P208)
     *          x' = a*x + c*y + e
     *          y' = b*x + d*y + f
     *      ��208.797 0 TD ʵ�ʵ�����Ϊ:
     *        x' = 0.05 * 208.797 + 0*0 + 80.52 = 184.9165 Լ184.917
     *        y' = 0 * 208.797 + (-0.05)*0 + 146.45 = 146.45
     *      �����ж�����ǰ��ı��
     *         75.15 134.4 130 16.1 re
     *       75.15 < x' < 75.15+130=205.15
     *       134.4 < y' < 134.4+16.1 = 150.5
     * 2016.12.26:
     *      Td �Ĵ���Ƚϸ���.
     *      1. ���TmΪ��,  ���֮ǰû������, ��(ox,oy) = (x,y); ���֮ǰ������, ��(ox,oy) = (x,y)+(x',y')
     *      2. ���Tm ��Ϊ��, (ox, oy) = (x,y) * tm, Ȼ�� tm �ÿ�
     *
     * depends on:
     *      self.getOriginXY()
     *   
     * 
     */
    // getXYofTd() �����Ǳ��غ���, �򻯴����,����processTd() �е���
    // 0 Tc 0 Tr 48.26 0 Td
    // ��ȡTd ֮ǰ��xy ���� 
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
        
        while ( i <= len ) {  // ֱ��β��
            if ( buf[i] == ' ' || buf[i] == 0 ) {  // �����ո���β
                memset( buf1, 0, 12 );
                strcpy( buf1, buf2 );       // ��һ��ѭ�������, ��ʵ��û�����ݵ�,�ǿմ�����, ��������������
                
                memset( buf1, 0, 12 );
                strcpy( buf2, buf3 );

                memset( buf3, 0, 12 );
                memcpy( buf3, &buf[j], i-j );
                if ( strstr(buf3, "Td" ) )
                    break;
                j = i+1;  // �ո����һ���ַ�
            }
            i ++;
        }

        cur_xy_p->x = atof( buf1 );
        cur_xy_p->y = atof( buf2 );

        return 0;
    }

    void processTd( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        getXYofTd( buf, cur_xy_p );     // ��ȡ Td ֮ǰ��xy����
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "Td" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // ȫΪ0��ʾû��Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]��ֵ
            memset( tm_p, 0, sizeof(TM) );      // ����tm_p������Ϊ0
        }
    }

    /*
     * 28. processTD()
     *      ���� TD �ڵ�����, ��ȡ(x,y) ������Ϣ
     * ��ڲ���:
     *      buf                 ��ǰ����TD ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *
     *  2016.12.10:
     *     ԭ���Ĵ���������, :<0016>Tj 91.188 0 TD<0015>Tj
     *     <547F>Tj 180 0 TD<097E>Tj 180 0 TD<4AD2>Tj 180 0 TD<4AF1>Tj 180 0 TD<0836>Tj 180 0 TD<34D7>TjET
     *     �������ҪǶ�״��� ֱ��һ�д������(�����ҵ����һ�������� ET)
     *  2019.02.14:
     *     ����Tj(���������TD)��TD Ҫ����Ƕ�״���һ����,ȷ���������ݶ�������ȫ.
     *     ����Ҫ����TD���������TD, ֻ��Ҫ�����һ��TD����. ��Ϊ���е�TD��y ������ͬ, ֮����ʱ���TD, ����ΪX���겻ͬ
     *
     * ע��: 
     *     �������Ǳ���ڵ�����, ������Ҫ�������һ��TD����ȡ���µ�λ����Ϣ. �����ԣ���
     *
     */
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ) 
    {
        getXYofTd( buf, cur_xy_p );     // ��ȡ Td ֮ǰ��xy����
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "TD" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // ȫΪ0��ʾû��Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]��ֵ
            memset( tm_p, 0, sizeof(TM) );      // ����tm_p������Ϊ0
        }

    }    

    /*
     * 29. getOriginXY()
     *      ��ȡTd/TD �����Matrics ���꣬��ΪTd/TD �󲿷�������������. 
     * ˵��:
     *      1. �÷���ֻ��processTd()�� processTD() ����
     *      2. Tm, re �����Ǿ�������, ������ø÷���
     *      3. T* Ҳ���õ��ø÷���, ��Ϊ��ֱ�ӻ�����, ���ý����ж��ˡ�
     * 
     *      
     */
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p )
    {
        printf("getOriginXY():-------------======================================\n");
        //print(cur_xy)
        //print(tm)

        // ��������ȡox, oy, Ҳ���ǲο������ԭʼ����ֵ(������), ���x,y ������Ǿ�������,��ox,oy = 0,0
        if ( !tm_p ) {      //# ���û��tm, Ҳ����û��ת������, ��x,y ���Ǿ�������
            if ( cur_xy_p->ox == 0 ){   //   # ����������ǵ�һ������
                cur_xy_p->ox = cur_xy_p->x;
                cur_xy_p->oy = cur_xy_p->y;
            } else {
                cur_xy_p->ox += cur_xy_p->x;
                cur_xy_p->oy += cur_xy_p->y;
            }
        } else {    // �����tm, Ҫ��������ת��
            //# x' = a*x + c*y+e,  y'=b*x+d*y+f
            cur_xy_p->ox = tm_p->a * cur_xy_p->x + tm_p->c * cur_xy_p->y + tm_p->e;
            cur_xy_p->oy = tm_p->b * cur_xy_p->x + tm_p->d * cur_xy_p->y + tm_p->f;
        }
    }            

    /*
     * 30. processTm()
     *      ���� Tm �ڵ����� 0.05 0 0 -0.05 233.759 92.57 Tm
     * ��ڲ���:
     *      buf                 ��ǰ���� Tm λ����Ϣ ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      �ر���Ҫע�����, Tm �Ǿ�������, ����Tmʱ, pre_cell, cur_cell �Ϳ�����Ϊ[;], preT_xy Ҳ�ÿ�[:]
     *      Ҳ����˵������Ҫ�� Tm ������Ϊ��׼���м������ж�
     * 2016.12.21:
     *      ������cur_xy����. ��Ϊһ������Tm, ��ô֮ǰ�����궼û����. ��Ҫ�����µ�ת������������
     *      ����Tm��, ��ʼ�� (x,y)= (0,0)*Tm= (e,f)
     *      �ƺ����е�Tm ����BT/ET �м�. ���� ET֮��, ��Tm ��ʧЧ��.
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
                j = i;      // ��һ�����ʼλ����׼��
                

                switch ( n ) {
                    case 1:
                            tm_p->a = atof( buf );      //��1���� tm_p->a ��ֵ 
                            n ++; 
                            break;
                    case 2:
                            tm_p->b = atof( buf );      //��2���� tm_p->b ��ֵ 
                            n ++;
                            break;
                    case 3:
                            tm_p->c = atof( buf );      //��3���� tm_p->c ��ֵ 
                            n ++; 
                            break;
                    case 4: 
                            tm_p->d = atof( buf );      //��4���� tm_p->d ��ֵ 
                            n ++;
                            break;
                    case 5: 
                            tm_p->e = atof( buf );      //��5���� tm_p->e ��ֵ 
                            n ++;
                            break;
                    case 6: 
                            tm_p->f = atof( buf );      //��6���� tm_p->f ��ֵ 
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        
        cur_xy_p->x = cur_xy_p->ox = tm_p->e;       // ��������ֵx
        cur_xy_p->y = cur_xy_p->oy = tm_p->f;       // ��������ֵx

        memset( cur_xy_p->opr, 0, L_OPR );
        strcpy( cur_xy_p->opr, "Tm" );

        //printTM( tm_p );
        //printCUR_XY( cur_xy_p );

        printf("--------processTM() end !srcbuf=%s\n", srcbuf );
        return;
    }

    /*
    # 30.1 processCm()
    #     ת������, Ŀǰ�����ú�Tm����, ��ͷ�����о�
    */
    void processCm( char * buf, CM * cm_p )
    {        
        printf("=================-�Թ�------------------------processTm(),%s\n", buf );
        // ��ʱ�Թ�
        return;
    }
    
    /*
     * 31. processTstar()
     *      ���� T* �ڵ�����
     * ��ڲ���:
     *      streambuf           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      buf                 ��ǰ����T* ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      T*   ֱ�ӵ���һ�е���ʼλ��
     *2016.12.13:
     *    ���������Ҫ���ƣ���������
     *    ����ҵ�T*, ��ôǰ��i-2, i-1����x,y 
     */
    void processTstar( char * buf, CUR_XY * cur_xy_p )
    {
        printf("processTstar() buf=%s\n", buf);
        cur_xy_p->x = 0;         
        cur_xy_p->y += 1;           //# ��+1 ��ʾ���ϴδ����λ�����, ������, ��ô������һ��
        strcpy( cur_xy_p->opr, "T*" );
        /*
        # ע�⣬ ����Ĳ����Ǽ���T* �����ǵ�һ��λ�ò������� Ŀǰ����Ҳ
        # ȷʵû�е�һ�������, �����������T*�ǵ�һ��λ�ò�����, �ͻ���쳣
        # ��Ϊ���ʱ��pre_xy["x"]�ȷ�������Ч�ġ�
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
    // ����Ĳ����Ƕ��ı������ĺ������� ��ԭ��ʽ
    //
    //
    void printCellMap( CELL * cellMap_p )
    {
        CELL    * cp = cellMap_p;
        
        printf("--------printCellMap()------------------\n");

        if ( !cp ) { 
            printf("------------------cellMap �� �յ�?-----------------------------------------\n");
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
        // # 1. ����textMap, �����ڱ���е��ı����д���, �����ͬһ�еľͽ��кϲ�, ͬʱ��cell�е�txtIDs_p ��������
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ
        //preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        /*       
        // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
        //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
        self.preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
        
        // # 4. �����ı���cell�Ĺ�ϵ, ��ֳ�table ����, ����tableMap={1:[cellid_list]}�� ͬʱ��cellMap �����tableid ����
        tableMap = {}
        self.buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print("--after buildTableMap()---cellIndexMap-----------")
        print(cellIndexMap)
        print(tableMap)

        // # 5. ����ı�. �Ѿ������ͬ�ж��ı���ƴ��, ����cell�е��ı����ǵ���һ���ı�
        retbuf = self.buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print(retbuf)
        
        return retbuf
        */
        return NULL;
    }

    
    // ɾ����ǰ�Ľڵ�
    void delCurTxt( TEXT * tp )
    {
        TEXT    * tpp, * tpn;    // ��ǰ�ڵ��ǰһ���ڵ�ͺ�һ���ڵ�
        
        if ( !tp ) 
            return;
        
        tpp = tp->prev;
        tpn = tp->next;

        free( tp->buf );
        free( tp );

        if ( !tpp ) {       // tpp Ϊ��, ˵��Ҫɾ����tp�ǵ�һ���ڵ�, ���һ���ڵ��ɵ�һ��
            tp = tpn;
            tp->prev = NULL;
            return;
        }
        
        if ( !tpn ) {       // ���tpnΪ��, ˵��Ҫɾ����tp �����һ���ڵ�
            tpp->next = NULL;
            return;
        }

        tpp->next = tpn;
        tpn->prev = tpp;

    }    

    // �ϲ�2���ı��ڵ��е��ı����ݵ���һ���ı��ڵ��buf��, 
    // ֻ��Ϊ�˼򻯴��룬���ӿɶ��Զ���������ĺ���
    void combineBuf( TEXT * tp1, TEXT * tp2 )
    {
        char        * buf;      // ��������ռ䲢��źϲ�����ı���Ϣ

        if ( ! tp1 || !tp2 ) {
            printf("�п�ָ�룡����\n");
            return ;
        }

        printf("�ϲ�ǰ, tp1->buf=%s, tp1->len=%d, tp1->id=%d, tp2->buf=%s, tp2->len=%d, tp2->id=%d\n", 
                        tp1->buf, tp1->len, tp1->id, tp2->buf, tp2->len, tp2->id );

        // ��Ϊ�ϲ�����ı���������ռ䲢����ϲ�����ı�, Ȼ���ͷ�֮ǰ��2���ı��ڴ�ռ�
        buf = ( char * )calloc( tp1->len + tp2->len + 1, 1 );   // +1 ��Ϊ�˴��β0
        memcpy( buf, tp1->buf, tp1->len );
        memcpy( buf+tp1->len, tp2->buf, tp2->len );

        free( tp1->buf );
        
        tp1->buf = buf;
        tp1->len += tp2->len;
        
        printf("�ϲ��������Ϊ:%s(textID=%d), len=%d\n", tp1->buf,tp1->id, tp1->len);
    }


    // # 1. Ԥ����textMap, �����ٱ���е��ı����д���, �����ͬһ�еľͽ��кϲ�
    //# 15.3 preProcTxtMap()
    //#     Ԥ����textMap, �����ڲ��ڵ�Ԫ��������ı����д���,  ��ԭ����һ�е�ƴ����һ��, ɾ��������ı����, ��������
    //#     ҳü�ڵ��ı�Ҳ����ƴ�Ӵ���
    //# depends on:
    //#       delTxt( item, textMap, cellIndexMap )
    //# 2017.01.10:
    //#     ����ʵ�ַ�ʽ, ѭ������ oy ����������´���
    //# 2019.02.24
    //      ����C���԰汾�� ����cellMap_p ��Ԥ����, Ҳ����Ϊ���ı���cell��txtIds_p ����ռ�
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp;    // tp ��ŵ�ǰ�ڵ�ָ��, tpp �ǵ�ǰ�ڵ��ǰһ���ڵ�
        CELL        * cp;

        float       ox, oy, px, py;     // ��¼��ǰ�ı��������ǰһ���ı�������

        if ( !textMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID != 0 ) {   // ������0, ��ʾ���ı���cell��, ������������
                tp = tp->next;
                continue;        
            }
            
            if ( !tp->prev )  {          // ���û��ǰһ���ı����Ͳ�����, Ҳ���ǵ�һ���ı�
                tp = tp->next;
                continue;
            }

            tpp = tp->prev;             // ��tpp ��Ϊ����ߴ���ɶ���, ��Ȼ�༶ָ�뿴�����Ѿ�

            ox = tp->ox,        oy = tp->oy;
            px = tpp->ox,       py = tpp->oy;

            printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
            if ( abs( py - oy ) < 1 ) {     //  # ����Y����С��1��Ϊͬһ��

                
                if ( px <= ox ) {        // ��С������, С����ǰ��
                    // tpp С, ����tp->prev
                    combineBuf( tpp, tp );

                    delCurTxt( tp );                    // ɾ��tp �ڵ�
                    tp = tpp->next;                     // tp ���¶�λ����һ����Ҫ����Ľڵ�
                } else {
                    // tp С������ tp
                    combineBuf( tp, tpp );


                    delCurTxt( tpp );                    // ɾ��tpp �ڵ�
                    
                    tp = tp->next;                      // tp ���¶�λ����һ����Ҫ����Ľڵ� 
                }
            } else {
                printf("���úϲ���\n");
                tp = tp->next;
            }
        }     

        // 2. ����Ϊ��Ԫ�� ���ı���ż�¼����ռ�(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal != 0 )
                cp->txtIDs_p = (int *)calloc( sizeof(int), cp->txtTotal );   // Ϊ��ż�¼cell�е��ı��������ռ�
            cp = cp->next;
        }

        printf("preProcTxtMap() end \n");
    }

    // �ӵ�ǰ�ڵ���ǰ����ָ��ID�� TEXT
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

        // ��ǰ�������û�ҵ�
        return NULL;
    }

    // �������û���ҵ�ָ��id��CELL
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

        // �������û�ҵ�
        return NULL;
    }

    // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ
    // ����˼·:
    //    1. ����һ�����е�text�ڵ�, ��������cell�еĽڵ�
    //    2. ������cell�е��ı��ڵ�, �ҵ���Ӧ��cell, Ȼ�����cell�е�֮ǰ���ı����бȽ�, ���Ƿ���Ҫ�ϲ��ı�(ͬһ��)
    void preProcTxtMap4Cell( TEXT *textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp, * tpn;    // tp ��ŵ�ǰ�ڵ�ָ��, tpp �ǵ�ǰ�ڵ��ǰһ���ڵ�, tpn ����һ���ڵ�
        CELL        * cp;
        int         cell_id;   
        int         text_id;  

        float       ox, oy, px, py;     // ��¼��ǰ�ı��������ǰһ���ı�������

        int         i;                  // �����±�

        if ( !textMap_p || !cellMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID == 0 ) {   // ����0, ��ʾ���ı�û����cell��, ����������
                tp = tp->next;
                continue;        
            }

            cp = getCellByID( cellMap_p, tp->cellID );
            // ��ʱ, ���ı���id ��û�д����cell �е�txtIDs_p ������, ֮ǰ�Ĵ���������������㹻�Ĵ�ſռ�
            // ������㴦��㽫�������ı���id��ŵ�txtIDs_p ������
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->txtIDs_p[i] == 0 )   // ������β��
                    break;
            
            if ( i == 0 ) { // ��ʾ���ı���cell �еĵ�һ���ı�, ��¼��txtIDs_p[0] ��, �����ϲ��ı�����
                cp->txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {   // �ݴ���,��ʾԤ����ռ��Ѿ�������, ��ô��ǰ������ı��Ͳ�Ӧ���ڸ�cell��
                return NULL;
            } else  {
                text_id = cp->txtIDs_p[i-1];
                tpp = searchForward( tp, text_id );
                if ( !tpp )                     // �ݴ��� 
                    return NULL;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {     //  # ����Y����С��1��Ϊͬһ��
                    // �ϲ��ı�, ���� tpp, ��Ϊtpp�Ѿ���¼����cell����
                    combineBuf( tpp, tp );
                    if ( tpp->len > cp->maxlen )    // ����cell�е�����ı�����
                        cp->maxlen = tpp->len;

                    tpn = tp->next;
                    delCurTxt( tp );                    // ɾ��tpp �ڵ�
                    
                    tp = tpn;                      // tp ���¶�λ����һ����Ҫ����Ľڵ� 
                } else {    // ����Ҫ�ϲ�
                    cp->txtIDs_p[i] = tp->id;       // ���cell�е��ı���¼
                    tp = tp->next;
                }
            }

        }


        return;
    }
    /*
    // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
    colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
    {
        return;
    }
    
    // # 4. �����ı���cell�Ĺ�ϵ, ��ֳ�table ����, ����tableMap={1:[cellid_list]}�� ͬʱ��cellMap �����tableid ����
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }

    // # 5. ����ı�. �Ѿ������ͬ�ж��ı���ƴ��, ����cell�е��ı����ǵ���һ���ı�
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }
*/
