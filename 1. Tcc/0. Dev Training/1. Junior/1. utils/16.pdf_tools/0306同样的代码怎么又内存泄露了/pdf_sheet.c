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
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "zlib.h"
    #include "pdf_tools.h"

    char *  procText4Table( TEXT * textMap_p, DECODE * decode_p );
    

    void preProcTxtMap( TEXT * textMap_p, DECODE * decode_p );
    void preProcTxtMap4Cell( TEXT * textMap_p, DECODE * decode_p );
    /*
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap );
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    */
    char * procText4Table( TEXT * textMap_p, DECODE * decode_p )
    {
        // # 1. 处理textMap, 将不在表格中的文本进行处理, 如果是同一行的就进行合并, 同时将cell中的txtIDs_p 补充完整
        //preProcTxtMap( textMap_p, decode_p );
                
        // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
        //preProcTxtMap4Cell( textMap_p, cellMap_p );
     
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
        return NULL;
    }

    
    // 删除当前的节点
    void delCurTxt( TEXT * tp )
    {
        TEXT    * tpp, * tpn;    // 当前节点的前一个节点和后一个节点
        
        if ( !tp ) 
            return;
        
    /*
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
*/
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

        /*
        printf("合并前, tp1->buf=%s, tp1->len=%d, tp1->id=%d, tp2->buf=%s, tp2->len=%d, tp2->id=%d\n", 
                        tp1->buf, tp1->len, tp1->id, tp2->buf, tp2->len, tp2->id );

        // 先为合并后的文本重新申请空间并保存合并后的文本, 然后释放之前的2个文本内存空间
        buf = ( char * )calloc( tp1->len + tp2->len + 1, 1 );   // +1 是为了存放尾0
        memcpy( buf, tp1->buf, tp1->len );
        memcpy( buf+tp1->len, tp2->buf, tp2->len );

        free( tp1->buf );
        */
        
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
    void preProcTxtMap( TEXT * textMap_p, DECODE * decode_p )
    {
        TEXT        * tp, * tpp;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点
        CELL        * cp;

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标
        int         txtRows;            // 

        if ( !textMap_p )
            return ;

        txtRows = decode_p->txtRows;        // 未处理之前的

        /*
        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
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
            
            if ( abs( py - oy ) < 1 ) {     //  # 两个Y坐标小于1视为同一行

                
                if ( px <= ox ) {        // 留小不留大, 小的在前面
                    // tpp 小, 保留tp->prev
                    combineBuf( tpp, tp );

                    delCurTxt( tp );                    // 删除tp 节点
                    tp = tpp->next;                     // tp 重新定位到下一个需要处理的节点
                } else {
                    // tp 小，保留 tp
                    combineBuf( tp, tpp );


                    delCurTxt( tpp );                    // 删除tpp 节点
                    
                    tp = tp->next;                      // tp 重新定位到下一个需要处理的节点 
                }
            } else {
                printf("不用合并！\n");
                tp = tp->next;
            }
        }     

        // 2. 下面为单元格 的文本编号记录申请空间(txtIDs_p)
        if ( decode_p->c_cursor == 0 )
            return;

        for ( int i = 0; i < decode_p->c_cursor; i ++ ) {
            if ( decode_p->cellMap[i].txtTotal != 0 )
                decode_p->cellMap[i].txtIDs_p = (int *)calloc( sizeof(int), decode_p->cellMap[i].txtTotal );   // 为存放记录cell中的文本编号申请空间
        }

        */
        printf("preProcTxtMap() end \n");
    }

    // 从当前节点往前查找指定ID的 TEXT
    TEXT * searchForward( TEXT * textMap_p, int id )
    {
        TEXT        * tp;

        /*
        if ( !textMap_p ) 
            return NULL;

        tp = textMap_p;

        while ( tp ) {
            if ( tp->id == id )
                return tp;

            tp = tp->prev;
        }

        */
        // 向前遍历完后没找到
        return NULL;
    }


    // # 2. 预处理 表格中的文本, 将原本一行的多个文本拼接为一个文本, 删除多余的文本. 同时修正cell中的txtIDs_p 中的文本编号信息
    // 处理思路:
    //    1. 遍历一遍所有的text节点, 跳过不在cell中的节点
    //    2. 对于在cell中的文本节点, 找到对应的cell, 然后与该cell中的之前的文本进行比较, 看是否需要合并文本(同一行)
    void preProcTxtMap4Cell( TEXT *textMap_p, DECODE * decode_p )
    {
        TEXT        * tp, * tpp, * tpn;    // tp 存放当前节点指针, tpp 是当前节点的前一个节点, tpn 是下一个节点
        CELL        * cp;
        int         cell_id;   
        int         text_id;  

        float       ox, oy, px, py;     // 记录当前文本的坐标和前一个文本的坐标

        int         i, j, k;                  // 数组下标

        /*
        if ( !textMap_p || decode_p->c_cursor == 0 )
            return ;

        tp = textMap_p;

        while ( tp ) {
            printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            j = tp->cellID;
            if ( j == 0 ) {   // 等于0, 表示该文本没有在cell中, 跳过不处理
                tp = tp->next;
                continue;        
            }

            for ( i = 0; i < decode_p->cellMap[j].txtTotal; i ++ ) 
                if ( decode_p->cellMap[j].txtIDs_p[i] == 0 )   // 碰到结尾了
                    break;
            
            if ( i == 0 ) { // 表示该文本是cell 中的第一个文本, 记录在txtIDs_p[0] 中, 不做合并文本处理
                decode_p->cellMap[j].txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {   // 容错处理,表示预申请空间已经存满了, 那么当前的这个文本就不应该在该cell中
                return NULL;
            } else  {       // 下面是进行判断, 如果需要合并就合并文本
                text_id = decode_p->cellMap[j].txtIDs_p[i-1];      // 同一个cell中的前一个文本
                tpp = searchForward( tp, text_id );
                if ( !tpp )                     // 容错处理 
                    return NULL;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {     //  # 两个Y坐标小于1视为同一行
                    // 合并文本, 保留 tpp, 因为tpp已经记录了在cell中了
                    combineBuf( tpp, tp );
                    if ( tpp->len > decode_p->cellMap[j].maxlen )    // 更新cell中的最大文本长度
                        decode_p->cellMap[j].maxlen = tpp->len;

                    tpn = tp->next;
                    delCurTxt( tp );                    // 删除tpp 节点
                    
                    tp = tpn;                      // tp 重新定位到下一个需要处理的节点 
                } else {    // 不需要合并
                    decode_p->cellMap[j].txtIDs_p[i] = tp->id;       // 添加cell中的文本记录
                    tp = tp->next;
                }
            }

        }

        */

        return;
    }
    /*
    // # 3. 预处理 表格, 根据坐标关系归类到列与行, 相关信息存放在 colMap, colIndexMap, rowMap, rowIndexMap
    colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # 字典不能作为key, 只能再建立一个colIndexMap来记录每个col对应的cell集合
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
    {
        return;
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
*/
