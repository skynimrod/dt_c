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
    #include "file_tools.h"
    #include "filemap.h"
    #include "str_tools.h"
    #include "pdf_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

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
    #define ID_LEN  128             // �趨 Trailer�е�ID ��󳤶�Ϊ128�� һ���ǲ��ᳬ���������
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
        int         obj;                        // type0 �����Ӧ�Ķ�����
        char        fontname[L_FONTNAME+1];     // type0 ��������
    }FONTMAP;

    typedef struct __pages__ {
        int         total;      // ��ҳ��
        int         cursor;     // ���һ��Ҷ��ҳ�����洢��leafs�����е�λ��(�±�), �����Ϊ�˱��ڴ���
        int     *   leafs_p;        // ����, �������Ҷ��ҳ�������
        int     **  c_objlist_p;    // leafs_p�е�Ҷ�Ӷ�����������ݶ����б� c_objlist_p[total][�����б�]
        FONTMAP **  fontmap_p;      // fontname[ҳ����total][���ٸ�����][FONTMAP]
    }PAGES;

    // ������ CMAP ���ֵ����ݽṹ����
    typedef struct __cmap__ {
        int         code;
        int         decode;
    } CMAP;

    typedef struct __cmaplist__ {
        int         pageNo;     // ҳ����
        int         count;      // CMAP ����
        CMAP        * cmaps_p;  // ��ҳ��ʹ�õ�CMAPs, �����Ƕ��
    }CMAPLIST;

    typedef struct __contentMap__ {     // ��¼ÿҳ�е�����ҳ�����
        int         obj;            // ���ݶ�����
        int         * objlist;      // ���ݶ����������Ķ����б�
    }CONTENTMAP;

    int     getXREF( FILEMAP * fm_p, XREF * xref_p,  long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p );
    
    void    freeAll( POSMAP * posmap_p, TRAILER * trailer_p, XREF * xref_p, PAGES *pages_p, FILEMAP * fm_p );
    void    print_trailer( TRAILER * trailer_p );
    int     getPages( FILEMAP * fm_p, XREF * xref_p,  PAGES *pages_p, int Root );
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );

    int     getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    
    int     getCMAPLIST( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p, CMAPLIST * cmaplist_p );

    // ����ĺ���Ϊ���Ժ���, �����������Ϣ������ļ���, ���ڲ���
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

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
    //			            
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
        /*
        CMAPLIST    *cmaplist_p;
        cmaplist_p = (CMAPLIST *)malloc(sizeof(CMAPLIST)+1);
        memset( cmaplist_p, 0, sizeof(CMAPLIST) + 1 );

        getCMAPLIST( fm_p, xref_p, pages_p, cmaplist_p );
        */
        // 3. ��ȡÿҳ��Ӧ�����ݶ����б� contentMap
        getContentMap( fm_p, xref_p, pages_p );
        getFontMap( fm_p, xref_p, pages_p );


        
        freeAll( posmap_p, trailer_p, xref_p, pages_p, fm_p );

        return 0;
    }



    //һ��Ϊ�ڲ����õ�API
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
                
                memset( trailer_p->ID, 0, ID_LEN + 1 );     
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
        printf("Prev=%d\n",     trailer_p->Prev );
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

        if ( buf )          // ���� fm_readLine() �ڲ��������ڴ�, �����Ҫ���ͷ�֮ǰ������ڴ�
            free( buf );

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

        // ��ʼ����xref�������ڶ���
        item = (char *)strtok( buf, " " );     // ��һ��: 1 5267 ֮��ĸ�ʽ(��ʼ������ ��������)
        int     i = 0;
        while( item != NULL ) {
            printf( "\nitem-%s\n", item );
            if ( !IsNumberStr( item ) || i >= 2 )   {   // ����������� �򳬹�2� ��ʾ��ʽ����ȷ
                free( buf );
                return -1;
            }
            
            if ( i == 0 )
                *firstObj = atoi( item );                // ��ʼ������
            
            if ( i == 1 ) 
                *objSum = atoi( item );                  // ����������
            
            item = (char *)strtok( NULL, " " );
            i ++;
        }
        free( buf );            // �ͷ��ڴ�

        printf( "\nfirstObj=%d, objSum=%d\n", *firstObj, *objSum);
        
        return 0;
    }

    // ����objSum < 1 �����.  �ݴ���
    void procObjSumLT1( FILEMAP *fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p )
    {
        char    *   buf;
        int         ret;

        printf("--in procObjSumLT1()\n");

        xref_p->retcode = -40424;       // Ԥ��һ��������
        buf = fm_readLine( fm_p );

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

        if ( trailer_p->retcode < 0 || !strstr( buf, "Prev" ) || trailer_p->Prev <= 0 ) { 
            // ��Ч��xref֮���trailer��Ч��û�а�����Ч��self.xref ��ַ, �������
            free( buf );
            return xref_p;
        }

        if ( buf )
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

        objpos_p = ( long * )malloc( sizeof(long) * objSum );       // ָ��һ��long����, ����δobjSum

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
        
        return 0;
    }


    int getXREF( FILEMAP * fm_p, XREF * xref_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
    {
        int         firstObj;       // xref �е�һ�м�¼�ĵ�һ��������
        int         objSum;         //                    ����������
        int         ret;
                
        if ( fm_p == NULL )
            return -1;

        // 1. ��һ�еĴ���
        ret = procFirstLine( fm_p, xrefpos, xref_p, posmap_p );
        if ( ret != 0 ) 
            return -1;

        // 2. �ڶ��еĴ���
        ret = procSecondLine( fm_p, &firstObj, &objSum, trailer_p );       // Ϊ�����ӿɶ���, ����һ�еĴ����װ��һ��������
        if ( ret < 0 )                     // ������������0 ��ʾ������,ֱ�ӷ���xref_p,  xref_p->retcode =-1(��ʼ��deֵ), Ҳ���Ǳ�ʾ������
            return -1;
        if ( ret == 1 ) {               // �������1, ��ʾ�ҵ�trailer��, ˵����Ҫ��������һ��xref, Ƕ�׵���getXREF(), ����Ĵ�������
            if ( !trailer_p || trailer_p->retcode < 0 ) {            // ���trailer������, �����
                return -1;
            }
            printf("\n������ˣ�\n");
            return getXREF( fm_p, xref_p, trailer_p->Prev, posmap_p, trailer_p );
        }

        // 3. objSum < 1 ������ݴ��� ��������п���, doc�ļ�ת��pdf��, ��һ��xref ����0 0, 
        // �����trailerָ���µ�xref, ���Ӧ�ü�������trailer�� ���û��Prev �ʹ���      
        if ( objSum < 1 ) {
            procObjSumLT1( fm_p, xref_p, posmap_p, trailer_p );
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

        
        for ( int i=0; i < xref_p->objTotal; i ++ ) {
            printf("xref_p->objpos_p[%d]=%016d\n",i, xref_p->objpos_p[i] );
        }
        
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

    // ��ȡ��ֺ�ĵڼ�����
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


    // ----------- ������ CMAP  ��ش���
    //
    // writeRawData()
    //     ���Ժ���, ��stream�е��ֽ���д�뵽�ļ���, ���ڲ鿴
    void writeRawData( char * filename, uchar *bytes, int len )
    {
        FILE        * fp;
        int         l;

        if ( !( fp=fopen( filename, "wb") )) 
            return;

        l = fwrite( bytes, len, 1, fp );
        printf("\nʵ��д���ļ�����l=%d, Ӧ��д�� len=%d\n", l, len);
        fclose( fp );
        
        return ;
    }

    // ���ݽ����������� ��ȡcmap �ı���
    int getCMAP( char *stream, CMAP * cmap_p )
    {
        return 0;
    }
    //
    // procType0Stream()
    int procType0Stream( FILEMAP *fm_p, XREF *xref_p, int obj, CMAP *cmap_p )
    {
        int     ret;
        // 1. F052  �������жԹ��ķ�����, �޷�print(), Ҳ�޷���txt�ļ�����ʾ. 
        char    * codeFilter[] = {"F052", "F0FE"};

        char    *buf;
        uchar   *bytes;
        int     len;
        
        printf( "\nprocType0_1()#####������Ϊ��\n" + obj );
        
        //# ��2�����.
        //    # 50569||<</Filter/FlateDecode/Length 7645>>stream
        //    # 50569||<</Filter/FlateDecode/Length 7645>>\r\nstream        
        //
        buf = getItemOfObj( fm_p, xref_p, obj, "Length" );
        if ( !buf ) {   // û���ҵ�Length� ������
            printf( "CMAP:obj=%d Error 3, no ToUnicode Length", obj );
            return -1;
        } 
        len = atoi( strsplit( buf, " ", 2 ) );    // ��ȡ /Length 7645 �е����� 7645, ���ڸ��ַ����ĵ�2��, Ҳ�����ֽ�������
        free( buf );

        // ����Ĵ���ҪС��, ��������content �а���stream, �Ͳ�Ҫ������һ����, ���������, ����Ҫ����һ�С�����������������
        buf = getObjContent( fm_p, xref_p, obj );
        if ( !buf ) {
            printf("%d û�л�ȡ������!\n", obj );
            return -1;
        }

        if ( !strstr( buf, "stream" ) ) {  //����������\r\nstream\r\n ����stream\r\n
            free( buf );
            buf = fm_readLine( fm_p );      // ���� stream\r\n
            free(buf);
        }

        bytes = fm_read( fm_p, len, fm_p->pos );

        writeRawData( "\nF:/F_T_tmp/tmp1/test_raw.dat\n", bytes, len );
        
        free( bytes );
        

        // OK�� ���� buf �е��ֽ�������steam �����ݣ� ������н�ѹ, ��ѹ������ݴ����desbuf
        /*
        if ( "stream" not in content):    # 2016.12.20  ����������\r\nstream\r\n ����stream\r\n
                print("û��stream, ��Ҫ������һ��--------------------------")
                buf = self.file_tools.readLine()                        # ���� stream\r\n
            buf, len1 = self.file_tools.read( len1 )  # ����ͨ�� getItemOfObj(), �ڴ��ļ� ���ö�λ��stream ����ʼλ��
            if ( obj == "51134") :
                print( "pos=%d; l=%d; len=%d, stream=" % ( self.file_tools.getPosition(),l, len1 ) )
                print( buf )

            with open("f:/F_t_tmp/tmp1/%s_row.dat" % obj, "wb") as fs:
                fs.write(buf)
          */      
    }

    // ����ָ���ַ����ַ����г��ֵĴ���
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

        printf("buf=%s,  \'%c\'���ֵĴ���Ϊ%d\n", buf, ch, n);

        return i;
    }

    // �жϸ����Ķ����Ƿ���type0 ����, ���Ƿ���CMAP
    bool isType0( FILEMAP *fm_p, XREF * xref_p, int obj ) 
    {
        char    *buf;

        buf = (char *)getObjContent( fm_p, xref_p, obj );

        if ( !buf )
            return false;

        if ( strstr( buf, "Type0" ) ) {     // �����Type0 ��, �������CMAP ����
            free( buf );
            return true;
        } else {
            free( buf );
            return false;
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
        
        pages_p->c_objlist_p = (int **)malloc(sizeof(int)*pages_p->total + 1 );
        memset( pages_p->c_objlist_p, 0, sizeof(int)*pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            printf("---------------------------%d-------------------\n", i+1);
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Contents" );
            printf( "Ҷ�Ӷ���(��%dҳ):%d| ����:%s", i+1, leafs_p[i], buf );

            if ( strchr(buf, '[' ) ) { // �����������, ��ʾ����������ݶ���,  Contents[5278 0 R 5279 0 R]
                count = countChrInStr( rtrim(buf), ' ' );  // �ȼ����ַ����г��ֵĿո���
                count = (count+1)/3;                // �������������, +1 ����Ϊ��һ������ֻ��2���ո�
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset(  pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );
                ret = getObjList( buf, count, pages_p->c_objlist_p[i] );
            }
            else {
                count =1;                   // û��������, ��1�����ݶ��� Contents 7 0 R
                printf("��һ�����ݶ���(buf=%s)\n", buf);
                
                pages_p->c_objlist_p[i] = (int *)malloc( sizeof(int ) * (count + 1) );
                memset( pages_p->c_objlist_p[i], 0, sizeof(int ) * (count + 1) );

                pages_p->c_objlist_p[i][0] = atoi( strsplit( buf, " ", 2 ) );       // �ڶ���������ݶ���
            }

            
            printf("\nobj %d �����ݶ���Ϊ:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", pages_p->c_objlist_p[i][j]);
            printf("\n");
            
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

    // <</Contents 57 0 R/Resources <</Font <</FT15 15 0 R /FT20 20 0 R /FT8 8 0 R>>>> /Type /Page>>
    // ��ȡҶ�Ӷ��� ��Ӧ�����ݶ����б�  pages_p->c_objlist_p;
    //  pages_p->fontmap_p ��2��ָ��, ��һ�������Ƕ��ٸ�Ҷ��ҳ��, �ڶ�����ҳ�����������
    int getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p )
    {
        int         ret;
        char        * buf, * item;
        int         * leafs_p = pages_p->leafs_p;       // �����Ϊ�˼򻯴��볤��
        int         count;
        bool        flag = false;
        
        pages_p->fontmap_p = (FONTMAP **)malloc(sizeof( FONTMAP * ) * pages_p->total + 1 );
        memset( pages_p->fontmap_p, 0, sizeof( FONTMAP * ) * pages_p->total + 1  );

        for ( int i=0; i < pages_p->total; i ++ ) {
            printf("----------------FontMap-%d--(obj=%d)-----------------\n", i+1, leafs_p[i] );
            count = getFontCount( fm_p, xref_p, leafs_p[i] );

            pages_p->fontmap_p[i] = (FONTMAP *) malloc( sizeof( FONTMAP ) * count  + 1 );  // FONTMAP ����
            memset( pages_p->fontmap_p[i], 0, sizeof( FONTMAP ) * count  + 1 );
            
            buf = (char *)getObjContent( fm_p, xref_p, leafs_p[i] );
            
            printf( "\n��ȡFont ����:%s\n", buf);
            item = (char *)strtok( buf, "/" );
            
            while( item && !flag ){     // flag = true ��ʾ�����Ѿ�������ϣ� �˳�ѭ��
                printf( "item = %s\n", item );
                if ( ! strstr( item, "Font" ) ) { // û�ҵ�Font �ͼ�������һ��
                    item = (char *)strtok( NULL, "/" );
                    continue;
                }
                
                //     ����ҵ���"Font", ����ѭ�������ȡ��������
                while ( item ) {
                    item = (char *)strtok( NULL, "/" );
                    printf("Font ��Ϣ: %s\n", item );
                    if ( strstr( item, ">>") ) {        // �������">>"��ʾ�������һ��������
                        flag = true;
                        break;
                    }
                }
            }
            free(buf);
        }
        return 0;
    }

    // ���� Pages_p ����ȡÿҳ��Ӧ��cmap�б�
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
            printf( "Ҷ�Ӷ���(��%dҳ):%d| ����:%s", i+1, leafs_p[i], buf );

            count = countChrInStr( rtrim(buf), ' ' );  // �ȼ����ַ����г��ֵĿո���
            if ( strchr(buf, '[' ) ) {    // �����������, ��ʾ����������ݶ���,  Contents[5278 0 R 5279 0 R]
                count = (count+1)/3;                // �������������, +1 ����Ϊ��һ������ֻ��2���ո�
                
                objlist = (int *)malloc( sizeof(int) * count + 1 );
                memset( objlist, 0, sizeof(int) * count + 1 );
                ret = getObjList( buf, count, * objlist );
            }
            else {
                count =1;                   // û��������, ��1�����ݶ��� Contents 7 0 R
                
                objlist = (int *)malloc( sizeof(int) + 1 );
                memset( objlist, 0, sizeof(int) * count + 1 );
                objlist[0] = atoi( strsplit( buf, " ", 2 ) );       // �ڶ���������ݶ���
            }

            printf("\nobj %d �����ݶ���Ϊ:", leafs_p[i] );
            for( int j=0; j < count; j ++) 
                printf("%d|", objlist[j]);
            printf("\n");

            free( buf );

            // ���洦��type0 �Ķ���, Ҳ����cmap����
            // 1. �Ȼ�ȡ Ҷ�Ӷ������Ϣ�е�Font����
            // 2. ���� Font ���ݣ� ��ø����������Ķ�����
            // 3. �����������(���ݻ�õĶ����Ż������), ����ö���������Type0, ��ʾ��cmap, �����¼
            // /Font<</C2_0 5 0 R/TT0 10 0 R/TT1 12 0 R/TT2 14 0 R/TT3 17 0 R>>
            // Font �����Ǻ���<<>>�����������ݣ� ���������ֶ���/��ʼ��
            buf = (char *)getItemOfObj( fm_p, xref_p, leafs_p[i], "Font" );
            printf( "Ҷ�Ӷ���(��%dҳ):%d| Font����:%s", i+1, leafs_p[i], buf );
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
        char        fontname[L_FONTNAME+1];     // ��������
    }CMAPS;

    typedef struct __cmaplist__ {
        int         pageNo;     // ҳ����
        int         count;      // CMAP ����
        CMAP        * cmaps_p;  // ��ҳ��ʹ�õ�CMAPs, �����Ƕ��
    }CMAPLIST;

 */
