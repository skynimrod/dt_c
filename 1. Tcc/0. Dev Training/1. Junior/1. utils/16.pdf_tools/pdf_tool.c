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
    //
    // ����Ľ������ļ��ڲ�ʹ��
    //      6.  getObjContent()
    //      11. getItemOfObj() -- getObjContent
    //      
    //  
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "pdf_tools.h"


    char *  getItemOfObj( FILEMAP * fm_p, XREF *xref_p, int obj, char *item );
    char *  getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );
    void    writeRawData( char * filename, uchar *bytes, int len );
    
    char *  strsplit2( char * buf, int len, char ch, int n ) ;
    char *  strsplit1( char * buf, char ch, int n );

    
    // ����ĺ���Ϊ���Ժ���, �����������Ϣ������ļ���, ���ڲ���
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

    void    printTM( TM * tm_p );
    void    printCUR_XY( CUR_XY * cur_xy_p );

    void    printTextMap( TEXT * textMap_p );
    void    printCellMap( CELL * cellMap_p );


    #define L_TMPBUF        256

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
                value = ( char * )calloc( len + 1, 1 );
                //memset( value, 0, len + 1 );
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
     *      self.xref    self.xref��ϣ���� ��ŵ��� ������:��������λ��
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

