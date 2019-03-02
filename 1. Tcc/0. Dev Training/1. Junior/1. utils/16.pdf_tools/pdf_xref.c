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
        uchar     * buf;
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
            free( buf );
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
        tmpbuf = (char *)strsplit1( buf, ' ', 2 );
        *objSum = atoi( tmpbuf );     // ��ʼ������
        free( tmpbuf);

        tmpbuf = (char *)strsplit1( buf, ' ', 1 );
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
    
        if ( objSum == 1 ) {            // 1 1 �ĵ�һ���Թ�
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
        if ( ret < 0 )                     // ����ֵ������0 ��ʾ������,ֱ�ӷ���xref_p,  xref_p->retcode =-1(��ʼ��deֵ), Ҳ���Ǳ�ʾ������
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



