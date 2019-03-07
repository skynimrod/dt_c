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
    //      5.  getPages()  --- getObjContent(), getPageleaf()
    //      6.  getObjContent()
    //      7.  getPageleaf()
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

    int     getPages( FILEMAP * fm_p, XREF * xref_p,  PAGES *pages_p, int Root );
    void    cutTail( char * buf );

    int     getContentMap( FILEMAP *fm_p, XREF *xref_p, PAGES *pages_p );
        
    //����Ϊ�ڲ����õ�API
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

    // getPageleaf() ��������ȡ���е�Ҷ��ҳ�����, Ҳ���Ǵ���Content�Ķ�����Ϣ, �ö���Ҳ����ҳ�����ݶ���
    // ÿ��ҳ�����ʵ����Ҳ���Ƕ�Ӧһ��ҳ�������, leafs �Ǹ�long ������
    // cursor ���������leafs_p�����һ����ЧҶ�ӵ��±��, �µ�Ҷ��Ҫ��֮ǰ���е�Ҷ�ӱȽϣ�ȷ�����ظ��Ż����
    int getPageleaf( FILEMAP * fm_p, XREF * xref_p, int obj, int *leafs_p, int *cursor )
    {
        char        * buf;
        char        * item;
        int         kidscount = 0;  // ��ʼ��Ϊ0
        int         * objlist;
        int         ret;


        // # 5251||<</Count 100/Kids[5252 0 R 5253 0 R 5254 0 R 5255 0 R 5256 0 R 5257 0 R 5258 0 R 5259 0 R 5260 0 R 5261 0 R]/Parent 5250 0 R/Type/Pages>>
        buf = getObjContent( fm_p, xref_p, obj );
        
        if ( strstr( buf, "Kids" ) ) {      // �����"Kids", ˵������Ҷ��ҳ��, ��Ҫ����Ƕ�ױ���kidsҳ��, ֱ���ҵ����յ�����ҳ��
            item = (char *) strtok( buf, "/" );
            while( item ) {
                if ( strstr( item, "Count" ) ) {    // kids ����  //Count 100
                    kidscount = atoi( (char *)strsplit( item, " ", 2 ) );
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
            free( objlist );
            free( buf );

        } else if( strstr( buf, "Contents" ) ){ //ֻ�к��� Contents �Ķ������������Ҷ�Ӷ���
            //printf("Ҷ�Ӷ���:%s\n", buf);
            
            for ( int i=0; i < *cursor; i ++ ) {
                if ( leafs_p[i] == obj ) {   // �ݴ�.����ö����Ѿ������� leafs_p, ��ʽ�����⣬һ��Ҷ��ҳ����󲻿��ܷ����ڶ��ҳ��    
                    free(buf);
                    return -1;
                }
            } 
            leafs_p[*cursor] = obj;
            //printf("�ҵ�һ��Ҷ�Ӷ���%d(��%dҳ)\n", obj, *cursor+1 );
            *cursor += 1;

            free( buf );
        }

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

        buf = (char *)getItemOfObj( fm_p, xref_p, Root, "Pages" );
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
                pages_p->leafs_p = (int *)calloc( sizeof(int)* pages_p->total + 1, 1 );
                //memset( pages_p->leafs_p, 0, sizeof(int)* pages_p->total + 1 );
                
                ret = getObjList( item, pages_p->total, pages_p->leafs_p );   // ע�� ���pages_p->leafs_p ���������յ�ҳ��Ҷ�Ӷ���
            }
            item = (char *)strtok( NULL, "/" );
        }

        free( buf );

        //printPages( pages_p );
        for ( int j = 0; j < pages_p->total; j ++ ) {
            ret = getPageleaf( fm_p, xref_p, pages_p->leafs_p[j], pages_p->leafs_p, &pages_p->cursor );
            if ( ret < 0 ) { 
                return ret;
            }
        }
        //printPages( pages_p );

        return ret;
    }


