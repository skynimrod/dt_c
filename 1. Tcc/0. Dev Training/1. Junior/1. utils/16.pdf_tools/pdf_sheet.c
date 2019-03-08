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
     
        /*       
        // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
        //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
        self.preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
        
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

        freeSheet( sheet_p );
        
        return NULL;
    }

    void freeCOL( COL * col_p )
    {
        if ( !col_p )
            return;

        if ( col_p->cellTotal > 0 )
            free( col_p->c_list );

        if ( col_p->sonTotal > 0 )
            free( col_p->son );

        free( col_p );
    }

    void freeROW( ROW * row_p )
    {
        if ( !row_p )
            return;

        if ( row_p->cellTotal > 0 )
            free( row_p->c_list );

        if ( row_p->sonTotal > 0 )
            free( row_p->son );

        free( row_p );
    }

    void freeTable( TABLE * table_p )
    {
        if ( !table_p )
            return;

        if ( table_p->cellTotal > 0 )
            free( table_p->c_list );

        if ( table_p->colTotal > 0 )
            free( table_p->col_list );

        if ( table_p->rowTotal > 0 )
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

        while ( !col_p ) {
            cw = col_p->x,  cw = col_p->w;
            
            if ( x == cw && w == cw  ) {    // 说明该列已经存
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

        if ( !sheet_p->rowMap_p ) { // rowMap_p 是空的, 则是第一个row
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
            return;
        } else {
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );
            
            lrp = sheet_p->lrp;                             // 最后一个col pointer

            row_p->id   = lrp->id + 1;                      // 新id 为 前一个col的id + 1
            row_p->y    = cp->y,    row_p->h    = cp->h;
            row_p->prev = lrp,  row_p->next = NULL;         // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            //col_p->parent = NULL, col_p->son = NULL;      // 申请内存时已经初始化为0了， 这儿赋值是为了可读性

            row_p->maxlines  = cp->txtTotal;               // 对于新建的row, 最大行数就是第一个cell的最大行数(txtTotals 里面已经把同行的合并了)
            row_p->cellTotal = 1;

            lrp->next = row_p;
            
            sheet_p->lrp = row_p;               // 更新 last row pointer
            sheet_p->rowTotal ++;               // 更新 shet 中的rowTotal 数量, 但是这儿并没有记录具体的cellID, 后面需要根据该数值申请内存后再处理
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

        while ( !row_p ) {
            cy = row_p->y,  ch = row_p->h;
            
            if ( y == cy && h == ch  ) {    // 说明该列已经存
                return row_p;
            }
            row_p = row_p->next;
        }

        // 如果遍历完都没有找到, 则说明该cell所在的col 没有记录
        return NULL;
    }


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
        //fixColMaxlen( cellIndexMap, colMap, colIndexMap, rowIndexMap )
        // 修正 row 中的maxlines, 主要是根据 儿子row 来修正 父 row 的行数
        //fixRows( cellMap, cellIndexMap, rowMap, colIndexMap, rowIndexMap )
        return;
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
    void fixColMaxlen( CELL *cellMap_p, SHEET * sheet_p )
    {
        if ( !cellMap_p || ! sheet_p )
            return;
        
        if ( !sheet_p->colMap_p )
            return;
        
        int     colTotal = sheet_p->colTotal;
        COL     o_colMap[ sheet_p->colTotal ];
        
        // 先把col 进行从窄到宽 排序, 因为只有宽的col才能是窄的col的父col 
        //orderColMapByWidth( &o_colMap, sheet_p );      // 从窄到宽对 colMap_p 排序, 结果存放在 o_colMap
        //x1, x2 分别代表 col 的2条边的位置(x1=x, x2 = x+ w)
        
        
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

        cp = cellMap_p;
        while ( cp ) {
            col_p = getColOfID( sheet_p->colMap_p, cp->col );
            i = 0;
            while ( col_p->c_list[i] != 0 )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            col_p->c_list[i] = cp->id;

            row_p = getRowOfID( sheet_p->rowMap_p, cp->row );
            i = 0;
            while ( row_p->c_list[i] != 0 )      // 寻找c_list第一个可用的位置, 用来存放cell ID
                i ++;
            
            row_p->c_list[i] = cp->id;
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
            if( cp->h < 1 || cp->w < 1 )            // 高度或宽度小于1 的都不处理, 一般是控制用的
                cp = cp->next;

            x = cp->x, y = cp->y, w = cp->w, h = cp->h;
                                                    // 1. 先处理列row
            col_p = inColMap( sheet_p->colMap_p, x, w );
            
            if ( !col_p ) {                         // 如果该cell 所在列 不在colMap 中, 则加入colMap
                newCol( sheet_p, cp );              // 注意, 这儿没有真正将cellID 记录到colMap中, 因为只有遍历完一遍cellMap才知道有多少cell在该col
                cp->col = sheet_p->lcp->id;         // 新建的col id就是  lcp 的id(  只有一个col 节点)
            } else {                                // cell 所在的列已经在colmap里记录了
                                                    // 同时在cell 中记录col 信息
                cp->col = col_p->id;      

                col_p->cellTotal ++;                // 增加列中的cell 计数
                
                                                    // 更新 col_p 的maxlen
                if ( col_p->maxlen < cp->maxlen ) 
                    col_p->maxlen = cp->maxlen;
            }
            
                                                    // 2. 处理 行row
            row_p = inRowMap( sheet_p->rowMap_p, y, h );
            
            if ( !row_p ) {                         // 如果该cell 所在行 没有在rowMap中记录, 则增加该行在rowMap中的记录
                newRow( sheet_p, cp );
                cp->row = sheet_p->lrp->id;         // 只有一个row 节点
            } else {                                // 该cell所在的行已经在rowMap中了, row id 记录在 row_id变量中
                                                    // 在cell 中记录  row 信息
                cp->row =row_p->id;

                row_p->cellTotal ++;
                
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
