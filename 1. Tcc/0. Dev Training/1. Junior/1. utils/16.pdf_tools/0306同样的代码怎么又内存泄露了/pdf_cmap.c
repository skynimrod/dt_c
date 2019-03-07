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
        

    // ----------- 下面是 CMAP  相关处理
    //

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
                pages_p->c_objlist_p[i][1] = atoi( (char *)strsplit( buf, " ", 2 ) );       // buf 第二项就是内容对象

                pages_p->comprRatio_p[i] = (float *)calloc( sizeof(float ) * (count + 1), 1 );
                //memset( pages_p->comprRatio_p[i], 0, sizeof(float ) * (count + 1) );

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
                    
                    tmpbuf = (char *)strsplit1( item, ' ', 1 );
                    memcpy( pages_p->fontmap_p[i][n].fontname, tmpbuf, strlen(tmpbuf) );
                    free( tmpbuf );
                    
                    tmpbuf = (char *)strsplit1( item, ' ', 2 );
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
                tmpbuf =  (char *)strsplit1( buf, ' ', 1 );
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
                tmpbuf =  (char *)strsplit1( buf, ' ', 1 );
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
                tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                for( int i = 0; i < mapsum; i ++ ) {
                    free( buf );
                    buf = mm_readLine( mm_p );              // <1440> <5E10> 
                    DelCharsInString( buf, "<>" );          // 删除 尖括号  "1440 5E10" 
                    
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
            }else if ( strstr( buf, "beginbfrange" ) ) {   // 范围编码映射  2 beginbfrange
                tmpbuf = (char *)strsplit1( buf, ' ', 1 );
                mapsum = atoi( tmpbuf );
                free( tmpbuf );
                //printf("beginbfrange sum=%d\n", mapsum);
                total += mapsum;
                for ( int i =0; i < mapsum; i ++ ) {        // 跳过 mapsum 行即可
                    free( buf );
                    buf = mm_readLine( mm_p );              // <01C3> <01C4> <3001>
                    DelCharsInString( buf, "<>" );          // 删除 尖括号  "01C3 01C4 3001"

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



