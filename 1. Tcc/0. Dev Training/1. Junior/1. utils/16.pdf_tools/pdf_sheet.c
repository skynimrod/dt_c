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
    #include "str_tools.h"
    #include "pdf_tools.h"
    #include "sheet.h"

    char * procText4Table( char * retbuf, int dlen, TEXT * textMap_p, CELL * cellMap_p );
    void freeSheet( SHEET * sheet_p );
    
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p );
    void preProcTxtMap4Cell( TEXT * textMap_p, CELL * cellMap_p );
    void preProcColRow( CELL *cellMap_p, SHEET * sheet_p );
    
    ROW * getRowByID( ROW * rowMap_p, int  id );
    COL * getColByID( COL * colMap_p, int  id );
    TABLE * getTableByID( TABLE * tableMap_p, int  id );

    // buildColRowMap(), spaceForColRowMap(), fill_c_list()  仅被 preProcColRow() 调用
    void  buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p );
    
    void fillColSon( CELL * cellMap_p, SHEET * sheet_p );
    void fillRowSon( CELL * cellMap_p, SHEET * sheet_p );
    
    void fixColMaxlen( CELL * cellMap_p, SHEET * sheet_p );
    void fixRowMaxlines( CELL * cellMap_p, SHEET * sheet_p );

    void fillColRow_List( SHEET * sheet_p );

    
    void orderRowMap( ROW ** des, ROW * src, int rowTotal );
    void spaceForColRowMap( SHEET * sheet_p );
    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p );
    void fill_son_c_llist( COL * col_p, ROW * row_p, int cell_id );
    // buildColRowMap() end

    TEXT * buildTableTxt( char * desbuf, int dlen, SHEET *sheet_p, CELL *cellMap_p, int table_id );
    int getTableID( SHEET * sheet_p, CELL * cellMap_p, int cell_id  );

    char * buildPageTxt( char * retbuf, int dlen, SHEET * sheet_p, CELL * cellMap_p, TEXT * textMap_p );

    bool buildRowHeader( char * desbuf, int dlen, CELL * cellMap_p, SHEET * sheet_p, ROW * row_p );
    bool buildRowSplit( char * desbuf, int dlen, CELL * cellMap_p, SHEET *sheet_p, TABLE * table_p, int rowNo );


    char * procText4Table( char * retbuf, int dlen, TEXT * textMap_p, CELL * cellMap_p )
    {
        SHEET       * sheet_p;          // 表单, 记录表格及相关信息

        sheet_p = (SHEET *)calloc( sizeof(SHEET), 1 );


        // # 1. 处理textMap, 将不在表格中的文本进行处理, 如果是同一行的就进行合并, 同时将cell中的txtIDs_p 补充完整
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
        preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        // # 3. 预处理 表格, 根据坐标关系归类到列与行, 
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
        buildPageTxt( retbuf, dlen, sheet_p, cellMap_p, textMap_p );
        

        freeSheet( sheet_p );
        
        return NULL;
    }

    void freeCOL( COL * col_p )
    {
        if ( !col_p )
            return;

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
        COL     * col_p, * tmp_col_p;
        ROW     * row_p, * tmp_row_p;
        TABLE   * table_p,* tmp_table_p;
        
        if ( !sheet_p )
            return;
        
        // 1. 释放 列col  相关节点
        if ( sheet_p->colMap_p ) {
            col_p = sheet_p->colMap_p;       // 从前往后删除节点
            while( col_p ) {
                tmp_col_p = col_p->next;
                freeCOL( col_p );
                col_p = tmp_col_p;
            }
        }

        // 2. 释放 行row  相关节点
        if ( sheet_p->rowMap_p ) {
            row_p = sheet_p->rowMap_p;            // 从前往后删除节点
            while( row_p ) {
                tmp_row_p = row_p->next;        // 先保留好下一个节点的地址
                freeROW( row_p );    
                row_p = tmp_row_p;
            }
        }

        // 3. 释放 表格 table  相关节点
        if ( sheet_p->tableMap_p ) {
            table_p = sheet_p->tableMap_p;      // 从前往后删除节点
            while( table_p ) {
                tmp_table_p = table_p->next;        // 先保留好下一个节点的地址
                freeTable( table_p );
                table_p = tmp_table_p;
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

        // 2. 下面为cell 的文本指针数组 申请空间(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal > 0 )
                cp->tps = (TEXT **)calloc( sizeof(TEXT*), cp->txtTotal );   // 为存放记录cell中的文本编号申请空间
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
            // 此时, 该文本的id 并没有存放在cell 中的tps 数组中, 之前的处理仅仅是申请了足够的存放空间
            // 在这儿便处理便将处理后的文本指针存放到tps 数组中
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->tps[i] == 0 )         // 碰到结尾了
                    break;
            
            if ( i == 0 ) {                    // 表示该文本是cell 中的第一个文本, 记录在tps[0] 中, 不做合并文本处理
                cp->tps[i] = tp;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {       // 容错处理,表示预申请空间已经存满了, 那么当前的这个文本就不应该在该cell中
                return ;
            } else  {
                tpp = cp->tps[i-1];        //在同一个cell中的文本节点的前一个文本节点编号
                if ( !tpp )                         // 容错处理。(这种情况不应该发生)
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
                    cp->tps[i] = tp;            // 添加cell中的text指针记录
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
    void preProcColRow( CELL *cellMap_p, SHEET * sheet_p )
    {
        if ( !sheet_p || !cellMap_p )
            return;

        // 第一次遍历cellMap, 创建colMap, rowMap, 但是没有c_list, 因为没遍历完之前不知道有多少cell在col或row中, 
        // 为 c_list 无法申请空间
        buildColRowMap( cellMap_p, sheet_p );

        // 修正 Col 中的maxlen, 主要是根据子col 来修正 父 col 的宽度

        printf("==============================aaaaaaaaaaaaa=================\n");
        printColMap( sheet_p->colMap_p );
        printRowMap( sheet_p->rowMap_p );

        // 填充col 中的son, parent
        fillColSon( cellMap_p, sheet_p );
        // 填充row 中的son, parent
        fillRowSon( cellMap_p, sheet_p );

        // 为colMap和rowMap中的col, row 申请对应c_list 需要的空间
        spaceForColRowMap( sheet_p );
        
        // 第二次遍历cellMap_p 中的cell, 为 col, row 中的c_list 填充cell id
        fill_c_list( cellMap_p, sheet_p );

        fixColMaxlen( cellMap_p, sheet_p );
        fixRowMaxlines( cellMap_p, sheet_p );

        printf("==============================bbbbbbbbbbbb=================\n");
        printColMap( sheet_p->colMap_p );
        printRowMap( sheet_p->rowMap_p );
        printTableMap( sheet_p->tableMap_p );
        printf("==============================cccccccccccccc=================\n");

        // 填充 TABLE的col_list, row_list信息, 传递 cellMap_p与 textMap_p 是为了获取该表的最后一个text pointer
        fillColRow_List( sheet_p );

        printTableMap( sheet_p->tableMap_p );
        return;
    }

    // getLastTpofRow()
    // 获取该行的最后一个文本指针, 其实就是最后一个cell 的最后一个txtID
    TEXT * getLastTpofRow( ROW * row_p )
    {
        int         i;
        int         j;
        CELL    *   cp;

        printf("getLastTP()..begin...\n");
        if ( !row_p ) {
            printf("getLastTP().....\n");
            return NULL;
        }
     
        i = row_p->cellTotal;
        
        while ( i > 0 ) {       // 有可能cell 是空的, 所有要从后往前遍历该row的cell
            cp = row_p->c_list[ i - 1 ];     // 该 row 的最后一个cp

            printf("row (%d) 最后一个cell(%d)\n", row_p->id, cp->id);

            j = cp->txtTotal;

            if ( j > 0 ) {
                printf("cell (%d) 最后一个txt(%d)\n", cp->id, cp->tps[j-1]->id );
                return cp->tps[ j - 1];        // cel 中的最后一个text pointer
            } else {
                i --;
            }
        }
        return NULL;
    }

    // 填充 表中的col_list, row_list 节点指针数组内容
    void fillColRow_List( SHEET * sheet_p )
    {
        int         colTotal;
        int         rowTotal;
        int         i;
        COL     *   col_p;
        ROW     *   row_p;
        TABLE   *   table_p;
        TEXT    *   tp;

        if ( !sheet_p || !sheet_p->tableMap_p  ) 
            return;

        table_p = sheet_p->tableMap_p;

        while ( table_p ) {
            // 1. 填充 col_list
            colTotal = table_p->colTotal;

            col_p = sheet_p->colMap_p;
            i = 0;

            while ( col_p && i < colTotal ) {
                if ( col_p->table_id == table_p->id ) {
                    table_p->col_list[i] = col_p;
                    i ++;
                }
                col_p = col_p->next;
            }    

            // 2. 填充row_list
            rowTotal = table_p->rowTotal;

            row_p = sheet_p->rowMap_p;
            i = 0;

            while ( row_p && i < rowTotal ) {
                if ( row_p->table_id == table_p->id ) {
                    table_p->row_list[i] = row_p;
                    i ++;
                }
                row_p = row_p->next;
            }    

            // 把last tp 也找到并赋值
            //
            i = table_p->rowTotal;
            while ( i > 0 ) { // 从最后一行开始找, 直到找到一个有效的text或者空表就返回NULL
                row_p = table_p->row_list[i-1];     // 最后一行
                tp = getLastTpofRow( row_p  );
                if ( tp ) {
                    // 找到了最后一个有效的tp
                    printf( "table(%d)的最后一个文本是:%d, %s\n", table_p->id, tp->id, tp->buf );
                    table_p->l_tp = tp;
                    break;
                }

                i --;
            }
            

            table_p = table_p->next;
        }
    }

    // orderColMap()
    //     将 colMap 链表按照col 的w(宽度)从小往大排序,  结果存放在 des 数组中
    // 该函数仅被 fixColMaxlen() 调用, 调用前务必要确认空间足够
    // 排序方法:
    //     希尔排序法, 因为原来的链表要保持不变, 排序结果存放在新的数组中. 只需要
    //  遍历一遍链表就可以排序完成。
    //     增量从 coltotal/2 开始， 依次减1.
    void orderColMap( COL ** des, COL * src, int colTotal )
    {
        COL     *   col_p;
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
            while ( i < colTotal-step ) {    // 根据步长(增量) 进行比较交换
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
    //  2019.03.12
    //       有个问题, 对于同一页中的不同表格, 不应该进行列的子列判断.
    //       增加判断, 如果下一个cell不等于上一个cell的y + h, 那么就是另一个表中的cell了
    //       修改实现方式, 不适用cellMap， 也不用给colMap排序了, 直接双重循环处理
    // 2019.03.14
    //       需要处理排序后的colMap, 一个col 只能有且只有一个是最窄的那个col parent, 否则可能会匹配到宽的parent col, 结果造成maxlen 计算错误
    // 2019.03.18
    //       目前的处理有问题, 不能统计所有子列的maxlen 总和. 应该是同一行的子列的maxlen 之和
    //
    void fixColMaxlen( CELL * cellMap_p, SHEET * sheet_p )
    {
        int             maxlen;
        COL         *   col_p, * col_p1;
        ROW         *   row_p;
        CELL        *   cp;

        if ( !sheet_p || !cellMap_p ) 
            return;
        
        // 1. 循环遍历寻找母列
        col_p = sheet_p->colMap_p;
        while( col_p ) {

            // 按行进行遍历, 确保计算maxlen 的时候是同行的子列maxlen之和
            row_p = sheet_p->rowMap_p;
            while( row_p ) {
                maxlen = 0;
                
                for ( int i = 0; i < row_p->cellTotal; i ++ ) {
                    cp = row_p->c_list[i];
                    
                    col_p1 = getColByID( sheet_p->colMap_p, cp->col );
                    
                    if ( col_p == col_p1->parent ) 
                        maxlen += col_p1->maxlen; 
                }

                // 修正maxlen
                if ( maxlen > col_p->maxlen )
                    col_p->maxlen = maxlen;

                row_p = row_p->next;
            }
            col_p = col_p->next;
        }
    }

    void fixRowMaxlines( CELL * cellMap_p, SHEET * sheet_p )
    {
        int             maxlines;
        COL         *   col_p;
        ROW         *   row_p, * row_p1;
        CELL        *   cp;

        if ( !sheet_p || !cellMap_p ) 
            return;
        
        // 1. 循环遍历寻找母列
        row_p = sheet_p->rowMap_p;
        while( row_p ) {

            // 按行进行遍历, 确保计算maxlen 的时候是同行的子列maxlen之和
            col_p = sheet_p->rowMap_p;
            while( col_p ) {
                maxlines = 0;
                
                for ( int i = 0; i < col_p->cellTotal; i ++ ) {
                    cp = col_p->c_list[i];
                    
                    row_p1 = getRowByID( sheet_p->rowMap_p, cp->row );
                    
                    if ( row_p == row_p1->parent ) 
                        maxlines += row_p1->maxlines; 
                }

                // 修正maxlines
                if ( maxlines > row_p->maxlines )
                    row_p->maxlines = maxlines;

                col_p = col_p->next;
            }
            row_p = row_p->next;
        }
        return;
    }

    
    void fillColSon( CELL * cellMap_p, SHEET * sheet_p )
    {
        //int             maxlen;
        int             colTotal;
        float           x1, w1, x2, w2;
        COL         **  o_colMap_p;           // 存放 排好序的rowMap 节点指针, 按 maxline 从小到大排序 
        COL         *   col_p1, * col_p2;

        if ( !sheet_p || !cellMap_p ) 
            return;

        colTotal = sheet_p->colTotal;
        
        o_colMap_p = (COL **)calloc( sizeof(COL *) * colTotal, 1 );

        orderColMap( o_colMap_p, sheet_p->colMap_p, colTotal );

        // 1. 循环便利寻找母行
        for ( int i = 0; i < colTotal; i ++ ) {
            col_p1 = o_colMap_p[i];
            x1 = col_p1->x, w1 = col_p1->w;

            for ( int j = 0; j < colTotal; j ++ ) {
                col_p2 = o_colMap_p[j];

                if ( col_p1->id == col_p2->id )     // 跳过同一个row
                    continue;

                x2 = col_p2->x, w2 = col_p2->w;

                if ( col_p1->table_id == col_p2->table_id &&                        // 首先要保证在一个表中
                      ( x1 >= x2 || abs(x2-x1) < 1 ) && ( (x1+w1) <= (x2+w2) || abs(x2+w2-x1-w1) < 1 ) ) { // 判断col_p2 是否是  col_p1 母列  
                        // # abs 的判断是因为子列和母列左面边或右面边重叠的时候, 坐标有可能不是完全相同, 有可能有微小误差
                    printf( "col id=%d 的母列  是 col id=%d\n", col_p1->id, col_p2->id );

                    col_p1->parent = col_p2;
                    col_p2->sonTotal ++;
                    break;      // 找到 最窄的那个母行就结束, 有且只有一个母行
                }
            }
        }

        // 2. 填充 子列 节点指针   数组, 同时修正母行的maxlen,
        int     k;
        for( int i = 0; i < colTotal; i ++ ) {
            col_p1 = o_colMap_p[i];             // 这个赋值是为了提高下面代码的可读性
            if ( col_p1->sonTotal > 0 )  {           // 有子行
               col_p1->son = (COL **)calloc( sizeof(COL *) * col_p1->sonTotal, 1);
               
                //maxlen      = 0;
                k           = 0;        // 用来标识son 指针数组的下标
                
                // 这儿不能用 o_colMap_p, 因为这个是按照w排序的, 不是x坐标
                col_p2 = sheet_p->colMap_p;
                while ( col_p2 && k < col_p1->sonTotal ) {    // k < col_p1->sonTotal 是为了防止内存溢出
                    if  ( col_p2->parent == col_p1 ) {     // 找见 子列 了
                        col_p1->son[k] = col_p2;
                        k ++;

                        //maxlen += col_p2->maxlen;       // 合计子列的 maxlen , 2019.03.18 不能这么合计, 要区分是否是同一行的col
                    }
                    col_p2 = col_p2->next;
                }

                // 修正 母列的maxlen
                //if ( maxlen > col_p1->maxlen ) {
                //    col_p1->maxlen = maxlen;
                //}

                // 给子列排序, 按照y 坐标从小到大, 暂时不排序, 因为后面填充col 的 c_list 的时候, 需要把母列的c_list 也要补充进去
                // 暂时不需要, 应为整理表格内容的时候是按照row  顺序来输出的

            } 
        } 

        free( o_colMap_p );
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
          2019.03.14:
               每个row 只有一个parent, 且是最窄的那个parent. 每个son row 会继承 parent row 的cell list. 并且cell list 是按照x 坐标顺序排列(不是id顺序)
               这样能保证显示表格的时候顺序正确.
         2019.03.18
               目前的实现方式有问题, 不能计算所有的 子行
    #          
    */
    void fillRowSon( CELL * cellMap_p, SHEET * sheet_p )
    {
        //int             maxlines;
        int             rowTotal;
        float           y1, h1, y2, h2;
        ROW         **  o_rowMap_p;           // 存放 排好序的rowMap 节点指针, 按 maxline 从小到大排序 
        ROW         *   row_p1, * row_p2;

        if ( !sheet_p || !cellMap_p ) 
            return;

        rowTotal = sheet_p->rowTotal;
        
        o_rowMap_p = (ROW **)calloc( sizeof(ROW *) * rowTotal, 1 );

        orderRowMap( o_rowMap_p, sheet_p->rowMap_p, rowTotal );

        // 1. 循环便利寻找母行
        for ( int i = 0; i < rowTotal; i ++ ) {
            row_p1 = o_rowMap_p[i];
            y1 = row_p1->y, h1 = row_p1->h;

            for ( int j = 0; j < rowTotal; j ++ ) {
                row_p2 = o_rowMap_p[j];

                if ( row_p1->id == row_p2->id )     // 跳过同一个row
                    continue;

                y2 = row_p2->y, h2 = row_p2->h;

                if ( row_p1->table_id == row_p2->table_id &&                        // 首先要保证在一个表中
                        ( y1 >= y2 || abs(y2-y1) < 1 ) && ( (y1+h1) <= (y2+h2) || abs(y2+h2-y1-h2) < 1 ) ) { // 判断row_p2 是否是  row_p1 母行  
                        // # abs 的判断是因为子行和母行上面边或下面边重叠的时候, 坐标有可能不是完全相同, 有可能有微小误差
                    printf( "row id=%d 的母行  是 row id=%d\n", row_p1->id, row_p2->id );

                    row_p1->parent = row_p2;
                    row_p2->sonTotal ++;
                    break;      // 找到 最窄的那个母行就结束, 有且只有一个母行
                }
            }
        }

        // 2. 填充 子行 节点指针   数组, 同时修正母行的maxlines
        //  2019.03.18   只有cell在同一列的子行的maxlines 才可以合计， 否则就可能多算
        int     k;
        for( int i = 0; i < rowTotal; i ++ ) {
            row_p1 = o_rowMap_p[i];             // 这个赋值是为了提高下面代码的可读性
            if ( row_p1->sonTotal > 0 )  {           // 有子行
               row_p1->son = (ROW **)calloc( sizeof(ROW *) * row_p1->sonTotal, 1);
               
                //maxlines    = 0;
                k           = 0;        // 用来标识son 指针数组的下标
                
                for ( int j = 0; j < rowTotal, k < row_p1->sonTotal; j ++ ) {       // k < row_p1->sonTotal 是为了防止内存溢出
                    row_p2 = o_rowMap_p[j];
                    if ( row_p2->parent == row_p1 ) {     // 找见 子行 了
                        row_p1->son[k] = row_p2;
                        k ++;

                        //maxlines += row_p2->maxlines;       // 合计子行的 maxlines
                    }
                }

                // 修正 母行的maxlines
                //if ( maxlines > row_p1->maxlines ) {
                //    row_p1->maxlines = maxlines;
                //}

                // 给子行排序, 按照x 坐标从小到大, 暂时不排序, 因为后面填充row 的 c_list 的时候, 需要把母行的c_list 也要补充进去

            } 
        }
            
        free( o_rowMap_p );
    }

    

    // orderRowMap()
    //     将 rowMap 链表按照row 的h(高度)从小往大排序,  结果存放在 des 数组中
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
            i ++;
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
                i ++;
            } 
            step --;
        }
    }

    void printC_list( CELL **c_list, int total )
    {
        printf("--------------------adams-------printC_list() -------\n");
        for ( int i = 0; i < total; i ++ ) {
            printf("%d个 cell is %d \n", i,  c_list[i]->id);
        }
    }
    // 将c_list 指针数组排序 CELL **
    // 仍然用希尔排序法进行排序, 按照cell 的id 来进行排序
    void orderC_list( CELL **c_list, int total )
    { 
        int         i, step;
        CELL    *   cp;
        
        if ( !c_list )
            return;
    
        step = total / 2;
        while ( step > 0 ) {
            i = 0;
            while ( i < total - step ) {    // 根据步长(增量)进行比较交换排序
                if ( c_list[i]->id >c_list[i+step]->id ) {      // 前面的id 比后面的id 大
                    cp              = c_list[i];
                    c_list[i]       = c_list[i+step];
                    c_list[i+step]  = cp;
                }
                i ++;
            }
            step --;
        }
    }


    // 第二次遍历cellMap_p 中的cell, 为 col, row 中的c_list 填充cell id
    // fill_c_list() 仅仅是为了提高可读性而封装的代码
    //     仅被 preProcColRow() 调用
    //
    // 嵌套调用 修正 col的c_list, 保证先 修正 父col的c_list, 后 修正子 col的c_list
    void fixCol_c_list( COL * col_p )
    {
        int     i;
        
        if ( ! col_p )
            return;

        i = col_p->cellTotal;

        if ( col_p->c_list[i-1] )    // 如果最后一个位置已经被占用, 说明已经处理过了
            return;
            
        if ( col_p->parent ) {
                
            fixCol_c_list( col_p->parent );

            // 合并父col的c_list 到 子 c_list 中, 然后排序
            i = 0;
            while( col_p->c_list[i] && i < col_p->cellTotal )  // 因为子col的cellTotal 已经包含了父col的cellTotal, 这儿要跳过已经记录过的位置
                i ++;

            for ( int j = 0; j < col_p->parent->cellTotal && i+j < col_p->cellTotal ; j ++ ) 
                col_p->c_list[i+j] = col_p->parent->c_list[j]; 
        }
        // 下面进行排序
        orderC_list( col_p->c_list, col_p->cellTotal );
    }

    // 嵌套调用 修正 row的c_list, 保证先 修正 父row的c_list, 后 修正子 row的c_list
    void fixRow_c_list( ROW * row_p )
    {
        int     i;
        
        if ( ! row_p )
            return;

        i = row_p->cellTotal;

        if ( row_p->c_list[i-1] )    // 如果最后一个位置已经被占用, 说明已经处理过了
            return;
            
        if ( row_p->parent ) {
                
            fixRow_c_list( row_p->parent );
            // 合并父row的c_list 到 子 c_list 中, 然后排序
            
            i = 0;
            while( row_p->c_list[i] && i < row_p->cellTotal )  // 因为子row的cellTotal 已经包含了父row的cellTotal, 这儿要跳过已经记录过的位置
                i ++;

            for ( int j = 0; j < row_p->parent->cellTotal && i+j < row_p->cellTotal ; j ++ ) 
                row_p->c_list[i+j] = row_p->parent->c_list[j]; 
        }
        // 下面进行排序, 必须保证正确, 否则后面提取表格文本的时候会出错
        orderC_list( row_p->c_list, row_p->cellTotal );
    }

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
            col_p = getColByID( sheet_p->colMap_p, cp->col );
            
            if ( !col_p )
                return;
                       
            i = 0;
            while ( col_p->c_list[i] != 0 &&  i < col_p->cellTotal )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            col_p->c_list[i] = cp;

            row_p = getRowByID( sheet_p->rowMap_p, cp->row );
            
            if ( !row_p )
                return;

            i = 0;
            while ( row_p->c_list[i] != 0 )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            row_p->c_list[i] = cp;

            cp = cp->next;
        }

        // 下面进行修正, 将父c_list  补充到儿子的 c_list 中， 并进行排序
        col_p = sheet_p->colMap_p;

        while( col_p ) {
            i = col_p->cellTotal;
            
            if ( col_p->c_list[i-1] ) {   // 如果最后一个cell也已经记录了, 说明已经处理过了 
                col_p = col_p->next;
                continue;
            }
            
            fixCol_c_list( col_p );
            
            col_p = col_p->next;
        }

        row_p = sheet_p->rowMap_p;

        while( row_p ) {
            i = row_p->cellTotal;
            
            if ( row_p->c_list[i-1] ) {   // 如果最后一个cell也已经记录了, 说明已经处理过了 
                row_p = row_p->next;
                continue;
            }
            
            fixRow_c_list( row_p );
            
            row_p = row_p->next;
        }
    }
    


    // 为colMap和rowMap中的col, row 申请对应c_list 需要的空间
    //  spaceForColRowMap()  函数是为了提高可读性而封装的部分代码
    //      仅被 preProcColRow() 调用
    //  2019.03.14
    //      由于要把父col的c_list 也要合并加入, 所以, 申请空间要要考虑增加父col的cellTotal大小
    //
    // 嵌套函数, 保证先计算父col 的c_list空间, 后计算子 col 的c_list 空间(要包含父c_list 空间)
    void spaceForCol( COL * col_p )
    {
        int         cellTotal;

        cellTotal = col_p->cellTotal;

        if ( col_p->parent ) {
            spaceForCol( col_p->parent );
            
            cellTotal += col_p->parent->cellTotal;        // 加了父列的c_list 空间

            col_p->cellTotal = cellTotal;
        }
        col_p->c_list = (CELL **)calloc( sizeof(CELL*) * cellTotal, 1 ); 
    }

    // 嵌套函数, 保证先计算父row 的c_list空间, 后计算 子row  的c_list 空间(要包含父c_list 空间)
    void spaceForRow( ROW * row_p )
    {
        int         cellTotal;

        cellTotal = row_p->cellTotal;

        if ( row_p->parent ) {
            spaceForRow( row_p->parent );
            
            cellTotal += row_p->parent->cellTotal;        // 加了父列的c_list 空间

            row_p->cellTotal = cellTotal;
        }
        row_p->c_list = (CELL **)calloc( sizeof(CELL *) * cellTotal, 1 ); 
    }

    void spaceForTable( TABLE * table_p )
    {
        int         colTotal;
        int         rowTotal;

        colTotal = table_p->colTotal;
        rowTotal = table_p->rowTotal;

        table_p->col_list = (COL **)calloc( sizeof(COL *) * colTotal, 1 ); 
        table_p->row_list = (ROW **)calloc( sizeof(ROW *) * rowTotal, 1 ); 
    }

    void spaceForColRowMap( SHEET * sheet_p )
    {
        int             cellTotal;
        COL         *   col_p, * p_col_p;
        ROW         *   row_p, * p_row_p;
        TABLE       *   table_p, * p_table_p;

        if ( !sheet_p )
            return;

        col_p = sheet_p->colMap_p;
        
        while( col_p ) {
            if ( !col_p->c_list )       // 如果c_list 没有申请空间, 则申请. 因为spaceForCol() 是嵌套调用, 会将父col的c_list 空间申请了
                spaceForCol( col_p );
            col_p = col_p->next;
        }

        row_p = sheet_p->rowMap_p;
        
        while( row_p ) {
            if ( !row_p->c_list )       // 如果c_list 没有申请空间, 则申请. 因为spaceForRow() 是嵌套调用, 会将父row的c_list 空间申请了
                spaceForRow( row_p );
            row_p = row_p->next;
        }

        table_p = sheet_p->tableMap_p;

        while ( table_p ) {
            if ( !table_p->row_list )  // 没有在表中申请row , col 节点指针数组空间
                spaceForTable( table_p );

            table_p = table_p->next;          
        }

    }
    // newCol()
    //     注意, 这儿没有真正将cellID 记录到colMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该col
    // 仅 被  buildColRowMap() 调用, 创建一个COL 节点
    COL * newCol( SHEET *sheet_p, CELL * cp )
    {
        COL     * col_p, * lcp;
        TABLE   * table_p;

        if ( !sheet_p || !cp ) 
            return NULL;
        
        if ( !sheet_p->colMap_p ) {                             // colMap_p 是空的, 则是第一个col
            col_p = ( COL * )calloc( sizeof( COL ), 1 );

            col_p->id       = 1;                                // 第一列 id 为 1
            col_p->x        = cp->x,    col_p->w    = cp->w;
            col_p->table_id = cp->table_id;
            col_p->prev     = NULL, col_p->next = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            col_p->parent   = NULL, col_p->son  = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            
            col_p->maxlen   = cp->maxlen;
            col_p->cellTotal = 1;
                        
            sheet_p->colMap_p   = col_p;
            sheet_p->lcp        = col_p;
            sheet_p->colTotal   = 1;

            cp->col = 1;

            // 如果有需要就在这儿 table节点 colTotal ++, 然后最后还要申请空间并填充col_list..., 暂时没有处理
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->colTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            return col_p;
        } else {
            col_p = ( COL * )calloc( sizeof( COL ), 1 );
            
            lcp = sheet_p->lcp;                                 // 最后一个col pointer

            col_p->id       = lcp->id + 1;                            // 新id 为 前一个col的id + 1
            col_p->x        = cp->x,    col_p->w = cp->w;
            col_p->table_id = cp->table_id;
            col_p->prev     = lcp, col_p->next = NULL;            // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            col_p->maxlen = cp->maxlen;
            col_p->cellTotal = 1;                               // 对于新建的col而言,  这是第一个cell
            
            //col_p->parent = NULL, col_p->son = NULL;    // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            lcp->next = col_p;
            
            sheet_p->lcp = col_p;                               // 更新 last col pointer
            sheet_p->colTotal ++;               // 更新 shet 中的colTotal 数量, 但是这儿并没有记录具体的cellID, 后面需要根据该数值申请内存后再处理

            cp->col = col_p->id;

            // 如果有需要就在这儿 table节点 colTotal ++, 然后最后还要申请空间并填充col_list..., 暂时没有处理
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->colTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            
            return col_p;
        } 
        
        
    }

    // 查找列  映射中是否有复合挑件的列
    // 仅 被  buildColRowMap() 调用, 查找复合挑件的COL 
    COL * inColMap( COL * colMap_p, CELL * cp )
    {
        COL     *   col_p;
        float       x, w, cx, cw;     // col的x坐标与w宽度

        if ( !colMap_p )        // 如果 colMap_p 是空, 则肯定不包含
            return NULL;

        col_p = colMap_p;

        while ( col_p ) {
            x = cp->x,      w = cp->w;
            cx = col_p->x,  cw = col_p->w;
            
            if ( x == cx && w == cw && col_p->table_id == cp->table_id  ) {    // 说明该列已经存, 不但x,w 一样, 而且要同一个表才行
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
    ROW * newRow( SHEET *sheet_p, CELL *cp )
    {
        ROW     *   row_p, * lrp;
        TABLE   *   table_p;

        if ( !sheet_p ) 
            return NULL;

        printf("1111111111111111111111111\n");

        if ( !sheet_p->rowMap_p ) { // rowMap_p 是空的, 则是第一个row
            printf("这是第一个节点！y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );

            row_p->id       = 1;                            // 第一行 id 为 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->table_id = cp->table_id;
            row_p->prev     = NULL, row_p->next = NULL;     // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            row_p->parent   = NULL, row_p->son  = NULL;     // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            
            row_p->maxlines  = cp->txtTotal;                // 对于新建的row, 最大行数就是第一个cell的最大行数(txtTotals 里面已经把同行的合并了)
            row_p->cellTotal = 1;

            sheet_p->rowMap_p   = row_p;
            sheet_p->lrp        = row_p;
            sheet_p->rowTotal   = 1;

            cp->row = 1;
            
            // 如果有需要就在这儿 table节点 rowTotal ++, 然后最后还要申请空间并填充col_list..., 暂时没有处理
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->rowTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            
            return row_p;
        } else {
            printf("这是尾部添加的节点！y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );
            
            lrp = sheet_p->lrp;                             // 最后一个row pointer

            row_p->id       = lrp->id + 1;                      // 新id 为 前一个row的id + 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->table_id = cp->table_id;
            row_p->prev     = lrp,  row_p->next = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            row_p->parent = NULL, row_p->son = NULL;      // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            row_p->maxlines  = cp->txtTotal;               // 对于新建的row, 最大行数就是第一个cell的最大行数(txtTotals 里面已经把同行的合并了)
            row_p->cellTotal = 1;

            lrp->next = row_p;
            
            sheet_p->lrp = row_p;               // 更新 last row pointer
            sheet_p->rowTotal ++;               // 更新 sheet 中的rowTotal 数量, 但是这儿并没有记录具体的row 节点指针, 后面需要根据该数值申请内存后再处理

            cp->row = row_p->id;
            
            // 如果有需要就在这儿 table节点 rowTotal ++, 然后最后还要申请空间并填充col_list..., 暂时没有处理
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->rowTotal ++;

            return row_p;
        } 
    }

    // 仅 被  buildColRowMap() 调用, 查找复合挑件的ROW 
    ROW * inRowMap( ROW * rowMap_p, CELL * cp )
    {
        ROW     *   row_p;
        float       y, h, cy, ch;     // row的y坐标与h宽度

        if ( !rowMap_p )        // 如果 rowMap_p 是空, 则肯定不包含
            return NULL;

        row_p = rowMap_p;

        while ( row_p ) {
            y   = cp->y,    h   = cp->h;
            cy  = row_p->y, ch  = row_p->h;
            
            if ( y == cy && h == ch && row_p->table_id == cp->table_id ) {    // 说明该行已经存在
                return row_p;
            }
            row_p = row_p->next;
        }

        // 如果遍历完都没有找到, 则说明该cell所在的col 没有记录
        return NULL;
    }

    // newTable()
    //     注意, 这儿没有真正将cellID 记录到tableMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该table
    // 仅 被  buildColRowMap() 调用, 创建一个TABLE 节点
    //
    TABLE * newTable( SHEET *sheet_p, CELL *cp )
    {
        TABLE     * table_p, * ltp;

        if ( !sheet_p ) 
            return NULL;

        if ( !sheet_p->tableMap_p ) { // tableMap_p 是空的, 则是第一个table
            table_p = ( TABLE * )calloc( sizeof( TABLE ), 1 );

            table_p->id         = 1;                            // 第一行 id 为 1
            table_p->prev       = NULL, table_p->next = NULL;     // 申请内存时已经初始化为0了， 这儿赋值是为了可读性
            
            table_p->cellTotal  = 1;

            sheet_p->tableMap_p = table_p;
            sheet_p->ltp        = table_p;
            sheet_p->tableTotal = 1;

            cp->table_id        = 1;
            
            printf("这是第一个table节点！table_id=%d\n", table_p->id );
            return table_p;
        } else {
            table_p = ( TABLE * )calloc( sizeof( TABLE ), 1 );
            
            ltp = sheet_p->ltp;                             // 最后一个TABLE pointer

            table_p->id   = ltp->id + 1;                      // 新id 为 前一个table的id + 1
            table_p->prev = ltp,  table_p->next = NULL;    // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            table_p->cellTotal ++;

            ltp->next = table_p;
            
            sheet_p->ltp = table_p;               // 更新 last table pointer
            sheet_p->tableTotal ++;               // 更新 shet 中的tableTotal 数量

            cp->table_id = table_p->id;
            
            printf("这是尾部添加的table节点！table id = %d\n", table_p->id );
            printTableMap( sheet_p->tableMap_p );
            return table_p;
        } 
    }

    // 仅 被  buildColRowMap() 调用, 查找复合挑件的table
    // 判断当前cell 是否在已经存在的table里, 只需要与前一个cell进行比较
    //    
    TABLE * inTableMap( TABLE * tableMap_p, CELL * cp )
    {
        TABLE       *   table_p;
        float           px, py, pw, ph, x, y, w, h;     // py, ph 是前一个cell的, cell的y坐标与h宽度

        if ( !tableMap_p )          // 如果 tableMap_p 是空, 则肯定不包含
            return NULL;

        if ( !cp->prev )            // 如果cp 没有prev, 也就是cp 是第一个cell, 则肯定不包含
            return NULL;

        x = cp->x,          y   = cp->y,        w = cp->w,          h = cp->h, 
        x = cp->prev->x,    py  = cp->prev->y,  pw= cp->prev->w,    ph = cp->prev->h;
        
        if (  y > py+ ph || x > px+pw   )          // 如果与上一个cell 不在同一个表, 2个附近的cell 不相连, 则认为不是同一个表
            return NULL;

        // 到这儿说明与上一个cell 在同一个表, 找到这个table节点并返回
        table_p = tableMap_p;

        while ( table_p ) {
            if ( table_p->id == cp->prev->table_id  ) {    // 找到table节点
                return table_p;
            }
            table_p = table_p->next;
        }

        // 如果遍历完都没有找到, 则说明该cell所在的table 没有记录
        return NULL;
    }


    // 遍历一遍cellMap, 创建colMap, rowMap, tableMap 但是没有c_list, 因为没遍历完之前不知道有多少cell在col或row中, 
    // 无法为 c_list 申请空间
    //   buildColRowMap()  只被 preProcColRow() 调用
    // 调用了4个子函数
    //   inColMap(), newCol(),  inRowMap(), newRow() 
    void buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p )
    {
        CELL        *   cp;
        COL         *   col_p;
        ROW         *   row_p;
        TABLE       *   table_p;
        
        cp = cellMap_p;
        while( cp ) {
            while( cp->h < 1 || cp->w < 1 )            // 高度或宽度小于1 的都不处理, 一般是控制用的
                cp = cp->next;

            if ( !cp )
                break;
            
            // 2019.03.12  将tableMap 的构建也提前到这儿, 因为有可能一页有多个表, 如果比先把tableMap处理了, 后面的判断子列有可能是跨表格的
            // 这儿的tableMap 也仅仅是个表格编号, 不用记录长宽高等信息, 因为信息在 cellMap, colMap, rowMap 就足够处理了
            table_p = inTableMap( sheet_p->tableMap_p, cp );
            if ( !table_p ) {                       // cell 没有在现有tableMap中, 新建一个TABLE节点
                table_p = newTable( sheet_p, cp );
            }  else {                               // 该cell 所在的表格已经存在, 也就是与上一个cell的tableID 一样.
                cp->table_id = table_p->id;
                table_p->cellTotal ++;
            }
            
            col_p = inColMap( sheet_p->colMap_p, cp );
            
            if ( !col_p ) {                         // 如果该cell 所在列 不在colMap 中, 则加入colMap
                col_p = newCol( sheet_p, cp );              // 注意, 这儿没有真正将cellID 记录到colMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该col, 同时也更新了对应table中的相关信息
            } else {                                // cell 所在的列已经在colmap里记录了
                                                    // 同时在cell 中记录col 信息
                col_p->cellTotal ++;                // 增加列中的cell 计数
                //col_p->table_id = cp->table_id;   // 如果col 已经存在, 那么就已经登记过table信息了, 不用重新更新
                cp->col = col_p->id;
                
                                                    // 更新 col_p 的maxlen
                if ( col_p->maxlen < cp->maxlen ) 
                    col_p->maxlen = cp->maxlen;

            }

            printf("--------------------------cp->col=%d, cp->id=%d\n", cp->col, cp->id);
                                                    // 2. 处理 行row
            row_p = inRowMap( sheet_p->rowMap_p, cp );
            
            if ( !row_p ) {                         // 如果该cell 所在行 没有在rowMap中记录, 则增加该行在rowMap中的记录
                row_p = newRow( sheet_p, cp );
            } else {                                // 该cell所在的行已经在rowMap中了, row id 记录在 row_id变量中
                                                    // 在cell 中记录  row 信息
                row_p->cellTotal ++;
                //row_p->table_id = cp->table_id;   // 不用更新
                cp->row = row_p->id;
                
                if ( row_p->maxlines < cp->txtTotal )
                    row_p->maxlines = cp->txtTotal;
            }
            
            // 继续处理下一个cell
            cp = cp->next;

        }
    }

    COL * getColByID( COL * colMap_p, int  id )
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

    ROW * getRowByID( ROW * rowMap_p, int  id )
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
    
    TABLE * getTableByID( TABLE * tableMap_p, int  id )
    {
        TABLE     * table_p;
        
        if ( !tableMap_p )
            return NULL;
        
        table_p = tableMap_p;
        while ( table_p ) {
            if ( table_p->id == id )
                return table_p;
            table_p = table_p->next;
        }
        // 遍历完没有找到
        return NULL;
    }
    

    // # 5. 输出文本. 已经处理过同行多文本的拼接, 不再cell中的文本就是单独一行文本
    char * buildPageTxt( char * retbuf, int dlen, SHEET * sheet_p, CELL * cellMap_p, TEXT * textMap_p )
    {
        char        tmpbuf[512];            // 
        TEXT    *   tp, * ltp;              // ltp 用来存放表格的最后一个有效文本指针, 有可能是空
        int         table_id;
        int         len;
        CELL    *   cp;

        if ( !sheet_p || !textMap_p )       // cellMap_p 有可能为空, 没有表格的情况
            return NULL;

        tp = textMap_p;

        while ( tp ) {
            memset( tmpbuf, 0, 512 );
            memcpy( tmpbuf, tp->buf, tp->len );
            if ( tp->cellID == 0 )   {       // 不在表格中的文本 
                if ( strlen( rtrim(tmpbuf) ) == 0 )    // 空行, 要处理下空行的情况, 有可能全是空格
                    sprintf( retbuf, "%s\n\n", retbuf );
                else
                    sprintf( retbuf, "%s%s\n", retbuf, tp->buf );
            } else {                        // 表格中的文本
                cp = (CELL *)findCell( cellMap_p, tp->cellID );

                if ( !cp )
                    return NULL;

                len = strlen( retbuf );
                ltp = buildTableTxt( retbuf + len, dlen - len, sheet_p, cellMap_p, cp->table_id );  

                if ( ltp )  // 如果 表格的最后一个文本指针有效, 则赋值给tp,下面循环会处理tp->next. 否则就说明该表是个空表(极端情况)
                    tp = ltp;
            }
            
            tp = tp->next;
        }
        
        return retbuf;
    }

    TEXT * getLastTp( SHEET *sheet_p, CELL *cellMap_p, int table_id )
    {
        TEXT        * tp, * ptp;   // ptp 用来存上一行的最后一个有效文本指针
        ROW         * row_p;

        if ( !sheet_p || !sheet_p->tableMap_p || !cellMap_p || table_id < 1 )
            return NULL;

        return NULL;


    }

    TEXT * buildTableTxt( char * desbuf, int dlen, SHEET *sheet_p, CELL *cellMap_p, int table_id )
    {
        ROW     *   row_p;
        bool        flag = false;       // 是否建过表头的标识
        TEXT    *   tp = NULL;                 // 用来放置最后一个cell的最后一个TEXT指针, 返回方便后续处理
        TABLE   *   table_p;
        int         text_id;
        int         rowTotal;
        int         len;                // 用来记录已经处理的desbuf 长度

        if ( !desbuf || !sheet_p || !cellMap_p  ) {
            return NULL;
        }

        table_p = getTableByID( sheet_p->tableMap_p, table_id );

        if ( !table_p )
            return NULL;

        rowTotal = table_p->rowTotal;


        for ( int i = 0; i < rowTotal; i ++ ) {
            row_p = table_p->row_list[i];

            if ( row_p->sonTotal > 0)           // 不处理母行， 子行处理的时候就同时处理了母行的cell
                continue;

            len = strlen( desbuf );
            
            if ( !flag ) {              // # 如果没有建表头, 需要建表头
                flag = buildRowHeader( desbuf, dlen, cellMap_p, sheet_p, row_p );
                printf("table(%d)表头:%s, flag=%d\n", table_id,  desbuf, flag);
            } else {
                // 建表 中间分割线
                // #retbuf += "├──────────┼────┼────┼────┼────┼────┤ "
                buildRowSplit( desbuf+len, dlen-len, cellMap_p, sheet_p, table_p, i );
            }
            // buildRowBody( rowIndexMap, colIndexMap, item, textMap, cellMap, cellIndexMap )
        }

        /*
         * # 表格最下面的线
            lastRowId = rows[-1]
            while ( len( rowIndexMap[lastRowId]['sons'] ) > 0 ):   # 如果最后一行有子行, 那么就处理该行的最后一个子行
                lastRowId = rowIndexMap[lastRowId]['sons'][-1]
            retbuf += self.buildRowFooter( rowIndexMap, colIndexMap, lastRowId, cellMap, cellIndexMap )
         */

        printf("desbuf=\n%s\n", desbuf);

        return table_p->l_tp;
    }

    /*
    # 15.10 buildRowHeader()
    #      将表格里面的文本格式化输出
    # getRowSplitPos
    # 2017.01.11:
    #      修正实现方式, 根据新的rowIndexMap ={'cells':,'lines':,'parent':,'sons':[]}来进行处理
    #      只处理子行, 母行不用处理, 因为处理子行的时候顺便就把母行的内容处理了.
    #      header 的处理简单处理, 直接按照子行中的所有cell来进行处理(包含母行)
    #      也就是说实现方式其实没有改变.
    */
    #define LEFT_TOP        "┌"
    #define MID_TOP         "┬"
    #define RIGHT_TOP       "┐\n"
    #define MID_LEFT        "├"
    #define MID_MID         "┼"
    #define MID_RIGHT       "┤\n"
    #define LEFT_BOTTOM     "└"
    #define MID_BOTTOM      "┴"
    #define RIGHT_BOTTOM    "┘\n"
    #define ROWLINE         "─"
    #define COLLINE         "│"

    char * nchar( char * buf, int n, char ch )
    {

        if ( n < 1 )
            return NULL;
        
        for ( int i = 0; i < n; i ++ ) 
            buf[i] = ch;

        return buf;
    }

    char * nstr( char * buf, int buflen, int n, char  *str )
    {
        int     len;

        if ( !buf || n < 1 || !str )
            return NULL;

        if ( buflen < n*strlen(str) )
            return NULL;
        
        len = strlen( str );
        for ( int i = 0; i < n; i ++ ) {
            memcpy( buf + i * len, str, len );
        }

        return buf;
    }

    // 修正所在列 有子列的cell的连接线的长度
    int fixLinelen( COL * col_p )
    {
        int         ret;
        int         sonTotal;
        COL     *   s_col_p;
        int         i;

        if ( !col_p || col_p->sonTotal <= 0 ) 
            return 0;

        sonTotal = col_p->sonTotal;

        ret = sonTotal - 1;             // 子列数 - 1 就是需要增加的行 连接线 数量

        // 下面嵌套计算孙子列的子列, 修正当前列 的 行 连接线 数量
        i = 0;
        while ( i < sonTotal ) {
            s_col_p = col_p->son[i];
            if ( s_col_p->sonTotal > 0 ) 
                ret += fixLinelen( s_col_p );

            i ++;
        }

        return ret;
    }

    /*
     * # 15.12 getRowSplitPos()
     * #      获取指定行的分割线位置
     * #      第一个元素0 是为了方便计算列长, retlist[i] - retlist[i-1] 就是实际的列长
     */
/*
    def getRowSplitPos( self, rowIndexMap, colIndexMap, cellIndexMap, rowID ):
        retlist = [0]
        try:
            cols = len( rowIndexMap[rowID]["cells"] )
            col_pos = 0
            for i in range( 0, cols ):
                cellId = rowIndexMap[rowID]['cells'][i]          # rowIndexMap[rowID]的第0个元素是最大行数
                colId = cellIndexMap[cellId]['col']
                col_len = colIndexMap[colId][0]            # 当前列的最大宽度
                if ( i == 0 ):
                    col_pos = col_len
                else:
                    col_pos += col_len  + 1           # 加 1 是为了 修正分隔符所占的2位, 因为每一个都进行修正, 这样只需要加一次即可, 保证本次是真实的位置即可.以后的也只需要加一次
                retlist.append( col_pos )
        except:
            print("getRowSplitPos() Exception Error!")
            traceback.print_exc()
        return retlist
*/

    //             # ┌──────────┬────┬────┬────┬────┬────┐
    bool buildRowHeader( char * desbuf, int dlen, CELL * cellMap_p, SHEET * sheet_p, ROW * row_p )
    {
        int         col_pos;
        CELL    *   cp;
        COL     *   col_p;
        
        if ( !desbuf || !cellMap_p || !sheet_p || !row_p )
            return false; 

        if ( row_p->sonTotal > 0 )  // 这儿仅仅是容错, 调用本函数之前就应该做过该检查
            return false;
    
        strcpy( desbuf, LEFT_TOP );                             // "┌"
    
        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // 遍历行中的cell
            cp =  row_p->c_list[i];       
            col_p = getColByID( sheet_p->colMap_p, cp->col );     // 找到 cell对应的col节点

            if ( i != 0 )                                       // 不是第一个cell 的话, 都有中间间隔符 "┬"
                memcpy( desbuf+strlen(desbuf), MID_TOP, strlen(MID_TOP) );      // "┬"
            
            col_pos = col_p->maxlen;                            // 连线长度
            // 需要修正有子列的情况, 子列会多 分隔符的长度, 所以要增加  (子列数 -1)个"─" 
            // 2019.03.18 需要嵌套修正, 因为子列有可能还有子列.  尚未实现。。。。。
            //if ( col_p->sonTotal > 0 )
            //    col_pos += col_p->sonTotal - 1;

            col_pos += fixLinelen( col_p );

            nstr( desbuf + strlen(desbuf), dlen, col_pos, ROWLINE );       //  "─"
        }

        // 最后再增加一个结束符     RIGHT_TOP      
        memcpy( desbuf+strlen(desbuf), RIGHT_TOP, strlen(RIGHT_TOP) );      //  "┐"
        
        return true;
    }

    // 判断给定行中的cell 的列是否有给定 col 
    bool colInRow( SHEET * sheet_p, ROW * row_p, COL * col_p ) 
    {
        CELL    * cp;
        COL     * col_p1;

        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // 遍历行中的cell
            cp =  row_p->c_list[i];       
            col_p1 = getColByID( sheet_p->colMap_p, cp->col );     // 找到 cell对应的col节点
            if ( col_p1 == col_p )                  // 找到了相同的列
                return true;
        }   
        return false;
    }

    // 指定cell 是否在指定的row 中
    bool cellInRow( CELL * cp, ROW * row_p )
    {
        if ( !cp || !row_p || row_p->sonTotal <= 0 )    // 最后一个条件仅仅是容错
            return false;

        for ( int i = 0; i < row_p->cellTotal; i ++ ) {
            if ( cp == row_p->c_list[i] )
                return true;
        }

        return false;

    }

/*
    # 15.14 buildRowSplit()
    #      创建表格的行之间的分隔线
    # 2017.01.11:
    #      更改实现方式, rowIndexMap={'cells':,'lines':,'parent':,'sons':}
    #      调用这个方法之前已经确认这个就是子行了，
    #      获取的前一行要进一步判断是否是同一parent的子行, 如果不是, 需要进一步判断有子行,
    #      嵌套处理, 获取最后一个子行, 这个子行才是真正的"前一行"
    # 2019.03.17:
    //     属于table 的row 指针节点全部存放在row_list 指针数组中. 所以直接处理即可
    //     改变实现思路, 与上一行的col 进行比较, 来确定分隔符
    //     由于有创建表头的行, 所以必然存在 不是母行的子行是该行的前一行
    */
    //            "├──────────┼────┼────┼────┼────┼────┤ "
    bool buildRowSplit( char * desbuf, int dlen, CELL * cellMap_p, SHEET * sheet_p, TABLE * table_p, int rowNo )
    {
        int         col_pos;
        CELL    *   cp;
        COL     *   col_p, * p_col_p;       // p_col_p 指向当前列的前一列
        ROW     *   row_p, * p_row_p;
        int         j, k;
        bool        f_p_in_parent, f_in_parent, f_n_parent;  // 前一个cell, 当前cell, 后一个cell 是否在母行中的标识

        if ( !desbuf || !cellMap_p || !sheet_p || !table_p || rowNo < 1 )     // < 1 是因为 只有包括第二行之后的行才会有 行分隔符
            return false; 

        // 找到上一个有效的子行
        row_p   = table_p->row_list[rowNo];     // rowNo 是从0开始的
        
        j = rowNo-1;
        while ( j >= 0 ) {
            p_row_p = table_p->row_list[j];      // 前一行
            if ( p_row_p->sonTotal == 0 )                       // 找到了, 不是母行就可以与当前行进行比较, 因为母行不处理
                break;
            j --;
        }
        
        if ( p_row_p->sonTotal > 0 || row_p->sonTotal > 0 )   // 容错
            return false;
        

        // 1. 起始分隔符。
        //    这儿要判断第一个cell 是否是母行的, 且本行不是母行的第一个 子行, 那么就是  COLLINE         "│"
        //    否则就是  MID_LEFT         "├" 
        //    
        if ( row_p->parent ) {       // 有母行
            cp =  row_p->c_list[0];       
            if ( cp == row_p->parent->c_list[0] && row_p != row_p->parent->son[0] )   // 第一个cell是母行的, 且本行不是第一个子行
                strcpy( desbuf, COLLINE );                             //  "│"
            else
                strcpy( desbuf, MID_LEFT );                             // "├"
        } else
            strcpy( desbuf, MID_LEFT );                             // "├"

   
        // 2.下面的判断还需要细化 
        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // 遍历行中的cell
            cp =  row_p->c_list[i];       

            // 2.1 当前cell 在母行中就有, 当前行 是第一个子行     // "┼" 
            // 2.2 当前cell 在母行中就有, 当前行不是第一个子行    // "┤"
            // 2.3 
            col_p = getColByID( sheet_p->colMap_p, cp->col );     // 找到 cell对应的col节点

            if ( i != 0 ) {                                      // 不是第一个cell 的话, 都有中间间隔符 "┼" 或  "┬"
                // col_p 是否在p_row_p, 如果在, 则 "┼", 不在上一行 则 "┬"
                if ( colInRow( sheet_p, p_row_p, col_p ) )         // 该列在上一行就有， 则用间隔符"┼"
                     memcpy( desbuf+strlen(desbuf), MID_MID, strlen(MID_MID) );      // "┼"   
                else {
                    cp = row_p->c_list[i-1];
                    p_col_p = getColByID( sheet_p->colMap_p, cp->col ); 
                    
                    if ( colInRow( sheet_p, p_row_p, p_col_p ) )  // 如果前一列在上一行有, 则还应该用   MID_MID      // "┼"
                        memcpy( desbuf+strlen(desbuf), MID_MID, strlen(MID_MID) );      // "┼"   
                    
                    else 
                        memcpy( desbuf+strlen(desbuf), MID_TOP, strlen(MID_TOP) );      // "┬"
                }
            }
            
            col_pos = col_p->maxlen;                            // 连线长度
            // 需要修正有子列的情况, 子列会多 分隔符的长度, 所以要增加  (子列数 -1)个"─" 
            // 这儿的修正要 单独处理， 嵌套处理, 因为子列有可能还有子列。。。。
            col_pos += fixLinelen( col_p );
            nstr( desbuf + strlen(desbuf), dlen, col_pos, ROWLINE );       //  "─"
        }

        // 5. 结束分隔符。
        //    这儿要判断最后一个cell 是否是母行的, 且本行不是母行的第一个 子行, 那么就是  COLLINE         "│"
        //    否则就是  MID_RIGHT         "┤"
        if ( row_p->parent ) {       // 有母行
            j = row_p->cellTotal;
            cp =  row_p->c_list[j-1];   

            k =  row_p->parent->cellTotal;  

            if ( cp == row_p->parent->c_list[k-1] && row_p != row_p->parent->son[0] )   // 最后一个cell是母行的, 且本行不是第一个子行
                strcpy( desbuf+strlen(desbuf), COLLINE );                             //  "│"
            else
                strcpy( desbuf+strlen(desbuf), MID_RIGHT );                             // "┤"
        } else
            strcpy( desbuf+strlen(desbuf), MID_RIGHT );                             // "┤"

        return true;
    }


