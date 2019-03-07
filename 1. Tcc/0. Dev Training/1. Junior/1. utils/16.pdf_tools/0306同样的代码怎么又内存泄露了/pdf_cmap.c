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
    //      8.  getCmapList() ---- getObjContent(),isType0()
    //      9.  isType0()
    //      10. getCMAPs() -----getObjContent(), getItemOfObj(),procType0Stream()
    //      12. procType0Stream() ---  getItemOfObj(), decompress()
    //      14. processPage() ---- getItemOfObj()
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


    int     getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getFontMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    int     getCMAPS( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
    
    

    #define CHECK_ERR(err, msg) { \
        if (err != Z_OK) { \
            fprintf(stderr, "%s error: %d\n", msg, err); \
            exit(1); \
        } \
    }
    #define L_TMPBUF        256
        

    // ----------- ������ CMAP  ��ش���
    //

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
                pages_p->c_objlist_p[i][1] = atoi( (char *)strsplit( buf, " ", 2 ) );       // buf �ڶ���������ݶ���

                pages_p->comprRatio_p[i] = (float *)calloc( sizeof(float ) * (count + 1), 1 );
                //memset( pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

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
                    
                    tmpbuf = (char *)strsplit1( item, ' ', 1 );
                    memcpy( pages_p->fontmap_p[i][n].fontname, tmpbuf, strlen(tmpbuf) );
                    free( tmpbuf );
                    
                    tmpbuf = (char *)strsplit1( item, ' ', 2 );
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
                tmpbuf =  (char *)strsplit1( buf, ' ', 1 );
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
                tmpbuf =  (char *)strsplit1( buf, ' ', 1 );
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
                tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                for( int i = 0; i < mapsum; i ++ ) {
                    free( buf );
                    buf = mm_readLine( mm_p );              // <1440> <5E10> 
                    DelCharsInString( buf, "<>" );          // ɾ�� ������  "1440 5E10" 
                    
                    tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                    cmap_p->code_p[n].code = Hex2Int2( tmpbuf );          // 0x1440=
                    free(tmpbuf);

                    tmpbuf = (char *)strsplit1( buf, ' ', 2 );
                    cmap_p->code_p[n].decode = Hex2Int2( tmpbuf );        // 0x5E10=
                    free( tmpbuf );

                    n ++;
                }
                free( buf );
                continue;
            }else if ( strstr( buf, "beginbfrange" ) ) {   // ��Χ����ӳ��  2 beginbfrange
                tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                //printf("beginbfrange sum=%d\n", mapsum);
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // ���� mapsum �м���
                    free( buf );
                    buf = mm_readLine( mm_p );              // <01C3> <01C4> <3001>
                    DelCharsInString( buf, "<>" );          // ɾ�� ������  "01C3 01C4 3001"

                    tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                    start1 = Hex2Int2( tmpbuf );          //  0x01C3 =
                    free(tmpbuf);

                    tmpbuf = (char *)strsplit1( buf, ' ', 2 );
                    end1 = Hex2Int2( tmpbuf );          //  0x01C4 =
                    free(tmpbuf);

                    tmpbuf = (char *)strsplit1( buf, ' ', 3 );
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



