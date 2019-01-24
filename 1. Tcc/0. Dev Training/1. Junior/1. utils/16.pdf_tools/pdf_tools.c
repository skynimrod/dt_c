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

    // λ����Ϣӳ���, ��¼�Ѿ���������xrefλ����Ϣ, ��ֹ�ظ�����,�����ѭ��
    typedef struct __posmap_ {
        long                xrefpos;
        struct __posmap_  * next;
    }POSMAP;

    // �������ñ�  XREF ��Ϣ
    typedef struct __xref__ {
        int                 retcode;
        int                 objTotal;   // ��������
        long              * objpos_p;       // �����ַָ��, objpos_p[0]�����ݵ����ݾ��ǵ�1������ĵ�ַ
    } XREF;

    // trailer ��Ϣ�а���: ��object��ţ�object����,  startxref( xref λ����Ϣ)
    typedef struct __trailer__ {
        int         Root;       // ������ID
        int         Size;       // ��������. �ȶ�������Ŵ�1. ���Ϊ0�Ķ��� �������ݶ���.
        // Encrypt    // ����ļ���������
        long        Info;       // ��ѡ. ��¼�ĵ���Ϣ�Ķ���ID.   ������Ϣ�ȵ�
        char      * ID;         // ��ѡ�� Ϊ�˱�֤�ļ���Ψһ��, ǿ�ҽ����и�����. 
        long        Prev;       // ǰһ��Trailer�ĵ�ַ.
        long        xrefStm;    // ���õ�xref��ַ
        int         retcode;    // �������ӵ�����, �����жϸ�Trailer�Ƿ���Ч
    } TRAILER;

    XREF * getXREF( FILEMAP * fm_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p );
    
    void print_trailer( TRAILER * trailer_p );


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
        FILEMAP       * fm_p;
        long            xrefpos;                // xref ��ʼλ��

        POSMAP      *   posmap_p = NULL;        //  ����, ��Ϊ�޷�ȷ��ʵ���м���xref, ������¼�Ѿ���������xref, ��ֹ������ѭ��
        XREF        *   xref_p = NULL;          //  �����Ч��xref ��Ϣ.
        TRAILER     *   trailer_p = NULL;       //  ��� ��Ч��trailer��Ϣ

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        xrefpos   = getStartXrefPos( fm_p );          // ��ȡstartxref ������, Ҳ���ǵ�һ��xref ��λ��

        if ( xrefpos < 0 )
            return -1; 
       
        // posmap_p ��ʼ��
        posmap_p = (POSMAP *) malloc( sizeof(POSMAP) );
        memset( posmap_p, 0, sizeof(POSMAP) );
        
        xref_p = (XREF *)getXREF( fm_p, xrefpos, posmap_p, trailer_p );

        print_trailer( trailer_p ); 

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
    TRAILER * getTrailer( char * buf )
    {
        TRAILER     *   trailer_p;
        char        *   buf;
        char        *   item;
        int             len;

        if ( !buf )             // �ݴ�����, ��ָ�벻����
            return NULL;

        trailer_p = ( TRAILER * )malloc( sizeof(TRAILER) );
        
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

            if ( memcmp( item, "ID", 2 ) == 0 ) {  // 4. ID ��Ŀ ID[<BE239411BFD7D4D9318C29408037556><5CA7DFC3C76C2F42985CAE05DBD580F9>]
                item = (char *)strtok( NULL, "/ " );          
                printf("ID ��ֵΪ:%s\n", item );

                len =  strlen( item ) - 2;          // -2 �ǲ�����"ID"
                
                trailer_p->ID = (char *)malloc( len +1 );      
                memset( trailer_p->ID, 0, len + 1 );
                memcpy( trailer_p->ID, item, len );
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
        return trailer_p;
    }

    // �ͷ�trailerָ���ڴ�
    void  freeTrailer( TRAILER * trailer_p )
    {
        if ( !trailer_p )           // ��ָ�벻���ͷ��ڴ�
            return;

        if ( trailer_p->ID )
            free( trailer_p->ID );

        memset( trailer_p, 0, sizeof(TRAILER));

        free( trailer_p );
        
        return;
    }

    // �����ú���
    void print_trailer( TRAILER * trailer_p )
    {
        if ( !trailer_p ) {
            printf( "\n������trailer_p �ǿ�ָ�룡\n" );
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
    //  ifXrefPosProcessed()  ���ڲ�����, �����ж�xrefpos��ַ��Ӧ��xref �Ƿ��Ѿ���������, ��ֹѭ������ͬһ��xref�����ѭ��
    bool    ifXrefPosProcessed( long xrefpos, POSMAP * posmap_p )
    {   
        POSMAP  *   tmp_p = posmap_p;   // Ϊ�˲��ƻ�posmap_p ���ݣ� ��ʱָ�����������

        printf(" --- ifXrefPosProcessed(), xrefpos=%d -\n", xrefpos);

        if ( posmap_p == NULL )       // ����λ����Ϣ����������
            return false;
        
        while( tmp_p != NULL ) {
            printf( "%d\n", tmp_p->xrefpos );
            if ( xrefpos == tmp_p->xrefpos )    // ������ַ�������У� ��ʾ�Ѿ���������
                return true;
            tmp_p = (POSMAP *)tmp_p->next;
        }

        printf(" --- ifXrefPosProcessed()  end  -\n");

        // �������������Ҳû���ҵ�, ˵���õ�ַû����������, ����Ӧ��xrefû�д�����
        return false;

    }

    void freeXREF( XREF * xref_p )
    {
        if ( !xref_p )    // ��ָ�벻���ͷ� 
            return;

        if (  xref_p->objpos_p )
            free( xref_p->objpos_p ); 

        free( xref_p );
    }

    // XREF ��������һ�еĴ���, �ж��Ƿ��Ѿ��������õ�ַָ���xref(�ݴ�����, ��ֹ��ѭ��)
    int procFirstLine( FILEMAP *fm_p, long xrefpos, XREF * xref_p, POSMAP * posmap_p )
    {
        char        * buf;
        
        printf(" --- procFirstLine() -\n");


        xref_p = (XREF *)malloc( sizeof( XREF ) );
        xref_p->retcode = -1;

        printf(" ---111111111111111111 -\n");
        if( ifXrefPosProcessed( xrefpos, posmap_p ) )   // ����õ�ַ�Ѿ�������,  ��ʾ�ļ���ʽ������, ֱ�ӷ���xref_p (retcode=-1)
            return -1;
        
        printf(" --- 222222222222222222222222------------ -\n");
        // �õ�ַû�д�����, �Ѹõ�ַ���ӵ�����β��
        posmap_p->next = (POSMAP *)malloc( sizeof(POSMAP) );
        posmap_p->next->xrefpos = xrefpos;

        fm_seek( fm_p, xrefpos );

        buf = fm_readLine( fm_p );
        printf("XREF ��һ��: buf=%s\n", buf);

        if ( !buf || !strstr( buf, "xref" ) ) {     // ��һ��û��"xref", ��ʽ����
            xref_p->retcode = -40421;
            if ( buf )
                free( buf );
            return -1;
        }

        if ( buf )          // ���� fm_readLine() �ڲ��������ڴ�, �����Ҫ���ͷ�֮ǰ������ڴ�
            free( buf );

        printf(" --- procFirstLine() End -\n");
        return 0;
    }

    // XREF �������ڶ��еĴ���, ��ȡ ��������, ��һ��������
    // ������:  -1  ��ʾ����,  0 ��ʾ��ȡ�ɹ�
    int procSecondLine( FILEMAP * fm_p, int   * firstObj, int * objSum, TRAILER * trailer_p ) 
    {
        char    * buf;
        char    * item;

        printf(" --- procSecondLine() -\n");
        
        buf = fm_readLine( fm_p);

        if ( !buf )         // ��ȡ���ݳ���
            return -1;

        printf( "buf=%s\n", buf);

        // �������trailer, ��ʾ��Ҫ������trailerѰ����һ��xrefpos, Ȼ���ٽ���xref
        if ( strstr( buf, "trailer" ) ) {     // ����trailer 
            freeTrailer( trailer_p );           // ��ȡ trailer֮ǰ���ͷ��ڴ�, ��ֹ֮ǰ�Ѿ�������ڴ�
            trailer_p = getTrailer( buf );
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

    // ����objSum < 1 �����.  �ݴ�����
    void procObjSumLT1( FILEMAP *fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p )
    {
        char    * buf;

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

        freeTrailer( trailer_p );         // ���� getTrailer() �ڲ��������ڴ�, �������Ҫ�ͷ�֮ǰ����������ڴ�
        trailer_p = getTrailer( buf );

        if ( trailer_p->retcode < 0 || !strstr( buf, "Prev" ) || trailer_p->Prev <= 0 ) { 
            // ��Ч��xref֮���trailer��Ч��û�а�����Ч��self.xref ��ַ, ���������
            free( buf );
            freeTrailer( trailer_p );
            return xref_p;
        }

        if ( buf )
            free( buf );

        freeXREF( xref_p );     // Ƕ�׵���ǰ�����ͷ��ڴ�, Ƕ�׵�����������������ڴ�.
        xref_p = (XREF *)getXREF( fm_p, trailer_p->Prev, posmap_p, trailer_p );
    }

    // ��ȡÿ�������λ����Ϣ, 
    // ע��:
    //      �����Ŵ�1��ʼ, ��һ����: 0000000000 65535 f   ����ʵ�ʶ���, ���Ƕ�����Ϣ˵����ʾ�����65535������
    long * getOBJPOS( FILEMAP * fm_p, int objSum )
    {
        char        *   buf;
        char        *   item;
        long        *   objpos_p;

        printf("--in getOBJPOS() objSum=%d\n", objSum);

        objpos_p = ( long * )malloc( sizeof(long) * objSum );       // ָ��һ��long����, ����δobjSum

        // ֻ�� ������ < ��������  �Ž��д���
        for ( int i =0; i < objSum; i ++ ) {
            buf = fm_readLine( fm_p );      // ��ȡ��Ӧ���� 0000000016 00000 n���Ƹ�ʽ

            printf("\nbuf=%s\n", buf);

            item = (char *)strtok( buf, " " );          // ��ʵֻҪ��ȡ��һ��Ϳ���, ��һ���ǵ�ַ�� ȫ����ȡ��Ϊ���ݴ�
            int   j = 0;
            while ( item ) {
                printf("item=%s\n", item);
                if ( j == 0 || j == 1 ) {           // ��һ,����Ӧ��������, �ݴ�����
                    if ( !IsNumberStr( item ) )  {
                        printf("-------------------------23------------------------\n");
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
                printf("----s--j=%d------\n", j);
                free( buf );
                return NULL;
            }

            free( buf );            // �ͷ��ڴ�, ���滹��Ҫ��ʹ��bufָ��
        }

        for ( int i = 0; i < objSum; i ++ ){
            printf("%d: [%016d]\n",i,objpos_p[i]);
        }

        return objpos_p;
    }

    TRAILER * procTrailer( FILEMAP * fm_p, XREF * xref_p, POSMAP *posmap_p, TRAILER * trailer_p )
    {
        char     *  buf;
        TRAILER *   trailer_p2;
        
        buf = fm_readLine( fm_p );
        
        if ( !buf ){
            printf("\nERROR!!!\n");
            return;
        }

        if ( !strstr( buf, "Root" ) ) { // ���trailer.<<...>>��ʽ�Ļ�, trailer������ʵ��������һ��
            free( buf );
            buf = fm_readLine( fm_p );
        }
        
        trailer_p2 = getTrailer( buf );
        if( trailer_p2->retcode < 0 )
            return;

        if ( strstr( buf, "Prev" ) ) {
            printf("\n����Ƕ�׻�ȡxref\n");

            free( xref_p );
            xref_p = getXREF( fm_p, trailer_p->Prev, posmap_p, trailer_p );
            return NULL;
        }
        
        return trailer_p2;
    }


    XREF * getXREF( FILEMAP * fm_p, long xrefpos, POSMAP *posmap_p, TRAILER *trailer_p )
    {
        XREF    *   xref_p;
        int         firstObj;       // xref �е�һ�м�¼�ĵ�һ��������
        int         objSum;         //                    ����������
        int         ret;
                
        if ( fm_p == NULL )
            return NULL;

        printf("--in getXREF()\n");

        // 1. ��һ�еĴ���
        ret = procFirstLine( fm_p, xrefpos, xref_p, posmap_p );
        if ( ret != 0 ) {
            freeXREF( xref_p );
            return NULL;
        }

        printf("------------��ʼ�ڶ��еĴ���\n");
        // 2. �ڶ��еĴ���
        ret = procSecondLine( fm_p, &firstObj, &objSum, trailer_p );       // Ϊ�����ӿɶ���, ����һ�еĴ�����װ��һ��������
        if ( ret < 0 )                     // ������������0 ��ʾ������,ֱ�ӷ���xref_p,  xref_p->retcode =-1(��ʼ��deֵ), Ҳ���Ǳ�ʾ������
            return xref_p;
        if ( ret == 1 ) {               // �������1, ��ʾ�ҵ�trailer��, ˵����Ҫ��������һ��xref, Ƕ�׵���getXREF(), ����Ĵ�������
            if ( !trailer_p || trailer_p->retcode < 0 ) {            // ���trailer������, �����
                freeXREF( xref_p );
                freeTrailer( trailer_p );
                return NULL;
            }
            freeXREF( xref_p );
            return getXREF( fm_p, trailer_p->Prev, posmap_p, trailer_p );
        }

        // 3. objSum < 1 ������ݴ������� ��������п���, doc�ļ�ת��pdf��, ��һ��xref ����0 0, 
        // �����trailerָ���µ�xref, ���Ӧ�ü�������trailer�� ���û��Prev �ʹ���      
        if ( objSum < 1 ) {
            procObjSumLT1( fm_p, xref_p, posmap_p, trailer_p );
            return xref_p;
        }

        xref_p->objTotal = objSum;
        // 4. �����xref����������ÿ�������λ����Ϣ, �����Ŵ�1��ʼ
        xref_p->objpos_p = getOBJPOS( fm_p, objSum );
        if ( xref_p->objpos_p == NULL ) {
            printf("λ����Ϣû���أ�\n");
            xref_p->retcode = -40425;       // ����λ����Ϣ ��ʽ����
            return xref_p;
        }

        for ( int i=0; i < xref_p->objTotal; i ++ ) {
            printf("xref_p->objpos_p[%d]=%016d\n",i, xref_p->objpos_p[i] );
        }

        // 5. ��ȡ�� ����λ�����ݺ�, ����trailer ��������
        freeTrailer( trailer_p );
        trailer_p = procTrailer( fm_p, xref_p, posmap_p, trailer_p );

        print_trailer(trailer_p);

        if ( !trailer_p )
            printf("û����Ч����trailer\n");
        

        return xref_p;
        
    }
