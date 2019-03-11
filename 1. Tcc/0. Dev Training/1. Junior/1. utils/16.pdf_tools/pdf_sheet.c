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
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "pdf_tools.h"
    #include "sheet.h"

    char *  procText4Table( TEXT * textMap_p, CELL * cellMap_p );
    void freeSheet( SHEET * sheet_p );
    
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p );
    void preProcTxtMap4Cell( TEXT * textMap_p, CELL * cellMap_p );
    void preProcColRow( CELL *cellMap_p,  SHEET * sheet_p );
    
    ROW * getRowOfID( ROW * rowMap_p, int  id );
    COL * getColOfID( COL * colMap_p, int  id );

    // buildColRowMap(), spaceForColRowMap(), fill_c_list()  仅被 preProcColRow() 调用
    void  buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p );
    void spaceForColRowMap( SHEET * sheet_p );
    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p );
    void fixColMaxlen( CELL *cellMap_p, SHEET * sheet_p );
    void orderColMap( COL ** des, COL * src, int colTotal );

    /*
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    */
    char * procText4Table( TEXT * textMap_p, CELL * cellMap_p )
    {
        SHEET       * sheet_p;          // 表单, 记录表格及相关信息

        sheet_p = (SHEET *)calloc( sizeof(SHEET), 1 );


        // # 1. 处理textMap, 将不在表格中的文本进行处理, 如果是同一行的就进行合并, 同时将cell中的txtIDs_p 补充完整
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
        preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
        //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
        preProcColRow( cellMap_p, sheet_p );
        
        /*       
        // # 4. 解析文本与cell的关系, 拆分出table 出来, 创建tableMap={1:[cellid_list]}， 同时在cellMap 中添加tableid 属性
        tableMap = {}
        self.buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print("--after buildTableMap()---cellIndexMap-----------")
        print(cellIndexMap)
        print(tableMap)

        // # 5. 输出文本. 已经处理过同行多文本的拼接, 不再cell中的文本就是单独一行文本
        retbuf = self.buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print(retbuf)
        
        return retbuf
        */

        //freeSheet( sheet_p );
        
        return NULL;
    }

    void freeCOL( COL * col_p )
    {
        if ( !col_p )
            return;

        if ( col_p->c_list )
            free( col_p->c_list );

        if ( col_p->son )
            free( col_p->son );

        free( col_p );
    }

    void freeROW( ROW * row_p )
    {
        if ( !row_p )
            return;

        if ( row_p->c_list )
            free( row_p->c_list );

        if ( row_p->son )
            free( row_p->son );

        free( row_p );
    }

    void freeTable( TABLE * table_p )
    {
        if ( !table_p )
            return;

        if ( table_p->c_list )
            free( table_p->c_list );

        if ( table_p->col_list )
            free( table_p->col_list );

        if ( table_p->row_list )
            free( table_p->row_list );

        free( table_p );
    }

    void freeSheet( SHEET * sheet_p )
    {
        COL     * col_p;
        ROW     * row_p;
        TABLE   * table_p;
        
        if ( !sheet_p )
            return;
        
        // 1. 释放 列col  相关节点
        if ( sheet_p->colMap_p ) {
            col_p = sheet_p->lcp;           // 从后往前释放col 节点
            while( col_p ) {
                col_p = col_p->prev;        // 先保留好前一个节点的地址( 如果是首节点，则这个值为NULL)
                freeCOL( col_p->next );
            }
        }

        // 2. 释放 行row  相关节点
        if ( sheet_p->rowMap_p ) {
            row_p = sheet_p->lrp;           // 从后往前释放row 节点
            while( row_p ) {
                row_p = row_p->prev;        // 先保留好前一个节点的地址( 如果是首节点，则这个值为NULL)
                freeROW( row_p->next );     // row_p->next 其实就是当前节点, 因为前一句重新赋值了
            }
        }

        // 3. 释放 表格 table  相关节点
        if ( sheet_p->tableMap_p ) {
            table_p = sheet_p->ltp;           // 从后往前释放table 节点
            while( table_p ) {
                table_p = table_p->prev;        // 先保留好前一个节点的地址( 如果是首节点，则这个值为NULL)
                freeTable( table_p->next );
            }
        }
    }
    
    // 删除当前的节点
    void delCurTxt( TEXT * tp )
    {
        TEXT    * tpp, * tpn;    // 当前节点的前一个节点和后一个节点
        
        if ( !tp ) 
            return;
        
        tpp = tp->prev;
        tpn = tp->next;

        free( tp->buf );
        free( tp );

        if ( !tpp ) {       // tpp 为空, 说明要删除的tp是第一个节点, 则后一个节点变成第一个
            tp = tpn;
            tp->prev = NULL;
            return;
        }
        
        if ( !tpn ) {       // 如果tpn为空, 说明要删除的tp 是最后一个节点
            tpp->next = NULL;
            return;
        }

        tpp->next = tpn;
        tpn->prev = tpp;

    }    

    // 合并2个文本节点中的文本内容到第一个文本节点的buf中, 
    // 只是为了简化代码，增加可读性而剥离出来的函数
    void combineBuf( TEXT * tp1, TEXT * tp2 )
    {
        char        * buf;      // 用来申请空间并存放合并后的文本信息

        if ( ! tp1 || !tp2 ) {
            printf("有空指针！！！\n");
            return ;
        }

        //printf("合并前, tp1->buf=%s, tp1->len=%d, tp1->id=%d, tp2->buf=%s, tp2->len=%d, tp2->id=%d\n", 
        //                tp1->buf, tp1->len, tp1->id, tp2->buf, tp2->len, tp2->id );

        // 先为合并后的文本重新申请空间并保存合并后的文本, 然后释放之前的2个文本内存空间
        buf = ( char * )calloc( tp1->len + tp2->len + 1, 1 );   // +1 是为了存放尾0
        memcpy( buf, tp1->buf, tp1->len );
        memcpy( buf+tp1->len, tp2->buf, tp2->len );

        free( tp1->buf );
        
        tp1->buf = buf;
        tp1->len += tp2->len;
        
        printf("合并后的内容为:%s(textID=%d), len=%d\n", tp1->buf,tp1->id, tp1->len);
    }


    // # 1. 预处理textMap, 将不再表格中的文本进行处理, 如果是同一行的就进行合并
    //# 15.3 preProcTxtMap()
    //#     预处理textMap, 仅限于不在单元格里面的文本进行处理,  将原本在一行的拼接在一行, 删除多余的文本编号, 修正链表
    //#     页眉内的文本也进行拼接处理
    //# depends on:
    //#       delTxt( item, textMap, cellIndexMap )
    //# 2017.01.10:
    //#     更改实现方式, 循环按照 oy 坐标从上往下处理
    //# 2019.02.24
    //      对于C语言版本， 增加cellMap_p 的预处理, 也就是为由文本的cell的txtIds_p 申请空间
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点
        CELL        * cp;

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标

        if ( !textMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            //printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID != 0 ) {   // 不等于0, 表示该文本在cell中, 先跳过不处理
                tp = tp->next;
                continue;        
            }
            
            if ( !tp->prev )  {          // 如果没有前一个文本，就不处理, 也就是第一个文本
                tp = tp->next;
                continue;
            }

            tpp = tp->prev;             // 用tpp 是为了提高代码可读性, 不然多级指针看起来费劲

            ox = tp->ox,        oy = tp->oy;
            px = tpp->ox,       py = tpp->oy;

            printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
            if ( abs( py - oy ) < 1 ) {     //  # 两个Y坐标之差小于1视为同一行, 保留前一个节点, 当前节点的内容添加到前一个节点中, 删除当前节点
                combineBuf( tpp, tp );
                delCurTxt( tp );                    // 删除tp 节点
                tp = tpp->next;                     // tp 重新定位到下一个需要处理的节点
            } else {
                printf("不用合并！\n");
                tp = tp->next;
            }
        }     

        // 2. 下面为单元格 的文本编号记录申请空间(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal > 0 )
                cp->txtIDs_p = (int *)calloc( sizeof(int), cp->txtTotal );   // 为存放记录cell中的文本编号申请空间
            cp = cp->next;
        }

        printf("preProcTxtMap() end \n");
    }

    // 从当前节点往前查找指定ID的 TEXT
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

        // 向前遍历完后没找到
        return NULL;
    }

    // 遍历完后没有找到指定id的CELL
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

        // 遍历完后没找到
        return NULL;
    }

    // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息, txtTotal也要修正
    // 处理思路:
    //    1. 遍历一遍所有的text节点, 跳过不在cell中的节点
    //    2. 对于在cell中的文本节点, 找到对应的cell, 然后与该cell中的之前的文本进行比较, 看是否需要合并文本(同一行)
    void preProcTxtMap4Cell( TEXT *textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp, * tpn;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点, tpn 是下一个节点
        CELL        * cp;
        int         cell_id;   
        int         text_id;  

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标

        int         i;                  // 数组下标

        if ( !textMap_p || !cellMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            //printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID == 0 ) {                // 等于0, 表示该文本没有在cell中, 跳过不处理
                tp = tp->next;
                continue;        
            }

            cp = getCellByID( cellMap_p, tp->cellID );
            // 此时, 该文本的id 并没有存放在cell 中的txtIDs_p 数组中, 之前的处理仅仅是申请了足够的存放空间
            // 在这儿便处理便将处理后的文本的id存放到txtIDs_p 数组中
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->txtIDs_p[i] == 0 )         // 碰到结尾了
                    break;
            
            if ( i == 0 ) {                         // 表示该文本是cell 中的第一个文本, 记录在txtIDs_p[0] 中, 不做合并文本处理
                cp->txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {       // 容错处理,表示预申请空间已经存满了, 那么当前的这个文本就不应该在该cell中
                return ;
            } else  {
                text_id = cp->txtIDs_p[i-1];        //在同一个cell中的文本节点的前一个文本节点编号
                tpp = searchForward( tp, text_id );
                if ( !tpp )                         // 容错处理 , 没找到前一个文本节点。。。(这种情况不应该发生)
                    return ;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {         //  # 两个Y坐标小于1视为同一行
                                                    // 合并文本, 保留 tpp, 因为tpp已经记录了在cell中了
                    combineBuf( tpp, tp );
                    if ( tpp->len > cp->maxlen )    // 更新cell中的最大文本长度
                        cp->maxlen = tpp->len;

                    tpn = tp->next;                 // 记录下一个文本节点地址
                    delCurTxt( tp );                // 删除tpp 节点
                    
                    tp = tpn;                       // tp 重新定位到下一个需要处理的节点
                    cp->txtTotal --;                // 修正 cell 中的文本数量总数
                } else {                            // 不需要合并
                    cp->txtIDs_p[i] = tp->id;       // 添加cell中的文本记录
                    tp = tp->next;
                }
            }

        }


        return;
    }

    // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
    //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
    // 遍历所有的cell, 根据其(x,w) 归类 col, 根据 (y,h) 归类 row


    // 实现思路:
    //    遍历2次cellMap_p 中的所有cell, 第一次是创建cell 所属的col, row, 形成链表
    //    第二次遍历 是根据第一次遍历获得的col, row 内的cell 数量, 申请对应的内存, 
    //    然后把cell id 填充到 col, row 内的c_list 数组中.
    // 调用5个局部函数
    //   buildColRowMap(), buildColRowMap(), fill_c_list(), fixColMaxlen()
    void preProcColRow( CELL *cellMap_p,  SHEET * sheet_p )
    {
        if ( !sheet_p || !cellMap_p )
            return;

        // 第一次遍历cellMap, 创建colMap, rowMap, 但是没有c_list, 因为没遍历完之前不知道有多少cell在col或row中, 
        // 为 c_list 无法申请空间
        buildColRowMap( cellMap_p, sheet_p );

        // 为colMap和rowMap中的col, row 申请对应c_list 需要的空间
        spaceForColRowMap( sheet_p );
        
        // 第二次遍历cellMap_p 中的cell, 为 col, row 中的c_list 填充cell id
        fill_c_list( cellMap_p, sheet_p );

        // 修正 Col 中的maxlen, 主要是根据子col 来修正 父 col 的宽度
        fixColMaxlen( cellMap_p, sheet_p );

        //printColMap( sheet_p->colMap_p );
        // 修正 row 中的maxlines, 主要是根据 儿子row 来修正 父 row 的行数
        //fixRows( cellMap, cellIndexMap, rowMap, colIndexMap, rowIndexMap )
        return;
    }

    // 查找指定ID的单元格cell
    COL * findCol( COL * colMap_p, int id )
    {
        COL  * col_p = colMap_p;  // 从头开始遍历

        while ( col_p ) {
            if ( col_p->id == id )     // 找到了
                return col_p;
            
            col_p = col_p->next;
        }

        return NULL;    // 遍历完还没有找到就返回NULL
    }

            
    // 修正 Col 中的maxlen, 主要是根据子col 来修正 父 col 的宽度
    // fixColMaxlen() 仅仅是为了提高可读性而封装的代码
    //     仅被 preProcColRow() 调用
    //#      修正cell的最大长度.
    //# 说明:
    //#      目前的单元格的最大长度是同列的最长字符串长度(汉字算2个).
    //#      有时候该列内有可能有多个子列. 那么多个子列内的最大长度之和有可能会超过父列最大字符串长度
    //#      按行顺序来依次处理列
    //#
    //#      不行, 上面的实现方式还是有很多缺陷. 现在简要处理, 循环处理所有行, 获取最大的长度行的长度(也就是getRowSplitPos()后最后一个元素值最大).
    //#      然后所有的行的最大值都设成这个值即可. 即最后一个元素都设置为该值即可.
    //# 2017.01.01:
    //#      修正复杂表格的对齐暂时放弃. 很复杂, 不是简单调整能完成.
    //#      从width最短的col开始处理, 然后所有行轮训, 察看是否有subcol，
    //#      如果有, 就计算subcol 长度之和与分隔符数量, 然后修正最大长度.
    //# 2019.03.09
    //#      这儿的处理主要是子列与夫列的处理，分隔符占用宽度也要计算
    //#   实现思路:
    //       先将colMap 排序, 从窄到宽处理。对于每个列, 循环处理所有行, 判断该行中所有的cell, 对于cell 在列中的记录在
    //       col 的son 中, 每一行的所有son cell(其实也就是son col), 计算所有son col 的长度之和, 如果大于当前col 的maxlen
    //       则更新当前col 的maxlen为所有son col 的长度之和
    void fixColMaxlen( CELL *cellMap_p, SHEET * sheet_p )
    {
        if ( !cellMap_p || ! sheet_p )
            return;
        
        if ( !sheet_p->colMap_p )
            return;
        
        int     colTotal = sheet_p->colTotal;

        COL     *o_colMap[ sheet_p->colTotal ];      // 存放排序后的col 数组
        
        // 先把col 进行从窄到宽 排序, 因为只有宽的col才能是窄的col的父col 
        orderColMap( o_colMap, sheet_p->colMap_p, sheet_p->colTotal ); // 从窄到宽对 colMap_p 排序, 结果存放在 o_colMap

        for ( int i = 0; i < sheet_p->colTotal; i ++ ) {
            if ( ! o_colMap[i] ) {
                printf( " o_colMap[%d] 是空指针！！！！\n", i);
                continue; 
            }
            printf( " ----------- after orderColMap()- 排序好的:-----col id=%d--------------------\n", o_colMap[i]->id );
            printf( "x=%.2f, w=%.2f, maxlen = %d\n", o_colMap[i]->x, o_colMap[i]->w, o_colMap[i]->maxlen );
            printf( "cellTotal = %d, sonTotal=%d\n", o_colMap[i]->cellTotal, o_colMap[i]->sonTotal );

            if ( o_colMap[i]->c_list ) {
                printf("cells:");
                for( int j = 0; j < o_colMap[i]->cellTotal; j ++ ) {
                    printf( " %d", o_colMap[i]->c_list[j] );
                }
                printf("\n");
            }
            if ( o_colMap[i]->son ) {
                printf("sons:");
                for( int j = 0; j < o_colMap[i]->sonTotal; j ++ ) {
                    printf( " %d", o_colMap[i]->son[j] );
                }
                printf("\n");
            }
        }
        
        // 下面遍历所有行, 根据行中的cell 所在的列来判断是否是当前列的子列, 然后计算子列长度综合是否超过夫列maxlen
        for ( int i = 0;i < colTotal; i ++ ) {              // 遍历所有的col
            getColSon( sheet_p, o_colMap[i], cellMap_p );
        }
        
    }


    // getColSonTotal()
    //      获取给定的col 的可能存在的子列的个数。 有可能子列为0, 即没有子列
    // 该函数仅被 fixColMaxlen() 调用
    int getColSon( SHEET * sheet_p, COL *col_p, CELL * cellMap_p )
    {
        int         sonTotal;
        COL     *   scol_p;             // 用来指向son col_p
        ROW     *   row_p;
        CELL    *   cp;
        int         x, w, sx, sw;       // sx, sw 用来存放子列的x, w
        char        buf[512];           // 用来临时存放子列的编号, 因为没遍历完之前无法为col的son申请内存
        int         i, j, k;
        char        tmpbuf[12];         // 用来临时存放col的id 编号字符串
        int         maxlen;             // 用来计算子列长度之和
        
        if( !sheet_p || !col_p )
            return 0;

        x = col_p->x,  w = col_p->w;
             
        // 遍历rowMap
        sonTotal = 0;
        row_p = sheet_p->rowMap_p;
        memset( buf, 0, 512 );
        printf("getColSon()  打印 rowMap_p\n");
        printRowMap( row_p );
        while ( row_p ) {                                   // 遍历所有的行
            maxlen = 0;
            for ( j = 0; j < row_p->cellTotal; j ++ ) {     // 遍历 行中的所有cell
                cp = (CELL *)findCell( cellMap_p, row_p->c_list[j] );   // 根据编号找到cell 节点
                if ( !cp ) {
                    printf("row_p->c_list[%d]没找到！！！！！\n", j, row_p->c_list[j] );
                    continue;
                }
                if ( cp->col == col_p->id)                 // 该cell所在的col 与外循环中的col 是同一列, 跳过
                    continue;
                scol_p = findCol( sheet_p->colMap_p, cp->col );
                if ( !scol_p )                                   // 容错
                    continue;
                sx = scol_p->x, sw = scol_p->w;
                // 下面判断是否是子列
                if ( sx >= x && ( (sx+sw) < (x+w) || abs(x+w-sx-sw)<1 ) ) {  // abs() 是因为对于画布而言像素小于1也是同一列
                    sonTotal ++;      
                    sprintf( buf, "%s %d", buf, scol_p->id );   // " 2 3 12 23", 空格分隔, 临时保存子列列表
                    maxlen += scol_p->maxlen;
                }
            }

            // 根据子列总长度来修正当前列的maxlen
            if ( maxlen > col_p->maxlen )
                col_p->maxlen = maxlen; 

            row_p = row_p->next;
        }

        printf(" col id=%d 子列:%s\n", col_p->id,  buf );
        return 0;
        // 下面处理临时保存子列列表的字符串, 将子列编号放入 col_p->son 数组中
        if ( sonTotal > 0 ) {
            printf("列%d 的子列为:%s\n", col_p->id, buf);
            col_p->sonTotal = sonTotal;
            col_p->son = (int *)calloc( sizeof(int) * sonTotal, 1 );

            int     len = strlen( buf );
            j = 0,  k = 1;                          // 从1开始, 是为了跳过字符串的第一个空格
            for ( i = 1; i < len; i ++ ) {          // 从1开始, 是为了跳过字符串的第一个空格
                if ( buf[i] == ' ' || i == len-1 ) {      // 找见空格 或者到了尾部
                    memset( tmpbuf, 0, 12 );
                    memcpy( tmpbuf, &buf+k, i-k );
                    col_p->son[j] = atoi( tmpbuf );
                }
            }
        }
        return 0;
    }

    // orderColMap()
    //     将 colMap 链表按照col 的宽度从窄往宽排序,  结果存放在 des 数组中
    // 该函数仅被 fixColMaxlen() 调用, 调用前务必要确认空间足够
    // 排序方法:
    //     希尔排序法, 因为原来的链表要保持不变, 排序结果存放在新的数组中. 只需要
    //  遍历一遍链表就可以排序完成。
    //     增量从 coltotal/2 开始， 依次减1.
    void orderColMap( COL ** des, COL * src, int colTotal )
    {
        COL     *   dcol_p, * col_p;
        int         i;          // 用来记录排好序的col个数
        
        if ( !des || !src )
            return;

        col_p = src;
        
        // 1. 先将链表节点复制到 des 数组中
        i = 0;
        while ( col_p ) {
            des[i] = col_p;    
            col_p = col_p->next;
            i ++;
        }

        //2. 希尔排序法对 des 数组进行排序
        int     step = colTotal /2;     // 第一个增量是总长度/2
        while ( step > 0 ) {
            i = 0;
            while ( i < colTotal - step ) {    // 根据步长(增量) 进行比较交换
                if ( des[i]->w > des[i+step]->w ) { 
                    col_p       = des[i];
                    des[i]      = des[i+step];
                    des[i+step] = col_p;
                }
                i ++;
            } 
            step --;
        }
        

        
    }

    /*
    #  15.62 fixRows()
    #      修正某些行本身又带有子行的情况, 比如中间的一个单元格可能是上下两个单元格, 参见0106  300182捷成股份2016年度业绩预告
    #  思路:
    #     2017.01.10:
    #         rowIndexMap 修正. 按子行的多少来扩充母行中没有子行的单元格, 但是显示的时候, 不显示对应的线.
    #          ┌-------------------┬----------------┬----------------------------┐
    #          |         1         |    2           |   3                        |
    #          ├-------------------┼----------------┼----------------------------┤
    #          |                   |       5        |                            |
    #          |      4            |                |          7                 |
    #          |                   ├----------------┤                            |
    #          |                   |        6       |                            |
    #          └-------------------┴----------------┴----------------------------┘
    #         上面的row 应该有4行, 分别为: row1: [1,2,3], row2 = 4, 7, row3,row4;  row 2 = 5, row3 = 6 修正如下
    #         row1 = [1,2,3], row3={cells=[4,5,7],parent="2"}, row4={cells=[4,6,7],parent="2"},  row2= {[4, 7], "sons"=[row3, row4]}
    #         也就是说, 有son的row不用显示,
    #         有parent的row 显示的时候要3个工作
    #          1. 是否是子行中的第一行, 也就是y最大的一行. 如果是, 则所有分割线要显示, 也就是cell是否在parent不重要
    #          2. 判断cell是否在parent中, 如果有, 该cell对应的连线不处理
    #          rowMap = {1: {'h': 78.0, 'y': 561.42}, 2: {'h': 31.2, 'y': 379.16}, 3: {'h': 27.0, 'y': 351.68}, 4: {'h': 54.48, 'y': 324.2}}
    #          rowIndexMap={1: [2, 7], 2: [2, 1, 2, 3], 3: [1, 5], 4: [2, 4, 6]}
    #     2017.01.11:
    #          rowIndexMap = {1:{"cells":,"lines":,"parent":,"sons":}}
    #          
    */
    void fixRows()
    {
            
    }

    // orderRowMap()
    //     将 rowMap 链表按照row 的高度从小往大排序,  结果存放在 des 数组中
    // 该函数仅被 fixRows() 调用, 调用前务必要确认空间足够
    // 排序方法:
    //     希尔排序法, 因为原来的链表要保持不变, 排序结果存放在新的数组中. 只需要
    //  遍历一遍链表就可以排序完成。
    //     增量从 rowtotal/2 开始， 依次减1.
    void orderRowMap( ROW ** des, ROW * src, int rowTotal )
    {
        ROW     *   row_p;
        int         i;          // 用来记录排好序的col个数
        
        if ( !des || !src )
            return;

        row_p = src;
        
        // 1. 先将链表节点复制到 des 数组中
        i = 0;
        while ( row_p ) {
            des[i] = row_p;    
            row_p = row_p->next;
        }

        //2. 希尔排序法对 des 数组进行排序
        int     step = rowTotal /2;     // 第一个增量是总长度/2
        while ( step > 0 ) {
            i = 0;
            while ( i < rowTotal-step ) {    // 根据步长(增量) 进行比较交换
                if ( des[i]->h > des[i+step]->h ) { 
                    row_p       = des[i];
                    des[i]      = des[i+step];
                    des[i+step] = row_p;
                }
            } 
            step --;
        }
    }


    // 第二次遍历cellMap_p 中的cell, 为 col, row 中的c_list 填充cell id
    // fill_c_list() 仅仅是为了提高可读性而封装的代码
    //     仅被 preProcColRow() 调用
    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p )
    {
        CELL        *   cp;
        COL         *   col_p;
        ROW         *   row_p;
        int             i;          // 用来记录 c_list 下标


        printf("---------------fill_c_list() begin -----------------------------------------------------\n");
        cp = cellMap_p;

        printColMap( sheet_p->colMap_p );
        while ( cp ) {
            if ( cp->h < 1 || cp->w < 1 ) {     // 页眉页脚等控制类cell不处理
                cp = cp->next;
                continue;
            }

            printf( "-------------------cellID=%d(x=%.2f, w=%.2f)要找的列为:%d$$$$$$$$\n", cp->id, cp->x, cp->w, cp->col );
            col_p = getColOfID( sheet_p->colMap_p, cp->col );
            
            if ( !col_p )
                return;
                       
            i = 0;
            while ( col_p->c_list[i] != 0 )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            col_p->c_list[i] = cp->id;

            row_p = getRowOfID( sheet_p->rowMap_p, cp->row );
            i = 0;
            while ( row_p->c_list[i] != 0 )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            row_p->c_list[i] = cp->id;

            cp = cp->next;
        }
    }

    // 为colMap和rowMap中的col, row 申请对应c_list 需要的空间
    //  spaceForColRowMap()  函数是为了提高可读性而封装的部分代码
    //      仅被 preProcColRow() 调用
    void spaceForColRowMap( SHEET * sheet_p )
    {
        COL         * col_p;
        ROW         * row_p;

        if ( !sheet_p )
            return;

        col_p = sheet_p->colMap_p;
        
        while( col_p ) {
            if ( col_p->cellTotal > 0 )
                col_p->c_list = (int *)calloc( sizeof(int) * col_p->cellTotal, 1 ); 
            col_p = col_p->next;
        }

        row_p = sheet_p->rowMap_p;
        
        while( row_p ) {
            if ( row_p->cellTotal > 0 )
                row_p->c_list = (int *)calloc( sizeof(int) * row_p->cellTotal, 1 ); 
            row_p = row_p->next;
        }
    }
    // newCol()
    //     注意, 这儿没有真正将cellID 记录到colMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该col
    // 仅 被  buildColRowMap() 调用, 创建一个COL 节点
    void newCol( SHEET *sheet_p, CELL * cp )
    {
        COL     * col_p, * lcp;

        if ( !sheet_p || !cp ) 
            return;
        
        if ( !sheet_p->colMap_p ) {                             // colMap_p 是空的, 则是第一个col
            col_p = ( COL * )calloc( sizeof( COL ), 1 );

            col_p->id       = 1;                                // 第一列 id 为 1
            col_p->x        = cp->x,    col_p->w    = cp->w;
            col_p->prev     = NULL, col_p->next = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            col_p->parent   = NULL, col_p->son  = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            
            col_p->maxlen   = cp->maxlen;
            col_p->cellTotal = 1;
                        
            sheet_p->colMap_p   = col_p;
            sheet_p->lcp        = col_p;
            sheet_p->colTotal   = 1;

            cp->col = 1;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            return;
        } else {
            col_p = ( COL * )calloc( sizeof( COL ), 1 );
            
            lcp = sheet_p->lcp;                                 // 最后一个col pointer

            col_p->id = lcp->id + 1;                            // 新id 为 前一个col的id + 1
            col_p->x  = cp->x, col_p->w = cp->w;
            col_p->prev   = lcp, col_p->next = NULL;            // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            col_p->maxlen = cp->maxlen;
            col_p->cellTotal = 1;                               // 对于新建的col而言,  这是第一个cell
            
            //col_p->parent = NULL, col_p->son = NULL;    // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            lcp->next = col_p;
            
            sheet_p->lcp = col_p;                               // 更新 last col pointer
            sheet_p->colTotal ++;               // 更新 shet 中的colTotal 数量, 但是这儿并没有记录具体的cellID, 后面需要根据该数值申请内存后再处理

            cp->col = col_p->id;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            
            return;
        } 
        
        
    }

    // 查找列  映射中是否有复合挑件的列
    // 仅 被  buildColRowMap() 调用, 查找复合挑件的COL 
    COL * inColMap( COL * colMap_p, float x, float w )
    {
        COL     *   col_p;
        float       cx, cw;     // col的x坐标与w宽度

        if ( !colMap_p )        // 如果 colMap_p 是空, 则肯定不包含
            return NULL;

        col_p = colMap_p;

        while ( col_p ) {
            cx = col_p->x,  cw = col_p->w;
            
            if ( x == cx && w == cw  ) {    // 说明该列已经存
                return col_p;
            }
            col_p = col_p->next;
        }

        // 如果遍历完都没有找到, 则说明该cell所在的col 没有记录
        return NULL;
    }


    // newRow()
    //     注意, 这儿没有真正将cellID 记录到rowMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该row
    // 仅 被  buildColRowMap() 调用, 创建一个ROW 节点
    //
    void newRow( SHEET *sheet_p, CELL *cp )
    {
        ROW     * row_p, * lrp;

        if ( !sheet_p ) 
            return;

        printf("1111111111111111111111111\n");

        if ( !sheet_p->rowMap_p ) { // rowMap_p 是空的, 则是第一个row
            printf("这是第一个节点！y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );

            row_p->id       = 1;                            // 第一行 id 为 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->prev     = NULL, row_p->next = NULL;     // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            row_p->parent   = NULL, row_p->son  = NULL;     // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            
            row_p->maxlines  = cp->txtTotal;                // 对于新建的row, 最大行数就是第一个cell的最大行数(txtTotals 里面已经把同行的合并了)
            row_p->cellTotal = 1;

            sheet_p->rowMap_p   = row_p;
            sheet_p->lrp        = row_p;
            sheet_p->rowTotal   = 1;

            cp->row = 1;
            
            return;
        } else {
            printf("这是尾部添加的节点！y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );
            
            lrp = sheet_p->lrp;                             // 最后一个col pointer

            row_p->id   = lrp->id + 1;                      // 新id 为 前一个col的id + 1
            row_p->y    = cp->y,    row_p->h    = cp->h;
            row_p->prev = lrp,  row_p->next = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            //row_p->parent = NULL, row_p->son = NULL;      // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            row_p->maxlines  = cp->txtTotal;               // 对于新建的row, 最大行数就是第一个cell的最大行数(txtTotals 里面已经把同行的合并了)
            row_p->cellTotal = 1;

            lrp->next = row_p;
            
            sheet_p->lrp = row_p;               // 更新 last row pointer
            sheet_p->rowTotal ++;               // 更新 shet 中的rowTotal 数量, 但是这儿并没有记录具体的cellID, 后面需要根据该数值申请内存后再处理

            cp->row = row_p->id;
            
            return;
        } 
    }

    // 仅 被  buildColRowMap() 调用, 查找复合挑件的ROW 
    ROW * inRowMap( ROW * rowMap_p, float y, float h )
    {
        ROW     *   row_p;
        float       cy, ch;     // row的y坐标与h宽度

        if ( !rowMap_p )        // 如果 rowMap_p 是空, 则肯定不包含
            return NULL;

        row_p = rowMap_p;

        while ( row_p ) {
            cy = row_p->y,  ch = row_p->h;
            
            if ( y == cy && h == ch  ) {    // 说明该列已经存
                return row_p;
            }
            row_p = row_p->next;
        }

        // 如果遍历完都没有找到, 则说明该cell所在的col 没有记录
        return NULL;
    }


    // 遍历一遍cellMap, 创建colMap, rowMap, 但是没有c_list, 因为没遍历完之前不知道有多少cell在col或row中, 
    // 无法为 c_list 申请空间
    //   buildColRowMap()  只被 preProcColRow() 调用
    // 调用了4个子函数
    //   inColMap(), newCol(),  inRowMap(), newRow() 
    void buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p )
    {
        CELL        *   cp;
        float           x, y, w, h;
        COL         *   col_p;
        ROW         *   row_p;
        
        cp = cellMap_p;
        while( cp ) {
            while( cp->h < 1 || cp->w < 1 )            // 高度或宽度小于1 的都不处理, 一般是控制用的
                cp = cp->next;

            if ( !cp )
                break;
            
            x = cp->x, y = cp->y, w = cp->w, h = cp->h;
                                                    // 1. 先处理列row
            col_p = inColMap( sheet_p->colMap_p, x, w );
            
            if ( !col_p ) {                         // 如果该cell 所在列 不在colMap 中, 则加入colMap
                newCol( sheet_p, cp );              // 注意, 这儿没有真正将cellID 记录到colMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该col
            } else {                                // cell 所在的列已经在colmap里记录了
                                                    // 同时在cell 中记录col 信息
                col_p->cellTotal ++;                // 增加列中的cell 计数
                cp->col = col_p->id;
                
                                                    // 更新 col_p 的maxlen
                if ( col_p->maxlen < cp->maxlen ) 
                    col_p->maxlen = cp->maxlen;
            }
            
            printf("--------------------------cp->col=%d, cp->id=%d\n", cp->col, cp->id);
                                                    // 2. 处理 行row
            row_p = inRowMap( sheet_p->rowMap_p, y, h );
            
            if ( !row_p ) {                         // 如果该cell 所在行 没有在rowMap中记录, 则增加该行在rowMap中的记录
                newRow( sheet_p, cp );
            } else {                                // 该cell所在的行已经在rowMap中了, row id 记录在 row_id变量中
                                                    // 在cell 中记录  row 信息
                row_p->cellTotal ++;
                cp->row = row_p->id;
                
                if ( row_p->maxlines < cp->txtTotal )
                    row_p->maxlines = cp->txtTotal;
            }
            
            // 继续处理下一个cell
            cp = cp->next;
        }
    }

    COL * getColOfID( COL * colMap_p, int  id )
    {
        COL     * col_p;
        
        if ( !colMap_p )
            return NULL;
        
        col_p = colMap_p;
        while ( col_p ) {
            if ( col_p->id == id )
                return col_p;
            col_p = col_p->next;
        }
        // 遍历完没有找到
        return NULL;
    }

    ROW * getRowOfID( ROW * rowMap_p, int  id )
    {
        ROW     * row_p;
        
        if ( !rowMap_p )
            return NULL;
        
        row_p = rowMap_p;
        while ( row_p ) {
            if ( row_p->id == id )
                return row_p;
            row_p = row_p->next;
        }
        // 遍历完没有找到
        return NULL;
    }
    
    // # 4. 解析文本与cell的关系, 拆分出table 出来, 创建tableMap={1:[cellid_list]}， 同时在cellMap 中添加tableid 属性
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }

    // # 5. 输出文本. 已经处理过同行多文本的拼接, 不再cell中的文本就是单独一行文本
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }
    /*
*/
