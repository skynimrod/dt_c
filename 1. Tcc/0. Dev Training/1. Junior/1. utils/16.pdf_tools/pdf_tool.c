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
    // 下面的仅供本文件内部使用
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
    #include "sheet.h"


    char *  getItemOfObj( FILEMAP * fm_p, XREF *xref_p, int obj, char *item );
    char *  getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo );
    void    writeRawData( char * filename, uchar *bytes, int len );
    
    char *  strsplit2( char * buf, int len, char ch, int n ) ;
    char *  strsplit1( char * buf, char ch, int n );

    
    // 下面的函数为测试函数, 用来把相关信息输出到文件中, 便于查阅
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );
    void    printPAGESContent( FILEMAP * fm_p, XREF * xref_p, PAGES *pages_p, char * filename );
    void    printXREFContent( FILEMAP * fm_p, XREF * xref_p, char * filename );

    void    printTM( TM * tm_p );
    void    printCUR_XY( CUR_XY * cur_xy_p );

    void    printTextMap( TEXT * textMap_p );
    void    printCellMap( CELL * cellMap_p );
    
    void    printColMap( COL * colMap_p );



    #define L_TMPBUF        256

    // 调试用函数
    void print_trailer( TRAILER * trailer_p )
    {
        if ( !trailer_p ) {
            printf( "\n给定的trailer_p 是空指针！\n" );
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

    // ------------获取所有的页 对应的对象的信息
    // 获取指定对象中的指定item的内容
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
            if ( strstr( tmpbuf, item ) ) {     // 找到了包含item 的项目 
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

    /* 根据对象编号获取对象的内容, 使用内存映射文件 
     * 入口参数:
     *      obj     是整数字符串 ， 对应的是对象编号
     *      self.xref    self.xref哈希表， 存放的是 对象编号:对象物理位置
     * 返回:
     *      对象的文字描述内容部分.
     * 说明:
     *      对象的描述部分， 第一行是 3 0 R , 也就是对象编号等信息, 直接过滤即可. 
     *      第二行才是对象的描述性文本内容.
     * 2016.12.19:
     *      修正, 对于有 \xae 类似的内容, 需要额外处理.
     * 调用的外部方法有:
     *      1. seek() ------------------>util_fileService
     *      2. readLine() -------------->util_fileService
     * 调用的类对象内部的方法有:
     *      1. getObjPosInself.xref()
     */
    char    * getObjContent( FILEMAP * fm_p, XREF *xref_p, int  objNo )
    {
        char    * buf;

        if ( !fm_p || !xref_p || objNo <= 0 )       // 无效参数
            return NULL;

        fm_seek( fm_p, xref_p->objpos_p[objNo] );       //由于数组下标是从0开始， 对象1对应的下标是0
        
        buf = fm_readLine( fm_p );      // 第一行是  3 0 R  也就是对象编号
        if ( !buf ) 
            return NULL;

        free( buf );
        
        buf = fm_readLine( fm_p );      // 第二行才是对象对应的数据
        if ( !buf )
            return NULL;
       
        return buf;
    }
    // 下面是页面相关信息的处理i

    void printPages( PAGES * pages_p )
    {
        if ( pages_p == NULL ) {
            printf("\npages_p 是空指针！\n");
            return ;
        }

        printf( "pages_p->total = %d\n", pages_p->total );

        for ( int i = 0; i < pages_p->total; i ++ )
            printf("pages_p->leafs_p[%d]=%d\n", i, pages_p->leafs_p[i] ); 
    }

    // 获取拆分后的第几个项, 放弃使用strtok， 因为外部有可能会使用
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

    // 下面的函数为测试函数, 用来把相关信息输出到文件中, 便于查阅
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


    // ----------- 下面是 CMAP  相关处理
    //
    // writeRawData()
    //     调试函数, 把stream中的字节流写入到文件中, 便于查看
    void writeRawData( char * filename, uchar *bytes, int len )
    {
        FILE        *   fp;
        size_t          l;

        if ( !( fp=fopen( filename, "wb") )) 
            return;

        l = fwrite( bytes, len, 1, fp );
        printf("\n写成功次数:l=%d, 应该写入 len=%d\n", l, len);
        fclose( fp );
        
        return ;
    }

    // 计算指定字符在字符串中出现的次数
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
        int     k =0;       // 记录每一项的开始位置

        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // 找到第几个项的位置了
                    retbuf = (char *)malloc( i-k+1);
                    memset( retbuf, 0, i-k+1);   
                    memcpy( retbuf, buf+k, i-k );
                    return retbuf;
                }
                while( buf[i] == ch )       // 跳过连续的分隔符
                    i ++;
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // 最后一项
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
        int     k =0;       // 记录每一项的开始位置

        if ( !des )
            return NULL;
        
        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                if ( m == n ) {     // 找到第几个项的位置了
                    memset( des, 0, len);   
                    memcpy( des, buf+k, i-k );
                    printf( "des=%s\n", des );
                    return des;
                }
                while( buf[i] == ch )       // 跳过连续的分隔符
                    i ++;
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // 最后一项
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
            printf("------------------cellMap 是 空的?-----------------------------------------\n");
            return;
        }

        while ( cp ) {
            printf( "-------------cell id=%d--address=%p---------\n", cp->id, cp );
            printf( "x=%.2f,y=%.2f,w=%.2f, h=%.2f\n", cp->x, cp->y, cp->w, cp->h );
            printf( "row=%d, col=%d\n",  cp->row, cp->col );
            printf( "maxlines=%d, maxlen=%d, txtTotal=%d, table_id=%d\n",
                       cp->maxlines, cp->maxlen, cp->txtTotal, cp->table_id );
            if ( cp->tps )         // 因为第一次创建cell的时候, 还没有为txtIDs_p申请空间, 当时不知道有多少文本在cell中, 后来处理完文本才会知道
                for ( int i = 0; i < cp->txtTotal; i ++ ) {
                    printf( "%d ", cp->tps[i]->id ); 
                }

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

    void printColMap( COL * colMap_p )
    {
        COL     * col_p;

        if ( !colMap_p )
            return ;    

        printf("--------printColMap()----------------------------------------------------------\n");
        col_p = colMap_p;

        while ( col_p ) {
            printf( " -----------------col id=%d-----(%p)---------------\n", col_p->id, col_p );
            printf( "x=%.2f, w=%.2f, maxlen = %d\n", col_p->x, col_p->w, col_p->maxlen );
            printf( "cellTotal = %d, sonTotal=%d, table_id=%d\n",
                            col_p->cellTotal, col_p->sonTotal, col_p->table_id );
            printf("col_p->prev=%p, col_p->next=%p\n", col_p->prev, col_p->next );
            if ( col_p->c_list ) {
                printf("cells(%p):", col_p->c_list);
                for( int i = 0; i < col_p->cellTotal; i ++ ) {
                    if ( col_p->c_list[i] )
                        printf( " %d", col_p->c_list[i]->id );
                    else
                        printf( " 0" );         //  有可能没有填充
                }
                printf("\n");
            }
            if ( col_p->son ) {
                printf("sons:");
                for( int i = 0; i < col_p->sonTotal; i ++ ) {
                    printf( " %d", col_p->son[i]->id );
                }
                printf("\n");
            }

            if ( col_p->parent ) 
                printf("parent:%d(%p)\n", col_p->parent->id, col_p->parent );

            col_p = col_p->next;
        }
    }

    void printRowMap( ROW * rowMap_p )
    {
        ROW     * row_p;

        if ( !rowMap_p )
            return ;    

        printf("--------printRowMap()----------------------------------------------------------\n");
        row_p = rowMap_p;

        while ( row_p ) {
            printf( " -----------------row id=%d--(%p)------------------\n", row_p->id, row_p );
            printf( "y=%.2f, h=%.2f, maxlines = %d\n", row_p->y, row_p->h, row_p->maxlines );
            printf( "cellTotal = %d, sonTotal=%d, table_id=%d\n",
                            row_p->cellTotal, row_p->sonTotal, row_p->table_id );
            printf("row_p->prev=%p, row_p->next=%p\n", row_p->prev, row_p->next );
            if ( row_p->c_list ) {
                printf("cells:");
                for( int i = 0; i < row_p->cellTotal; i ++ ) {
                    if ( row_p->c_list[i] )
                        printf( " %d", row_p->c_list[i]->id );
                    else
                        printf( " 0" );         //  有可能没有填充
                }
                printf("\n");
            }
            if ( row_p->son ) {
                printf("sons:");
                for( int i = 0; i < row_p->sonTotal; i ++ ) {
                    printf( " %d", row_p->son[i]->id );
                }
                printf("\n");
            }
            if ( row_p->parent ) 
                printf("parent:%d(%p)\n", row_p->parent->id, row_p->parent );

            if ( row_p->splitpos ) {
                printf("splitpos:");
                for ( int i = 0; i <= row_p->cellTotal; i ++ ) {
                    printf(" %d", row_p->splitpos[i] );
                }
                printf("\n");
            }
                
            row_p = row_p->next;
        }
    }

    void printTableMap( TABLE * tableMap_p )
    {
        TABLE     * table_p;

        if ( !tableMap_p )
            return ;    

        printf("--------printTableMap()----------------------------------------------------------\n");
        table_p = tableMap_p;

        while ( table_p ) {
            printf( " -----------------table id=%d--(%p)------------------\n", table_p->id, table_p );
            if ( table_p->l_tp )        // 加判断是因为  初始化table 的时候该指针是空的
                    printf("last tp id=%d, tp buf=%s\n", table_p->l_tp->id, table_p->l_tp->buf );
            printf( "cellTotal=%d, colTotal=%d, rowTotal=%d\n", table_p->cellTotal, table_p->colTotal, table_p->rowTotal );
            printf("table_p->prev=%p, table_p->next=%p\n", table_p->prev, table_p->next );
            if ( table_p->col_list ) {
                printf("Cols:");
                for( int i = 0; i < table_p->colTotal; i ++ ) {
                    if ( table_p->col_list[i] )
                        printf( " %d", table_p->col_list[i]->id );
                    else
                        printf( " 0" );         //  有可能没有填充
                }
                printf("\n");
            }
            if ( table_p->row_list ) {
                printf("Rows:");
                for( int i = 0; i < table_p->rowTotal; i ++ ) {
                    if ( table_p->row_list[i] )
                        printf( " %d", table_p->row_list[i]->id );
                    else
                        printf( " 0" );         //  有可能没有填充
                }
                printf("\n");
            }
            /*
              */  
            table_p = table_p->next;
        }
    }


