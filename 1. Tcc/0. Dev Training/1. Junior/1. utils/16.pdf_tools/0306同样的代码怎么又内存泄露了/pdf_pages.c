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
        
    //以下为内部调用的API
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

    // getPageleaf() 是用来获取所有的叶子页面对象, 也就是带有Content的对象信息, 该对象也就是页面数据对象
    // 每个页面对象实际上也就是对应一个页面的数据, leafs 是个long 型数组
    // cursor 是用来标记leafs_p的最后一个有效叶子的下标的, 新的叶子要和之前所有的叶子比较，确认无重复才会加入
    int getPageleaf( FILEMAP * fm_p, XREF * xref_p, int obj, int *leafs_p, int *cursor )
    {
        char        * buf;
        char        * item;
        int         kidscount = 0;  // 初始化为0
        int         * objlist;
        int         ret;


        // # 5251||<</Count 100/Kids[5252 0 R 5253 0 R 5254 0 R 5255 0 R 5256 0 R 5257 0 R 5258 0 R 5259 0 R 5260 0 R 5261 0 R]/Parent 5250 0 R/Type/Pages>>
        buf = getObjContent( fm_p, xref_p, obj );
        
        if ( strstr( buf, "Kids" ) ) {      // 如果有"Kids", 说明不是叶子页面, 需要继续嵌套遍历kids页面, 直到找到最终的内容页面
            item = (char *) strtok( buf, "/" );
            while( item ) {
                if ( strstr( item, "Count" ) ) {    // kids 数量  //Count 100
                    kidscount = atoi( (char *)strsplit( item, " ", 2 ) );
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
            free( objlist );
            free( buf );

        } else if( strstr( buf, "Contents" ) ){ //只有含有 Contents 的对象才是真正的叶子对象
            //printf("叶子对象:%s\n", buf);
            
            for ( int i=0; i < *cursor; i ++ ) {
                if ( leafs_p[i] == obj ) {   // 容错.如果该对象已经存在于 leafs_p, 格式有问题，一个叶子页面对象不可能分属于多个页面    
                    free(buf);
                    return -1;
                }
            } 
            leafs_p[*cursor] = obj;
            //printf("找到一个叶子对象%d(第%d页)\n", obj, *cursor+1 );
            *cursor += 1;

            free( buf );
        }

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

        buf = (char *)getItemOfObj( fm_p, xref_p, Root, "Pages" );
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
                pages_p->leafs_p = (int *)calloc( sizeof(int)* pages_p->total + 1, 1 );
                //memset( pages_p->leafs_p, 0, sizeof(int)* pages_p->total + 1 );
                
                ret = getObjList( item, pages_p->total, pages_p->leafs_p );   // 注意 这儿pages_p->leafs_p 还不是最终的页面叶子对象
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


