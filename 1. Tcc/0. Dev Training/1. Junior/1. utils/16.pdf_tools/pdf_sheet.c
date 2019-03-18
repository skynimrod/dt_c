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

    // buildColRowMap(), spaceForColRowMap(), fill_c_list()  ���� preProcColRow() ����
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
        SHEET       * sheet_p;          // ��, ��¼��������Ϣ

        sheet_p = (SHEET *)calloc( sizeof(SHEET), 1 );


        // # 1. ����textMap, �����ڱ���е��ı����д���, �����ͬһ�еľͽ��кϲ�, ͬʱ��cell�е�txtIDs_p ��������
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ
        preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        // # 3. Ԥ���� ���, ���������ϵ���ൽ������, 
        preProcColRow( cellMap_p, sheet_p );
        
        /*       
        // # 4. �����ı���cell�Ĺ�ϵ, ��ֳ�table ����, ����tableMap={1:[cellid_list]}�� ͬʱ��cellMap �����tableid ����
        tableMap = {}
        self.buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
        print("--after buildTableMap()---cellIndexMap-----------")
        print(cellIndexMap)
        print(tableMap)

        // # 5. ����ı�. �Ѿ������ͬ�ж��ı���ƴ��, ����cell�е��ı����ǵ���һ���ı�
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
        
        // 1. �ͷ� ��col  ��ؽڵ�
        if ( sheet_p->colMap_p ) {
            col_p = sheet_p->colMap_p;       // ��ǰ����ɾ���ڵ�
            while( col_p ) {
                tmp_col_p = col_p->next;
                freeCOL( col_p );
                col_p = tmp_col_p;
            }
        }

        // 2. �ͷ� ��row  ��ؽڵ�
        if ( sheet_p->rowMap_p ) {
            row_p = sheet_p->rowMap_p;            // ��ǰ����ɾ���ڵ�
            while( row_p ) {
                tmp_row_p = row_p->next;        // �ȱ�������һ���ڵ�ĵ�ַ
                freeROW( row_p );    
                row_p = tmp_row_p;
            }
        }

        // 3. �ͷ� ��� table  ��ؽڵ�
        if ( sheet_p->tableMap_p ) {
            table_p = sheet_p->tableMap_p;      // ��ǰ����ɾ���ڵ�
            while( table_p ) {
                tmp_table_p = table_p->next;        // �ȱ�������һ���ڵ�ĵ�ַ
                freeTable( table_p );
                table_p = tmp_table_p;
            }
        }
    }
    
    // ɾ����ǰ�Ľڵ�
    void delCurTxt( TEXT * tp )
    {
        TEXT    * tpp, * tpn;    // ��ǰ�ڵ��ǰһ���ڵ�ͺ�һ���ڵ�
        
        if ( !tp ) 
            return;
        
        tpp = tp->prev;
        tpn = tp->next;

        free( tp->buf );
        free( tp );

        if ( !tpp ) {       // tpp Ϊ��, ˵��Ҫɾ����tp�ǵ�һ���ڵ�, ���һ���ڵ��ɵ�һ��
            tp = tpn;
            tp->prev = NULL;
            return;
        }
        
        if ( !tpn ) {       // ���tpnΪ��, ˵��Ҫɾ����tp �����һ���ڵ�
            tpp->next = NULL;
            return;
        }

        tpp->next = tpn;
        tpn->prev = tpp;

    }    

    // �ϲ�2���ı��ڵ��е��ı����ݵ���һ���ı��ڵ��buf��, 
    // ֻ��Ϊ�˼򻯴��룬���ӿɶ��Զ���������ĺ���
    void combineBuf( TEXT * tp1, TEXT * tp2 )
    {
        char        * buf;      // ��������ռ䲢��źϲ�����ı���Ϣ

        if ( ! tp1 || !tp2 ) {
            printf("�п�ָ�룡����\n");
            return ;
        }

        //printf("�ϲ�ǰ, tp1->buf=%s, tp1->len=%d, tp1->id=%d, tp2->buf=%s, tp2->len=%d, tp2->id=%d\n", 
        //                tp1->buf, tp1->len, tp1->id, tp2->buf, tp2->len, tp2->id );

        // ��Ϊ�ϲ�����ı���������ռ䲢����ϲ�����ı�, Ȼ���ͷ�֮ǰ��2���ı��ڴ�ռ�
        buf = ( char * )calloc( tp1->len + tp2->len + 1, 1 );   // +1 ��Ϊ�˴��β0
        memcpy( buf, tp1->buf, tp1->len );
        memcpy( buf+tp1->len, tp2->buf, tp2->len );

        free( tp1->buf );
        
        tp1->buf = buf;
        tp1->len += tp2->len;
        
        printf("�ϲ��������Ϊ:%s(textID=%d), len=%d\n", tp1->buf,tp1->id, tp1->len);
    }


    // # 1. Ԥ����textMap, �����ٱ���е��ı����д���, �����ͬһ�еľͽ��кϲ�
    //# 15.3 preProcTxtMap()
    //#     Ԥ����textMap, �����ڲ��ڵ�Ԫ��������ı����д���,  ��ԭ����һ�е�ƴ����һ��, ɾ��������ı����, ��������
    //#     ҳü�ڵ��ı�Ҳ����ƴ�Ӵ���
    //# depends on:
    //#       delTxt( item, textMap, cellIndexMap )
    //# 2017.01.10:
    //#     ����ʵ�ַ�ʽ, ѭ������ oy ����������´���
    //# 2019.02.24
    //      ����C���԰汾�� ����cellMap_p ��Ԥ����, Ҳ����Ϊ���ı���cell��txtIds_p ����ռ�
    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp;    // tp ��ŵ�ǰ�ڵ�ָ��, tpp �ǵ�ǰ�ڵ��ǰһ���ڵ�
        CELL        * cp;

        float       ox, oy, px, py;     // ��¼��ǰ�ı��������ǰһ���ı�������

        if ( !textMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            //printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID != 0 ) {   // ������0, ��ʾ���ı���cell��, ������������
                tp = tp->next;
                continue;        
            }
            
            if ( !tp->prev )  {          // ���û��ǰһ���ı����Ͳ�����, Ҳ���ǵ�һ���ı�
                tp = tp->next;
                continue;
            }

            tpp = tp->prev;             // ��tpp ��Ϊ����ߴ���ɶ���, ��Ȼ�༶ָ�뿴�����Ѿ�

            ox = tp->ox,        oy = tp->oy;
            px = tpp->ox,       py = tpp->oy;

            printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
            if ( abs( py - oy ) < 1 ) {     //  # ����Y����֮��С��1��Ϊͬһ��, ����ǰһ���ڵ�, ��ǰ�ڵ��������ӵ�ǰһ���ڵ���, ɾ����ǰ�ڵ�
                combineBuf( tpp, tp );
                delCurTxt( tp );                    // ɾ��tp �ڵ�
                tp = tpp->next;                     // tp ���¶�λ����һ����Ҫ����Ľڵ�
            } else {
                printf("���úϲ���\n");
                tp = tp->next;
            }
        }     

        // 2. ����Ϊcell ���ı�ָ������ ����ռ�(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal > 0 )
                cp->tps = (TEXT **)calloc( sizeof(TEXT*), cp->txtTotal );   // Ϊ��ż�¼cell�е��ı��������ռ�
            cp = cp->next;
        }

        printf("preProcTxtMap() end \n");
    }

    // �ӵ�ǰ�ڵ���ǰ����ָ��ID�� TEXT
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

        // ��ǰ�������û�ҵ�
        return NULL;
    }

    // �������û���ҵ�ָ��id��CELL
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

        // �������û�ҵ�
        return NULL;
    }

    // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ, txtTotalҲҪ����
    // ����˼·:
    //    1. ����һ�����е�text�ڵ�, ��������cell�еĽڵ�
    //    2. ������cell�е��ı��ڵ�, �ҵ���Ӧ��cell, Ȼ�����cell�е�֮ǰ���ı����бȽ�, ���Ƿ���Ҫ�ϲ��ı�(ͬһ��)
    void preProcTxtMap4Cell( TEXT *textMap_p, CELL * cellMap_p )
    {
        TEXT        * tp, * tpp, * tpn;    // tp ��ŵ�ǰ�ڵ�ָ��, tpp �ǵ�ǰ�ڵ��ǰһ���ڵ�, tpn ����һ���ڵ�
        CELL        * cp;
        int         cell_id;   
        int         text_id;  

        float       ox, oy, px, py;     // ��¼��ǰ�ı��������ǰһ���ı�������

        int         i;                  // �����±�

        if ( !textMap_p || !cellMap_p )
            return ;

        tp = textMap_p;

        while ( tp ) {
            //printf("--------address  tp->prev=%p, tp=%p, tp->next=%p\n", tp->prev, tp, tp->next );
            if ( tp->cellID == 0 ) {                // ����0, ��ʾ���ı�û����cell��, ����������
                tp = tp->next;
                continue;        
            }

            cp = getCellByID( cellMap_p, tp->cellID );
            // ��ʱ, ���ı���id ��û�д����cell �е�tps ������, ֮ǰ�Ĵ���������������㹻�Ĵ�ſռ�
            // ������㴦��㽫�������ı�ָ���ŵ�tps ������
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->tps[i] == 0 )         // ������β��
                    break;
            
            if ( i == 0 ) {                    // ��ʾ���ı���cell �еĵ�һ���ı�, ��¼��tps[0] ��, �����ϲ��ı�����
                cp->tps[i] = tp;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {       // �ݴ���,��ʾԤ����ռ��Ѿ�������, ��ô��ǰ������ı��Ͳ�Ӧ���ڸ�cell��
                return ;
            } else  {
                tpp = cp->tps[i-1];        //��ͬһ��cell�е��ı��ڵ��ǰһ���ı��ڵ���
                if ( !tpp )                         // �ݴ���(���������Ӧ�÷���)
                    return ;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {         //  # ����Y����С��1��Ϊͬһ��
                                                    // �ϲ��ı�, ���� tpp, ��Ϊtpp�Ѿ���¼����cell����
                    combineBuf( tpp, tp );
                    if ( tpp->len > cp->maxlen )    // ����cell�е�����ı�����
                        cp->maxlen = tpp->len;

                    tpn = tp->next;                 // ��¼��һ���ı��ڵ��ַ
                    delCurTxt( tp );                // ɾ��tpp �ڵ�
                    
                    tp = tpn;                       // tp ���¶�λ����һ����Ҫ����Ľڵ�
                    cp->txtTotal --;                // ���� cell �е��ı���������
                } else {                            // ����Ҫ�ϲ�
                    cp->tps[i] = tp;            // ���cell�е�textָ���¼
                    tp = tp->next;
                }
            }

        }


        return;
    }

    // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
    //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
    // �������е�cell, ������(x,w) ���� col, ���� (y,h) ���� row


    // ʵ��˼·:
    //    ����2��cellMap_p �е�����cell, ��һ���Ǵ���cell ������col, row, �γ�����
    //    �ڶ��α��� �Ǹ��ݵ�һ�α�����õ�col, row �ڵ�cell ����, �����Ӧ���ڴ�, 
    //    Ȼ���cell id ��䵽 col, row �ڵ�c_list ������.
    // ����5���ֲ�����
    //   buildColRowMap(), buildColRowMap(), fill_c_list(), fixColMaxlen()
    void preProcColRow( CELL *cellMap_p, SHEET * sheet_p )
    {
        if ( !sheet_p || !cellMap_p )
            return;

        // ��һ�α���cellMap, ����colMap, rowMap, ����û��c_list, ��Ϊû������֮ǰ��֪���ж���cell��col��row��, 
        // Ϊ c_list �޷�����ռ�
        buildColRowMap( cellMap_p, sheet_p );

        // ���� Col �е�maxlen, ��Ҫ�Ǹ�����col ������ �� col �Ŀ��

        printf("==============================aaaaaaaaaaaaa=================\n");
        printColMap( sheet_p->colMap_p );
        printRowMap( sheet_p->rowMap_p );

        // ���col �е�son, parent
        fillColSon( cellMap_p, sheet_p );
        // ���row �е�son, parent
        fillRowSon( cellMap_p, sheet_p );

        // ΪcolMap��rowMap�е�col, row �����Ӧc_list ��Ҫ�Ŀռ�
        spaceForColRowMap( sheet_p );
        
        // �ڶ��α���cellMap_p �е�cell, Ϊ col, row �е�c_list ���cell id
        fill_c_list( cellMap_p, sheet_p );

        fixColMaxlen( cellMap_p, sheet_p );
        fixRowMaxlines( cellMap_p, sheet_p );

        printf("==============================bbbbbbbbbbbb=================\n");
        printColMap( sheet_p->colMap_p );
        printRowMap( sheet_p->rowMap_p );
        printTableMap( sheet_p->tableMap_p );
        printf("==============================cccccccccccccc=================\n");

        // ��� TABLE��col_list, row_list��Ϣ, ���� cellMap_p�� textMap_p ��Ϊ�˻�ȡ�ñ�����һ��text pointer
        fillColRow_List( sheet_p );

        printTableMap( sheet_p->tableMap_p );
        return;
    }

    // getLastTpofRow()
    // ��ȡ���е����һ���ı�ָ��, ��ʵ�������һ��cell �����һ��txtID
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
        
        while ( i > 0 ) {       // �п���cell �ǿյ�, ����Ҫ�Ӻ���ǰ������row��cell
            cp = row_p->c_list[ i - 1 ];     // �� row �����һ��cp

            printf("row (%d) ���һ��cell(%d)\n", row_p->id, cp->id);

            j = cp->txtTotal;

            if ( j > 0 ) {
                printf("cell (%d) ���һ��txt(%d)\n", cp->id, cp->tps[j-1]->id );
                return cp->tps[ j - 1];        // cel �е����һ��text pointer
            } else {
                i --;
            }
        }
        return NULL;
    }

    // ��� ���е�col_list, row_list �ڵ�ָ����������
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
            // 1. ��� col_list
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

            // 2. ���row_list
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

            // ��last tp Ҳ�ҵ�����ֵ
            //
            i = table_p->rowTotal;
            while ( i > 0 ) { // �����һ�п�ʼ��, ֱ���ҵ�һ����Ч��text���߿ձ�ͷ���NULL
                row_p = table_p->row_list[i-1];     // ���һ��
                tp = getLastTpofRow( row_p  );
                if ( tp ) {
                    // �ҵ������һ����Ч��tp
                    printf( "table(%d)�����һ���ı���:%d, %s\n", table_p->id, tp->id, tp->buf );
                    table_p->l_tp = tp;
                    break;
                }

                i --;
            }
            

            table_p = table_p->next;
        }
    }

    // orderColMap()
    //     �� colMap ������col ��w(���)��С��������,  �������� des ������
    // �ú������� fixColMaxlen() ����, ����ǰ���Ҫȷ�Ͽռ��㹻
    // ���򷽷�:
    //     ϣ������, ��Ϊԭ��������Ҫ���ֲ���, ������������µ�������. ֻ��Ҫ
    //  ����һ������Ϳ���������ɡ�
    //     ������ coltotal/2 ��ʼ�� ���μ�1.
    void orderColMap( COL ** des, COL * src, int colTotal )
    {
        COL     *   col_p;
        int         i;          // ������¼�ź����col����
        
        if ( !des || !src )
            return;

        col_p = src;
        
        // 1. �Ƚ�����ڵ㸴�Ƶ� des ������
        i = 0;
        while ( col_p ) {
            des[i] = col_p;    
            col_p = col_p->next;
            i ++;
        }

        //2. ϣ�����򷨶� des �����������
        int     step = colTotal /2;     // ��һ���������ܳ���/2
        while ( step > 0 ) {
            i = 0;
            while ( i < colTotal-step ) {    // ���ݲ���(����) ���бȽϽ���
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



            
    // ���� Col �е�maxlen, ��Ҫ�Ǹ�����col ������ �� col �Ŀ��
    // fixColMaxlen() ������Ϊ����߿ɶ��Զ���װ�Ĵ���
    //     ���� preProcColRow() ����
    //#      ����cell����󳤶�.
    //# ˵��:
    //#      Ŀǰ�ĵ�Ԫ�����󳤶���ͬ�е���ַ�������(������2��).
    //#      ��ʱ��������п����ж������. ��ô��������ڵ���󳤶�֮���п��ܻᳬ����������ַ�������
    //#      ����˳�������δ�����
    //#
    //#      ����, �����ʵ�ַ�ʽ�����кܶ�ȱ��. ���ڼ�Ҫ����, ѭ������������, ��ȡ���ĳ����еĳ���(Ҳ����getRowSplitPos()�����һ��Ԫ��ֵ���).
    //#      Ȼ�����е��е����ֵ��������ֵ����. �����һ��Ԫ�ض�����Ϊ��ֵ����.
    //# 2017.01.01:
    //#      �������ӱ��Ķ�����ʱ����. �ܸ���, ���Ǽ򵥵��������.
    //#      ��width��̵�col��ʼ����, Ȼ����������ѵ, �쿴�Ƿ���subcol��
    //#      �����, �ͼ���subcol ����֮����ָ�������, Ȼ��������󳤶�.
    //# 2019.03.09
    //#      ����Ĵ�����Ҫ����������еĴ����ָ���ռ�ÿ��ҲҪ����
    //#   ʵ��˼·:
    //       �Ƚ�colMap ����, ��խ����������ÿ����, ѭ������������, �жϸ��������е�cell, ����cell �����еļ�¼��
    //       col ��son ��, ÿһ�е�����son cell(��ʵҲ����son col), ��������son col �ĳ���֮��, ������ڵ�ǰcol ��maxlen
    //       ����µ�ǰcol ��maxlenΪ����son col �ĳ���֮��
    //  2019.03.12
    //       �и�����, ����ͬһҳ�еĲ�ͬ���, ��Ӧ�ý����е������ж�.
    //       �����ж�, �����һ��cell��������һ��cell��y + h, ��ô������һ�����е�cell��
    //       �޸�ʵ�ַ�ʽ, ������cellMap�� Ҳ���ø�colMap������, ֱ��˫��ѭ������
    // 2019.03.14
    //       ��Ҫ����������colMap, һ��col ֻ������ֻ��һ������խ���Ǹ�col parent, ������ܻ�ƥ�䵽���parent col, ������maxlen �������
    // 2019.03.18
    //       Ŀǰ�Ĵ���������, ����ͳ���������е�maxlen �ܺ�. Ӧ����ͬһ�е����е�maxlen ֮��
    //
    void fixColMaxlen( CELL * cellMap_p, SHEET * sheet_p )
    {
        int             maxlen;
        COL         *   col_p, * col_p1;
        ROW         *   row_p;
        CELL        *   cp;

        if ( !sheet_p || !cellMap_p ) 
            return;
        
        // 1. ѭ������Ѱ��ĸ��
        col_p = sheet_p->colMap_p;
        while( col_p ) {

            // ���н��б���, ȷ������maxlen ��ʱ����ͬ�е�����maxlen֮��
            row_p = sheet_p->rowMap_p;
            while( row_p ) {
                maxlen = 0;
                
                for ( int i = 0; i < row_p->cellTotal; i ++ ) {
                    cp = row_p->c_list[i];
                    
                    col_p1 = getColByID( sheet_p->colMap_p, cp->col );
                    
                    if ( col_p == col_p1->parent ) 
                        maxlen += col_p1->maxlen; 
                }

                // ����maxlen
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
        
        // 1. ѭ������Ѱ��ĸ��
        row_p = sheet_p->rowMap_p;
        while( row_p ) {

            // ���н��б���, ȷ������maxlen ��ʱ����ͬ�е�����maxlen֮��
            col_p = sheet_p->rowMap_p;
            while( col_p ) {
                maxlines = 0;
                
                for ( int i = 0; i < col_p->cellTotal; i ++ ) {
                    cp = col_p->c_list[i];
                    
                    row_p1 = getRowByID( sheet_p->rowMap_p, cp->row );
                    
                    if ( row_p == row_p1->parent ) 
                        maxlines += row_p1->maxlines; 
                }

                // ����maxlines
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
        COL         **  o_colMap_p;           // ��� �ź����rowMap �ڵ�ָ��, �� maxline ��С�������� 
        COL         *   col_p1, * col_p2;

        if ( !sheet_p || !cellMap_p ) 
            return;

        colTotal = sheet_p->colTotal;
        
        o_colMap_p = (COL **)calloc( sizeof(COL *) * colTotal, 1 );

        orderColMap( o_colMap_p, sheet_p->colMap_p, colTotal );

        // 1. ѭ������Ѱ��ĸ��
        for ( int i = 0; i < colTotal; i ++ ) {
            col_p1 = o_colMap_p[i];
            x1 = col_p1->x, w1 = col_p1->w;

            for ( int j = 0; j < colTotal; j ++ ) {
                col_p2 = o_colMap_p[j];

                if ( col_p1->id == col_p2->id )     // ����ͬһ��row
                    continue;

                x2 = col_p2->x, w2 = col_p2->w;

                if ( col_p1->table_id == col_p2->table_id &&                        // ����Ҫ��֤��һ������
                      ( x1 >= x2 || abs(x2-x1) < 1 ) && ( (x1+w1) <= (x2+w2) || abs(x2+w2-x1-w1) < 1 ) ) { // �ж�col_p2 �Ƿ���  col_p1 ĸ��  
                        // # abs ���ж�����Ϊ���к�ĸ������߻�������ص���ʱ��, �����п��ܲ�����ȫ��ͬ, �п�����΢С���
                    printf( "col id=%d ��ĸ��  �� col id=%d\n", col_p1->id, col_p2->id );

                    col_p1->parent = col_p2;
                    col_p2->sonTotal ++;
                    break;      // �ҵ� ��խ���Ǹ�ĸ�оͽ���, ����ֻ��һ��ĸ��
                }
            }
        }

        // 2. ��� ���� �ڵ�ָ��   ����, ͬʱ����ĸ�е�maxlen,
        int     k;
        for( int i = 0; i < colTotal; i ++ ) {
            col_p1 = o_colMap_p[i];             // �����ֵ��Ϊ������������Ŀɶ���
            if ( col_p1->sonTotal > 0 )  {           // ������
               col_p1->son = (COL **)calloc( sizeof(COL *) * col_p1->sonTotal, 1);
               
                //maxlen      = 0;
                k           = 0;        // ������ʶson ָ��������±�
                
                // ��������� o_colMap_p, ��Ϊ����ǰ���w�����, ����x����
                col_p2 = sheet_p->colMap_p;
                while ( col_p2 && k < col_p1->sonTotal ) {    // k < col_p1->sonTotal ��Ϊ�˷�ֹ�ڴ����
                    if  ( col_p2->parent == col_p1 ) {     // �Ҽ� ���� ��
                        col_p1->son[k] = col_p2;
                        k ++;

                        //maxlen += col_p2->maxlen;       // �ϼ����е� maxlen , 2019.03.18 ������ô�ϼ�, Ҫ�����Ƿ���ͬһ�е�col
                    }
                    col_p2 = col_p2->next;
                }

                // ���� ĸ�е�maxlen
                //if ( maxlen > col_p1->maxlen ) {
                //    col_p1->maxlen = maxlen;
                //}

                // ����������, ����y �����С����, ��ʱ������, ��Ϊ�������col �� c_list ��ʱ��, ��Ҫ��ĸ�е�c_list ҲҪ�����ȥ
                // ��ʱ����Ҫ, ӦΪ���������ݵ�ʱ���ǰ���row  ˳���������

            } 
        } 

        free( o_colMap_p );
    }

       

    /*
    #  15.62 fixRows()
    #      ����ĳЩ�б����ִ������е����, �����м��һ����Ԫ�����������������Ԫ��, �μ�0106  300182�ݳɹɷ�2016���ҵ��Ԥ��
    #  ˼·:
    #     2017.01.10:
    #         rowIndexMap ����. �����еĶ���������ĸ����û�����еĵ�Ԫ��, ������ʾ��ʱ��, ����ʾ��Ӧ����.
    #          ��-------------------��----------------��----------------------------��
    #          |         1         |    2           |   3                        |
    #          ��-------------------��----------------��----------------------------��
    #          |                   |       5        |                            |
    #          |      4            |                |          7                 |
    #          |                   ��----------------��                            |
    #          |                   |        6       |                            |
    #          ��-------------------��----------------��----------------------------��
    #         �����row Ӧ����4��, �ֱ�Ϊ: row1: [1,2,3], row2 = 4, 7, row3,row4;  row 2 = 5, row3 = 6 ��������
    #         row1 = [1,2,3], row3={cells=[4,5,7],parent="2"}, row4={cells=[4,6,7],parent="2"},  row2= {[4, 7], "sons"=[row3, row4]}
    #         Ҳ����˵, ��son��row������ʾ,
    #         ��parent��row ��ʾ��ʱ��Ҫ3������
    #          1. �Ƿ��������еĵ�һ��, Ҳ����y����һ��. �����, �����зָ���Ҫ��ʾ, Ҳ����cell�Ƿ���parent����Ҫ
    #          2. �ж�cell�Ƿ���parent��, �����, ��cell��Ӧ�����߲�����
    #          rowMap = {1: {'h': 78.0, 'y': 561.42}, 2: {'h': 31.2, 'y': 379.16}, 3: {'h': 27.0, 'y': 351.68}, 4: {'h': 54.48, 'y': 324.2}}
    #          rowIndexMap={1: [2, 7], 2: [2, 1, 2, 3], 3: [1, 5], 4: [2, 4, 6]}
    #     2017.01.11:
    #          rowIndexMap = {1:{"cells":,"lines":,"parent":,"sons":}}
          2019.03.14:
               ÿ��row ֻ��һ��parent, ������խ���Ǹ�parent. ÿ��son row ��̳� parent row ��cell list. ����cell list �ǰ���x ����˳������(����id˳��)
               �����ܱ�֤��ʾ����ʱ��˳����ȷ.
         2019.03.18
               Ŀǰ��ʵ�ַ�ʽ������, ���ܼ������е� ����
    #          
    */
    void fillRowSon( CELL * cellMap_p, SHEET * sheet_p )
    {
        //int             maxlines;
        int             rowTotal;
        float           y1, h1, y2, h2;
        ROW         **  o_rowMap_p;           // ��� �ź����rowMap �ڵ�ָ��, �� maxline ��С�������� 
        ROW         *   row_p1, * row_p2;

        if ( !sheet_p || !cellMap_p ) 
            return;

        rowTotal = sheet_p->rowTotal;
        
        o_rowMap_p = (ROW **)calloc( sizeof(ROW *) * rowTotal, 1 );

        orderRowMap( o_rowMap_p, sheet_p->rowMap_p, rowTotal );

        // 1. ѭ������Ѱ��ĸ��
        for ( int i = 0; i < rowTotal; i ++ ) {
            row_p1 = o_rowMap_p[i];
            y1 = row_p1->y, h1 = row_p1->h;

            for ( int j = 0; j < rowTotal; j ++ ) {
                row_p2 = o_rowMap_p[j];

                if ( row_p1->id == row_p2->id )     // ����ͬһ��row
                    continue;

                y2 = row_p2->y, h2 = row_p2->h;

                if ( row_p1->table_id == row_p2->table_id &&                        // ����Ҫ��֤��һ������
                        ( y1 >= y2 || abs(y2-y1) < 1 ) && ( (y1+h1) <= (y2+h2) || abs(y2+h2-y1-h2) < 1 ) ) { // �ж�row_p2 �Ƿ���  row_p1 ĸ��  
                        // # abs ���ж�����Ϊ���к�ĸ������߻�������ص���ʱ��, �����п��ܲ�����ȫ��ͬ, �п�����΢С���
                    printf( "row id=%d ��ĸ��  �� row id=%d\n", row_p1->id, row_p2->id );

                    row_p1->parent = row_p2;
                    row_p2->sonTotal ++;
                    break;      // �ҵ� ��խ���Ǹ�ĸ�оͽ���, ����ֻ��һ��ĸ��
                }
            }
        }

        // 2. ��� ���� �ڵ�ָ��   ����, ͬʱ����ĸ�е�maxlines
        //  2019.03.18   ֻ��cell��ͬһ�е����е�maxlines �ſ��Ժϼƣ� ����Ϳ��ܶ���
        int     k;
        for( int i = 0; i < rowTotal; i ++ ) {
            row_p1 = o_rowMap_p[i];             // �����ֵ��Ϊ������������Ŀɶ���
            if ( row_p1->sonTotal > 0 )  {           // ������
               row_p1->son = (ROW **)calloc( sizeof(ROW *) * row_p1->sonTotal, 1);
               
                //maxlines    = 0;
                k           = 0;        // ������ʶson ָ��������±�
                
                for ( int j = 0; j < rowTotal, k < row_p1->sonTotal; j ++ ) {       // k < row_p1->sonTotal ��Ϊ�˷�ֹ�ڴ����
                    row_p2 = o_rowMap_p[j];
                    if ( row_p2->parent == row_p1 ) {     // �Ҽ� ���� ��
                        row_p1->son[k] = row_p2;
                        k ++;

                        //maxlines += row_p2->maxlines;       // �ϼ����е� maxlines
                    }
                }

                // ���� ĸ�е�maxlines
                //if ( maxlines > row_p1->maxlines ) {
                //    row_p1->maxlines = maxlines;
                //}

                // ����������, ����x �����С����, ��ʱ������, ��Ϊ�������row �� c_list ��ʱ��, ��Ҫ��ĸ�е�c_list ҲҪ�����ȥ

            } 
        }
            
        free( o_rowMap_p );
    }

    

    // orderRowMap()
    //     �� rowMap ������row ��h(�߶�)��С��������,  �������� des ������
    // �ú������� fixRows() ����, ����ǰ���Ҫȷ�Ͽռ��㹻
    // ���򷽷�:
    //     ϣ������, ��Ϊԭ��������Ҫ���ֲ���, ������������µ�������. ֻ��Ҫ
    //  ����һ������Ϳ���������ɡ�
    //     ������ rowtotal/2 ��ʼ�� ���μ�1.
    void orderRowMap( ROW ** des, ROW * src, int rowTotal )
    {
        ROW     *   row_p;
        int         i;          // ������¼�ź����col����
        
        if ( !des || !src )
            return;

        row_p = src;
        
        // 1. �Ƚ�����ڵ㸴�Ƶ� des ������
        i = 0;
        while ( row_p ) {
            des[i] = row_p;    
            row_p = row_p->next;
            i ++;
        }

        //2. ϣ�����򷨶� des �����������
        int     step = rowTotal /2;     // ��һ���������ܳ���/2
        while ( step > 0 ) {
            i = 0;
            while ( i < rowTotal-step ) {    // ���ݲ���(����) ���бȽϽ���
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
            printf("%d�� cell is %d \n", i,  c_list[i]->id);
        }
    }
    // ��c_list ָ���������� CELL **
    // ��Ȼ��ϣ�����򷨽�������, ����cell ��id ����������
    void orderC_list( CELL **c_list, int total )
    { 
        int         i, step;
        CELL    *   cp;
        
        if ( !c_list )
            return;
    
        step = total / 2;
        while ( step > 0 ) {
            i = 0;
            while ( i < total - step ) {    // ���ݲ���(����)���бȽϽ�������
                if ( c_list[i]->id >c_list[i+step]->id ) {      // ǰ���id �Ⱥ����id ��
                    cp              = c_list[i];
                    c_list[i]       = c_list[i+step];
                    c_list[i+step]  = cp;
                }
                i ++;
            }
            step --;
        }
    }


    // �ڶ��α���cellMap_p �е�cell, Ϊ col, row �е�c_list ���cell id
    // fill_c_list() ������Ϊ����߿ɶ��Զ���װ�Ĵ���
    //     ���� preProcColRow() ����
    //
    // Ƕ�׵��� ���� col��c_list, ��֤�� ���� ��col��c_list, �� ������ col��c_list
    void fixCol_c_list( COL * col_p )
    {
        int     i;
        
        if ( ! col_p )
            return;

        i = col_p->cellTotal;

        if ( col_p->c_list[i-1] )    // ������һ��λ���Ѿ���ռ��, ˵���Ѿ��������
            return;
            
        if ( col_p->parent ) {
                
            fixCol_c_list( col_p->parent );

            // �ϲ���col��c_list �� �� c_list ��, Ȼ������
            i = 0;
            while( col_p->c_list[i] && i < col_p->cellTotal )  // ��Ϊ��col��cellTotal �Ѿ������˸�col��cellTotal, ���Ҫ�����Ѿ���¼����λ��
                i ++;

            for ( int j = 0; j < col_p->parent->cellTotal && i+j < col_p->cellTotal ; j ++ ) 
                col_p->c_list[i+j] = col_p->parent->c_list[j]; 
        }
        // �����������
        orderC_list( col_p->c_list, col_p->cellTotal );
    }

    // Ƕ�׵��� ���� row��c_list, ��֤�� ���� ��row��c_list, �� ������ row��c_list
    void fixRow_c_list( ROW * row_p )
    {
        int     i;
        
        if ( ! row_p )
            return;

        i = row_p->cellTotal;

        if ( row_p->c_list[i-1] )    // ������һ��λ���Ѿ���ռ��, ˵���Ѿ��������
            return;
            
        if ( row_p->parent ) {
                
            fixRow_c_list( row_p->parent );
            // �ϲ���row��c_list �� �� c_list ��, Ȼ������
            
            i = 0;
            while( row_p->c_list[i] && i < row_p->cellTotal )  // ��Ϊ��row��cellTotal �Ѿ������˸�row��cellTotal, ���Ҫ�����Ѿ���¼����λ��
                i ++;

            for ( int j = 0; j < row_p->parent->cellTotal && i+j < row_p->cellTotal ; j ++ ) 
                row_p->c_list[i+j] = row_p->parent->c_list[j]; 
        }
        // �����������, ���뱣֤��ȷ, ���������ȡ����ı���ʱ������
        orderC_list( row_p->c_list, row_p->cellTotal );
    }

    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p )
    {
        CELL        *   cp;
        COL         *   col_p;
        ROW         *   row_p;
        int             i;          // ������¼ c_list �±�


        printf("---------------fill_c_list() begin -----------------------------------------------------\n");
        cp = cellMap_p;

        printColMap( sheet_p->colMap_p );
        while ( cp ) {
            if ( cp->h < 1 || cp->w < 1 ) {     // ҳüҳ�ŵȿ�����cell������
                cp = cp->next;
                continue;
            }

            printf( "-------------------cellID=%d(x=%.2f, w=%.2f)Ҫ�ҵ���Ϊ:%d$$$$$$$$\n", cp->id, cp->x, cp->w, cp->col );
            col_p = getColByID( sheet_p->colMap_p, cp->col );
            
            if ( !col_p )
                return;
                       
            i = 0;
            while ( col_p->c_list[i] != 0 &&  i < col_p->cellTotal )      // Ѱ��c_list��һ�����õ�λ��, �������cell ID
                i ++;
            
            col_p->c_list[i] = cp;

            row_p = getRowByID( sheet_p->rowMap_p, cp->row );
            
            if ( !row_p )
                return;

            i = 0;
            while ( row_p->c_list[i] != 0 )      // Ѱ��c_list��һ�����õ�λ��, �������cell ID
                i ++;
            
            row_p->c_list[i] = cp;

            cp = cp->next;
        }

        // �����������, ����c_list  ���䵽���ӵ� c_list �У� ����������
        col_p = sheet_p->colMap_p;

        while( col_p ) {
            i = col_p->cellTotal;
            
            if ( col_p->c_list[i-1] ) {   // ������һ��cellҲ�Ѿ���¼��, ˵���Ѿ�������� 
                col_p = col_p->next;
                continue;
            }
            
            fixCol_c_list( col_p );
            
            col_p = col_p->next;
        }

        row_p = sheet_p->rowMap_p;

        while( row_p ) {
            i = row_p->cellTotal;
            
            if ( row_p->c_list[i-1] ) {   // ������һ��cellҲ�Ѿ���¼��, ˵���Ѿ�������� 
                row_p = row_p->next;
                continue;
            }
            
            fixRow_c_list( row_p );
            
            row_p = row_p->next;
        }
    }
    


    // ΪcolMap��rowMap�е�col, row �����Ӧc_list ��Ҫ�Ŀռ�
    //  spaceForColRowMap()  ������Ϊ����߿ɶ��Զ���װ�Ĳ��ִ���
    //      ���� preProcColRow() ����
    //  2019.03.14
    //      ����Ҫ�Ѹ�col��c_list ҲҪ�ϲ�����, ����, ����ռ�ҪҪ�������Ӹ�col��cellTotal��С
    //
    // Ƕ�׺���, ��֤�ȼ��㸸col ��c_list�ռ�, ������� col ��c_list �ռ�(Ҫ������c_list �ռ�)
    void spaceForCol( COL * col_p )
    {
        int         cellTotal;

        cellTotal = col_p->cellTotal;

        if ( col_p->parent ) {
            spaceForCol( col_p->parent );
            
            cellTotal += col_p->parent->cellTotal;        // ���˸��е�c_list �ռ�

            col_p->cellTotal = cellTotal;
        }
        col_p->c_list = (CELL **)calloc( sizeof(CELL*) * cellTotal, 1 ); 
    }

    // Ƕ�׺���, ��֤�ȼ��㸸row ��c_list�ռ�, ����� ��row  ��c_list �ռ�(Ҫ������c_list �ռ�)
    void spaceForRow( ROW * row_p )
    {
        int         cellTotal;

        cellTotal = row_p->cellTotal;

        if ( row_p->parent ) {
            spaceForRow( row_p->parent );
            
            cellTotal += row_p->parent->cellTotal;        // ���˸��е�c_list �ռ�

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
            if ( !col_p->c_list )       // ���c_list û������ռ�, ������. ��ΪspaceForCol() ��Ƕ�׵���, �Ὣ��col��c_list �ռ�������
                spaceForCol( col_p );
            col_p = col_p->next;
        }

        row_p = sheet_p->rowMap_p;
        
        while( row_p ) {
            if ( !row_p->c_list )       // ���c_list û������ռ�, ������. ��ΪspaceForRow() ��Ƕ�׵���, �Ὣ��row��c_list �ռ�������
                spaceForRow( row_p );
            row_p = row_p->next;
        }

        table_p = sheet_p->tableMap_p;

        while ( table_p ) {
            if ( !table_p->row_list )  // û���ڱ�������row , col �ڵ�ָ������ռ�
                spaceForTable( table_p );

            table_p = table_p->next;          
        }

    }
    // newCol()
    //     ע��, ���û��������cellID ��¼��colMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�col
    // �� ��  buildColRowMap() ����, ����һ��COL �ڵ�
    COL * newCol( SHEET *sheet_p, CELL * cp )
    {
        COL     * col_p, * lcp;
        TABLE   * table_p;

        if ( !sheet_p || !cp ) 
            return NULL;
        
        if ( !sheet_p->colMap_p ) {                             // colMap_p �ǿյ�, ���ǵ�һ��col
            col_p = ( COL * )calloc( sizeof( COL ), 1 );

            col_p->id       = 1;                                // ��һ�� id Ϊ 1
            col_p->x        = cp->x,    col_p->w    = cp->w;
            col_p->table_id = cp->table_id;
            col_p->prev     = NULL, col_p->next = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            col_p->parent   = NULL, col_p->son  = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            
            col_p->maxlen   = cp->maxlen;
            col_p->cellTotal = 1;
                        
            sheet_p->colMap_p   = col_p;
            sheet_p->lcp        = col_p;
            sheet_p->colTotal   = 1;

            cp->col = 1;

            // �������Ҫ������� table�ڵ� colTotal ++, Ȼ�����Ҫ����ռ䲢���col_list..., ��ʱû�д���
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->colTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            return col_p;
        } else {
            col_p = ( COL * )calloc( sizeof( COL ), 1 );
            
            lcp = sheet_p->lcp;                                 // ���һ��col pointer

            col_p->id       = lcp->id + 1;                            // ��id Ϊ ǰһ��col��id + 1
            col_p->x        = cp->x,    col_p->w = cp->w;
            col_p->table_id = cp->table_id;
            col_p->prev     = lcp, col_p->next = NULL;            // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            col_p->maxlen = cp->maxlen;
            col_p->cellTotal = 1;                               // �����½���col����,  ���ǵ�һ��cell
            
            //col_p->parent = NULL, col_p->son = NULL;    // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            lcp->next = col_p;
            
            sheet_p->lcp = col_p;                               // ���� last col pointer
            sheet_p->colTotal ++;               // ���� shet �е�colTotal ����, ���������û�м�¼�����cellID, ������Ҫ���ݸ���ֵ�����ڴ���ٴ���

            cp->col = col_p->id;

            // �������Ҫ������� table�ڵ� colTotal ++, Ȼ�����Ҫ����ռ䲢���col_list..., ��ʱû�д���
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->colTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            
            return col_p;
        } 
        
        
    }

    // ������  ӳ�����Ƿ��и�����������
    // �� ��  buildColRowMap() ����, ���Ҹ���������COL 
    COL * inColMap( COL * colMap_p, CELL * cp )
    {
        COL     *   col_p;
        float       x, w, cx, cw;     // col��x������w���

        if ( !colMap_p )        // ��� colMap_p �ǿ�, ��϶�������
            return NULL;

        col_p = colMap_p;

        while ( col_p ) {
            x = cp->x,      w = cp->w;
            cx = col_p->x,  cw = col_p->w;
            
            if ( x == cx && w == cw && col_p->table_id == cp->table_id  ) {    // ˵�������Ѿ���, ����x,w һ��, ����Ҫͬһ�������
                return col_p;
            }
            col_p = col_p->next;
        }

        // ��������궼û���ҵ�, ��˵����cell���ڵ�col û�м�¼
        return NULL;
    }


    // newRow()
    //     ע��, ���û��������cellID ��¼��rowMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�row
    // �� ��  buildColRowMap() ����, ����һ��ROW �ڵ�
    //
    ROW * newRow( SHEET *sheet_p, CELL *cp )
    {
        ROW     *   row_p, * lrp;
        TABLE   *   table_p;

        if ( !sheet_p ) 
            return NULL;

        printf("1111111111111111111111111\n");

        if ( !sheet_p->rowMap_p ) { // rowMap_p �ǿյ�, ���ǵ�һ��row
            printf("���ǵ�һ���ڵ㣡y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );

            row_p->id       = 1;                            // ��һ�� id Ϊ 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->table_id = cp->table_id;
            row_p->prev     = NULL, row_p->next = NULL;     // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            row_p->parent   = NULL, row_p->son  = NULL;     // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            
            row_p->maxlines  = cp->txtTotal;                // �����½���row, ����������ǵ�һ��cell���������(txtTotals �����Ѿ���ͬ�еĺϲ���)
            row_p->cellTotal = 1;

            sheet_p->rowMap_p   = row_p;
            sheet_p->lrp        = row_p;
            sheet_p->rowTotal   = 1;

            cp->row = 1;
            
            // �������Ҫ������� table�ڵ� rowTotal ++, Ȼ�����Ҫ����ռ䲢���col_list..., ��ʱû�д���
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->rowTotal ++;

            printf("newCol()  cp->col=%d, cp->id=%d\n", cp->col, cp->id);
            
            return row_p;
        } else {
            printf("����β����ӵĽڵ㣡y=%.2f, h=%.2f\n", cp->y, cp->h );
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );
            
            lrp = sheet_p->lrp;                             // ���һ��row pointer

            row_p->id       = lrp->id + 1;                      // ��id Ϊ ǰһ��row��id + 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->table_id = cp->table_id;
            row_p->prev     = lrp,  row_p->next = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            row_p->parent = NULL, row_p->son = NULL;      // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            row_p->maxlines  = cp->txtTotal;               // �����½���row, ����������ǵ�һ��cell���������(txtTotals �����Ѿ���ͬ�еĺϲ���)
            row_p->cellTotal = 1;

            lrp->next = row_p;
            
            sheet_p->lrp = row_p;               // ���� last row pointer
            sheet_p->rowTotal ++;               // ���� sheet �е�rowTotal ����, ���������û�м�¼�����row �ڵ�ָ��, ������Ҫ���ݸ���ֵ�����ڴ���ٴ���

            cp->row = row_p->id;
            
            // �������Ҫ������� table�ڵ� rowTotal ++, Ȼ�����Ҫ����ռ䲢���col_list..., ��ʱû�д���
            table_p = getTableByID( sheet_p->tableMap_p, cp->table_id );
            table_p->rowTotal ++;

            return row_p;
        } 
    }

    // �� ��  buildColRowMap() ����, ���Ҹ���������ROW 
    ROW * inRowMap( ROW * rowMap_p, CELL * cp )
    {
        ROW     *   row_p;
        float       y, h, cy, ch;     // row��y������h���

        if ( !rowMap_p )        // ��� rowMap_p �ǿ�, ��϶�������
            return NULL;

        row_p = rowMap_p;

        while ( row_p ) {
            y   = cp->y,    h   = cp->h;
            cy  = row_p->y, ch  = row_p->h;
            
            if ( y == cy && h == ch && row_p->table_id == cp->table_id ) {    // ˵�������Ѿ�����
                return row_p;
            }
            row_p = row_p->next;
        }

        // ��������궼û���ҵ�, ��˵����cell���ڵ�col û�м�¼
        return NULL;
    }

    // newTable()
    //     ע��, ���û��������cellID ��¼��tableMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�table
    // �� ��  buildColRowMap() ����, ����һ��TABLE �ڵ�
    //
    TABLE * newTable( SHEET *sheet_p, CELL *cp )
    {
        TABLE     * table_p, * ltp;

        if ( !sheet_p ) 
            return NULL;

        if ( !sheet_p->tableMap_p ) { // tableMap_p �ǿյ�, ���ǵ�һ��table
            table_p = ( TABLE * )calloc( sizeof( TABLE ), 1 );

            table_p->id         = 1;                            // ��һ�� id Ϊ 1
            table_p->prev       = NULL, table_p->next = NULL;     // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            
            table_p->cellTotal  = 1;

            sheet_p->tableMap_p = table_p;
            sheet_p->ltp        = table_p;
            sheet_p->tableTotal = 1;

            cp->table_id        = 1;
            
            printf("���ǵ�һ��table�ڵ㣡table_id=%d\n", table_p->id );
            return table_p;
        } else {
            table_p = ( TABLE * )calloc( sizeof( TABLE ), 1 );
            
            ltp = sheet_p->ltp;                             // ���һ��TABLE pointer

            table_p->id   = ltp->id + 1;                      // ��id Ϊ ǰһ��table��id + 1
            table_p->prev = ltp,  table_p->next = NULL;    // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            table_p->cellTotal ++;

            ltp->next = table_p;
            
            sheet_p->ltp = table_p;               // ���� last table pointer
            sheet_p->tableTotal ++;               // ���� shet �е�tableTotal ����

            cp->table_id = table_p->id;
            
            printf("����β����ӵ�table�ڵ㣡table id = %d\n", table_p->id );
            printTableMap( sheet_p->tableMap_p );
            return table_p;
        } 
    }

    // �� ��  buildColRowMap() ����, ���Ҹ���������table
    // �жϵ�ǰcell �Ƿ����Ѿ����ڵ�table��, ֻ��Ҫ��ǰһ��cell���бȽ�
    //    
    TABLE * inTableMap( TABLE * tableMap_p, CELL * cp )
    {
        TABLE       *   table_p;
        float           px, py, pw, ph, x, y, w, h;     // py, ph ��ǰһ��cell��, cell��y������h���

        if ( !tableMap_p )          // ��� tableMap_p �ǿ�, ��϶�������
            return NULL;

        if ( !cp->prev )            // ���cp û��prev, Ҳ����cp �ǵ�һ��cell, ��϶�������
            return NULL;

        x = cp->x,          y   = cp->y,        w = cp->w,          h = cp->h, 
        x = cp->prev->x,    py  = cp->prev->y,  pw= cp->prev->w,    ph = cp->prev->h;
        
        if (  y > py+ ph || x > px+pw   )          // �������һ��cell ����ͬһ����, 2��������cell ������, ����Ϊ����ͬһ����
            return NULL;

        // �����˵������һ��cell ��ͬһ����, �ҵ����table�ڵ㲢����
        table_p = tableMap_p;

        while ( table_p ) {
            if ( table_p->id == cp->prev->table_id  ) {    // �ҵ�table�ڵ�
                return table_p;
            }
            table_p = table_p->next;
        }

        // ��������궼û���ҵ�, ��˵����cell���ڵ�table û�м�¼
        return NULL;
    }


    // ����һ��cellMap, ����colMap, rowMap, tableMap ����û��c_list, ��Ϊû������֮ǰ��֪���ж���cell��col��row��, 
    // �޷�Ϊ c_list ����ռ�
    //   buildColRowMap()  ֻ�� preProcColRow() ����
    // ������4���Ӻ���
    //   inColMap(), newCol(),  inRowMap(), newRow() 
    void buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p )
    {
        CELL        *   cp;
        COL         *   col_p;
        ROW         *   row_p;
        TABLE       *   table_p;
        
        cp = cellMap_p;
        while( cp ) {
            while( cp->h < 1 || cp->w < 1 )            // �߶Ȼ���С��1 �Ķ�������, һ���ǿ����õ�
                cp = cp->next;

            if ( !cp )
                break;
            
            // 2019.03.12  ��tableMap �Ĺ���Ҳ��ǰ�����, ��Ϊ�п���һҳ�ж����, ������Ȱ�tableMap������, ������ж������п����ǿ����
            // �����tableMap Ҳ�����Ǹ������, ���ü�¼����ߵ���Ϣ, ��Ϊ��Ϣ�� cellMap, colMap, rowMap ���㹻������
            table_p = inTableMap( sheet_p->tableMap_p, cp );
            if ( !table_p ) {                       // cell û��������tableMap��, �½�һ��TABLE�ڵ�
                table_p = newTable( sheet_p, cp );
            }  else {                               // ��cell ���ڵı���Ѿ�����, Ҳ��������һ��cell��tableID һ��.
                cp->table_id = table_p->id;
                table_p->cellTotal ++;
            }
            
            col_p = inColMap( sheet_p->colMap_p, cp );
            
            if ( !col_p ) {                         // �����cell ������ ����colMap ��, �����colMap
                col_p = newCol( sheet_p, cp );              // ע��, ���û��������cellID ��¼��colMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�col, ͬʱҲ�����˶�Ӧtable�е������Ϣ
            } else {                                // cell ���ڵ����Ѿ���colmap���¼��
                                                    // ͬʱ��cell �м�¼col ��Ϣ
                col_p->cellTotal ++;                // �������е�cell ����
                //col_p->table_id = cp->table_id;   // ���col �Ѿ�����, ��ô���Ѿ��Ǽǹ�table��Ϣ��, �������¸���
                cp->col = col_p->id;
                
                                                    // ���� col_p ��maxlen
                if ( col_p->maxlen < cp->maxlen ) 
                    col_p->maxlen = cp->maxlen;

            }

            printf("--------------------------cp->col=%d, cp->id=%d\n", cp->col, cp->id);
                                                    // 2. ���� ��row
            row_p = inRowMap( sheet_p->rowMap_p, cp );
            
            if ( !row_p ) {                         // �����cell ������ û����rowMap�м�¼, �����Ӹ�����rowMap�еļ�¼
                row_p = newRow( sheet_p, cp );
            } else {                                // ��cell���ڵ����Ѿ���rowMap����, row id ��¼�� row_id������
                                                    // ��cell �м�¼  row ��Ϣ
                row_p->cellTotal ++;
                //row_p->table_id = cp->table_id;   // ���ø���
                cp->row = row_p->id;
                
                if ( row_p->maxlines < cp->txtTotal )
                    row_p->maxlines = cp->txtTotal;
            }
            
            // ����������һ��cell
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
        // ������û���ҵ�
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
        // ������û���ҵ�
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
        // ������û���ҵ�
        return NULL;
    }
    

    // # 5. ����ı�. �Ѿ������ͬ�ж��ı���ƴ��, ����cell�е��ı����ǵ���һ���ı�
    char * buildPageTxt( char * retbuf, int dlen, SHEET * sheet_p, CELL * cellMap_p, TEXT * textMap_p )
    {
        char        tmpbuf[512];            // 
        TEXT    *   tp, * ltp;              // ltp ������ű������һ����Ч�ı�ָ��, �п����ǿ�
        int         table_id;
        int         len;
        CELL    *   cp;

        if ( !sheet_p || !textMap_p )       // cellMap_p �п���Ϊ��, û�б������
            return NULL;

        tp = textMap_p;

        while ( tp ) {
            memset( tmpbuf, 0, 512 );
            memcpy( tmpbuf, tp->buf, tp->len );
            if ( tp->cellID == 0 )   {       // ���ڱ���е��ı� 
                if ( strlen( rtrim(tmpbuf) ) == 0 )    // ����, Ҫ�����¿��е����, �п���ȫ�ǿո�
                    sprintf( retbuf, "%s\n\n", retbuf );
                else
                    sprintf( retbuf, "%s%s\n", retbuf, tp->buf );
            } else {                        // ����е��ı�
                cp = (CELL *)findCell( cellMap_p, tp->cellID );

                if ( !cp )
                    return NULL;

                len = strlen( retbuf );
                ltp = buildTableTxt( retbuf + len, dlen - len, sheet_p, cellMap_p, cp->table_id );  

                if ( ltp )  // ��� �������һ���ı�ָ����Ч, ��ֵ��tp,����ѭ���ᴦ��tp->next. �����˵���ñ��Ǹ��ձ�(�������)
                    tp = ltp;
            }
            
            tp = tp->next;
        }
        
        return retbuf;
    }

    TEXT * getLastTp( SHEET *sheet_p, CELL *cellMap_p, int table_id )
    {
        TEXT        * tp, * ptp;   // ptp ��������һ�е����һ����Ч�ı�ָ��
        ROW         * row_p;

        if ( !sheet_p || !sheet_p->tableMap_p || !cellMap_p || table_id < 1 )
            return NULL;

        return NULL;


    }

    TEXT * buildTableTxt( char * desbuf, int dlen, SHEET *sheet_p, CELL *cellMap_p, int table_id )
    {
        ROW     *   row_p;
        bool        flag = false;       // �Ƿ񽨹���ͷ�ı�ʶ
        TEXT    *   tp = NULL;                 // �����������һ��cell�����һ��TEXTָ��, ���ط����������
        TABLE   *   table_p;
        int         text_id;
        int         rowTotal;
        int         len;                // ������¼�Ѿ������desbuf ����

        if ( !desbuf || !sheet_p || !cellMap_p  ) {
            return NULL;
        }

        table_p = getTableByID( sheet_p->tableMap_p, table_id );

        if ( !table_p )
            return NULL;

        rowTotal = table_p->rowTotal;


        for ( int i = 0; i < rowTotal; i ++ ) {
            row_p = table_p->row_list[i];

            if ( row_p->sonTotal > 0)           // ������ĸ�У� ���д����ʱ���ͬʱ������ĸ�е�cell
                continue;

            len = strlen( desbuf );
            
            if ( !flag ) {              // # ���û�н���ͷ, ��Ҫ����ͷ
                flag = buildRowHeader( desbuf, dlen, cellMap_p, sheet_p, row_p );
                printf("table(%d)��ͷ:%s, flag=%d\n", table_id,  desbuf, flag);
            } else {
                // ���� �м�ָ���
                // #retbuf += "�����������������������੤�������੤�������੤�������੤�������੤�������� "
                buildRowSplit( desbuf+len, dlen-len, cellMap_p, sheet_p, table_p, i );
            }
            // buildRowBody( rowIndexMap, colIndexMap, item, textMap, cellMap, cellIndexMap )
        }

        /*
         * # ������������
            lastRowId = rows[-1]
            while ( len( rowIndexMap[lastRowId]['sons'] ) > 0 ):   # ������һ��������, ��ô�ʹ�����е����һ������
                lastRowId = rowIndexMap[lastRowId]['sons'][-1]
            retbuf += self.buildRowFooter( rowIndexMap, colIndexMap, lastRowId, cellMap, cellIndexMap )
         */

        printf("desbuf=\n%s\n", desbuf);

        return table_p->l_tp;
    }

    /*
    # 15.10 buildRowHeader()
    #      �����������ı���ʽ�����
    # getRowSplitPos
    # 2017.01.11:
    #      ����ʵ�ַ�ʽ, �����µ�rowIndexMap ={'cells':,'lines':,'parent':,'sons':[]}�����д���
    #      ֻ��������, ĸ�в��ô���, ��Ϊ�������е�ʱ��˳��Ͱ�ĸ�е����ݴ�����.
    #      header �Ĵ���򵥴���, ֱ�Ӱ��������е�����cell�����д���(����ĸ��)
    #      Ҳ����˵ʵ�ַ�ʽ��ʵû�иı�.
    */
    #define LEFT_TOP        "��"
    #define MID_TOP         "��"
    #define RIGHT_TOP       "��\n"
    #define MID_LEFT        "��"
    #define MID_MID         "��"
    #define MID_RIGHT       "��\n"
    #define LEFT_BOTTOM     "��"
    #define MID_BOTTOM      "��"
    #define RIGHT_BOTTOM    "��\n"
    #define ROWLINE         "��"
    #define COLLINE         "��"

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

    // ���������� �����е�cell�������ߵĳ���
    int fixLinelen( COL * col_p )
    {
        int         ret;
        int         sonTotal;
        COL     *   s_col_p;
        int         i;

        if ( !col_p || col_p->sonTotal <= 0 ) 
            return 0;

        sonTotal = col_p->sonTotal;

        ret = sonTotal - 1;             // ������ - 1 ������Ҫ���ӵ��� ������ ����

        // ����Ƕ�׼��������е�����, ������ǰ�� �� �� ������ ����
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
     * #      ��ȡָ���еķָ���λ��
     * #      ��һ��Ԫ��0 ��Ϊ�˷�������г�, retlist[i] - retlist[i-1] ����ʵ�ʵ��г�
     */
/*
    def getRowSplitPos( self, rowIndexMap, colIndexMap, cellIndexMap, rowID ):
        retlist = [0]
        try:
            cols = len( rowIndexMap[rowID]["cells"] )
            col_pos = 0
            for i in range( 0, cols ):
                cellId = rowIndexMap[rowID]['cells'][i]          # rowIndexMap[rowID]�ĵ�0��Ԫ�����������
                colId = cellIndexMap[cellId]['col']
                col_len = colIndexMap[colId][0]            # ��ǰ�е������
                if ( i == 0 ):
                    col_pos = col_len
                else:
                    col_pos += col_len  + 1           # �� 1 ��Ϊ�� �����ָ�����ռ��2λ, ��Ϊÿһ������������, ����ֻ��Ҫ��һ�μ���, ��֤��������ʵ��λ�ü���.�Ժ��Ҳֻ��Ҫ��һ��
                retlist.append( col_pos )
        except:
            print("getRowSplitPos() Exception Error!")
            traceback.print_exc()
        return retlist
*/

    //             # �����������������������Щ��������Щ��������Щ��������Щ��������Щ���������
    bool buildRowHeader( char * desbuf, int dlen, CELL * cellMap_p, SHEET * sheet_p, ROW * row_p )
    {
        int         col_pos;
        CELL    *   cp;
        COL     *   col_p;
        
        if ( !desbuf || !cellMap_p || !sheet_p || !row_p )
            return false; 

        if ( row_p->sonTotal > 0 )  // ����������ݴ�, ���ñ�����֮ǰ��Ӧ�������ü��
            return false;
    
        strcpy( desbuf, LEFT_TOP );                             // "��"
    
        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // �������е�cell
            cp =  row_p->c_list[i];       
            col_p = getColByID( sheet_p->colMap_p, cp->col );     // �ҵ� cell��Ӧ��col�ڵ�

            if ( i != 0 )                                       // ���ǵ�һ��cell �Ļ�, �����м����� "��"
                memcpy( desbuf+strlen(desbuf), MID_TOP, strlen(MID_TOP) );      // "��"
            
            col_pos = col_p->maxlen;                            // ���߳���
            // ��Ҫ���������е����, ���л�� �ָ����ĳ���, ����Ҫ����  (������ -1)��"��" 
            // 2019.03.18 ��ҪǶ������, ��Ϊ�����п��ܻ�������.  ��δʵ�֡���������
            //if ( col_p->sonTotal > 0 )
            //    col_pos += col_p->sonTotal - 1;

            col_pos += fixLinelen( col_p );

            nstr( desbuf + strlen(desbuf), dlen, col_pos, ROWLINE );       //  "��"
        }

        // ���������һ��������     RIGHT_TOP      
        memcpy( desbuf+strlen(desbuf), RIGHT_TOP, strlen(RIGHT_TOP) );      //  "��"
        
        return true;
    }

    // �жϸ������е�cell �����Ƿ��и��� col 
    bool colInRow( SHEET * sheet_p, ROW * row_p, COL * col_p ) 
    {
        CELL    * cp;
        COL     * col_p1;

        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // �������е�cell
            cp =  row_p->c_list[i];       
            col_p1 = getColByID( sheet_p->colMap_p, cp->col );     // �ҵ� cell��Ӧ��col�ڵ�
            if ( col_p1 == col_p )                  // �ҵ�����ͬ����
                return true;
        }   
        return false;
    }

    // ָ��cell �Ƿ���ָ����row ��
    bool cellInRow( CELL * cp, ROW * row_p )
    {
        if ( !cp || !row_p || row_p->sonTotal <= 0 )    // ���һ�������������ݴ�
            return false;

        for ( int i = 0; i < row_p->cellTotal; i ++ ) {
            if ( cp == row_p->c_list[i] )
                return true;
        }

        return false;

    }

/*
    # 15.14 buildRowSplit()
    #      ����������֮��ķָ���
    # 2017.01.11:
    #      ����ʵ�ַ�ʽ, rowIndexMap={'cells':,'lines':,'parent':,'sons':}
    #      �����������֮ǰ�Ѿ�ȷ��������������ˣ�
    #      ��ȡ��ǰһ��Ҫ��һ���ж��Ƿ���ͬһparent������, �������, ��Ҫ��һ���ж�������,
    #      Ƕ�״���, ��ȡ���һ������, ������в���������"ǰһ��"
    # 2019.03.17:
    //     ����table ��row ָ��ڵ�ȫ�������row_list ָ��������. ����ֱ�Ӵ�����
    //     �ı�ʵ��˼·, ����һ�е�col ���бȽ�, ��ȷ���ָ���
    //     �����д�����ͷ����, ���Ա�Ȼ���� ����ĸ�е������Ǹ��е�ǰһ��
    */
    //            "�����������������������੤�������੤�������੤�������੤�������੤�������� "
    bool buildRowSplit( char * desbuf, int dlen, CELL * cellMap_p, SHEET * sheet_p, TABLE * table_p, int rowNo )
    {
        int         col_pos;
        CELL    *   cp;
        COL     *   col_p, * p_col_p;       // p_col_p ָ��ǰ�е�ǰһ��
        ROW     *   row_p, * p_row_p;
        int         j, k;
        bool        f_p_in_parent, f_in_parent, f_n_parent;  // ǰһ��cell, ��ǰcell, ��һ��cell �Ƿ���ĸ���еı�ʶ

        if ( !desbuf || !cellMap_p || !sheet_p || !table_p || rowNo < 1 )     // < 1 ����Ϊ ֻ�а����ڶ���֮����вŻ��� �зָ���
            return false; 

        // �ҵ���һ����Ч������
        row_p   = table_p->row_list[rowNo];     // rowNo �Ǵ�0��ʼ��
        
        j = rowNo-1;
        while ( j >= 0 ) {
            p_row_p = table_p->row_list[j];      // ǰһ��
            if ( p_row_p->sonTotal == 0 )                       // �ҵ���, ����ĸ�оͿ����뵱ǰ�н��бȽ�, ��Ϊĸ�в�����
                break;
            j --;
        }
        
        if ( p_row_p->sonTotal > 0 || row_p->sonTotal > 0 )   // �ݴ�
            return false;
        

        // 1. ��ʼ�ָ�����
        //    ���Ҫ�жϵ�һ��cell �Ƿ���ĸ�е�, �ұ��в���ĸ�еĵ�һ�� ����, ��ô����  COLLINE         "��"
        //    �������  MID_LEFT         "��" 
        //    
        if ( row_p->parent ) {       // ��ĸ��
            cp =  row_p->c_list[0];       
            if ( cp == row_p->parent->c_list[0] && row_p != row_p->parent->son[0] )   // ��һ��cell��ĸ�е�, �ұ��в��ǵ�һ������
                strcpy( desbuf, COLLINE );                             //  "��"
            else
                strcpy( desbuf, MID_LEFT );                             // "��"
        } else
            strcpy( desbuf, MID_LEFT );                             // "��"

   
        // 2.������жϻ���Ҫϸ�� 
        for ( int i = 0; i < row_p->cellTotal; i ++ ) {         // �������е�cell
            cp =  row_p->c_list[i];       

            // 2.1 ��ǰcell ��ĸ���о���, ��ǰ�� �ǵ�һ������     // "��" 
            // 2.2 ��ǰcell ��ĸ���о���, ��ǰ�в��ǵ�һ������    // "��"
            // 2.3 
            col_p = getColByID( sheet_p->colMap_p, cp->col );     // �ҵ� cell��Ӧ��col�ڵ�

            if ( i != 0 ) {                                      // ���ǵ�һ��cell �Ļ�, �����м����� "��" ��  "��"
                // col_p �Ƿ���p_row_p, �����, �� "��", ������һ�� �� "��"
                if ( colInRow( sheet_p, p_row_p, col_p ) )         // ��������һ�о��У� ���ü����"��"
                     memcpy( desbuf+strlen(desbuf), MID_MID, strlen(MID_MID) );      // "��"   
                else {
                    cp = row_p->c_list[i-1];
                    p_col_p = getColByID( sheet_p->colMap_p, cp->col ); 
                    
                    if ( colInRow( sheet_p, p_row_p, p_col_p ) )  // ���ǰһ������һ����, ��Ӧ����   MID_MID      // "��"
                        memcpy( desbuf+strlen(desbuf), MID_MID, strlen(MID_MID) );      // "��"   
                    
                    else 
                        memcpy( desbuf+strlen(desbuf), MID_TOP, strlen(MID_TOP) );      // "��"
                }
            }
            
            col_pos = col_p->maxlen;                            // ���߳���
            // ��Ҫ���������е����, ���л�� �ָ����ĳ���, ����Ҫ����  (������ -1)��"��" 
            // ���������Ҫ �������� Ƕ�״���, ��Ϊ�����п��ܻ������С�������
            col_pos += fixLinelen( col_p );
            nstr( desbuf + strlen(desbuf), dlen, col_pos, ROWLINE );       //  "��"
        }

        // 5. �����ָ�����
        //    ���Ҫ�ж����һ��cell �Ƿ���ĸ�е�, �ұ��в���ĸ�еĵ�һ�� ����, ��ô����  COLLINE         "��"
        //    �������  MID_RIGHT         "��"
        if ( row_p->parent ) {       // ��ĸ��
            j = row_p->cellTotal;
            cp =  row_p->c_list[j-1];   

            k =  row_p->parent->cellTotal;  

            if ( cp == row_p->parent->c_list[k-1] && row_p != row_p->parent->son[0] )   // ���һ��cell��ĸ�е�, �ұ��в��ǵ�һ������
                strcpy( desbuf+strlen(desbuf), COLLINE );                             //  "��"
            else
                strcpy( desbuf+strlen(desbuf), MID_RIGHT );                             // "��"
        } else
            strcpy( desbuf+strlen(desbuf), MID_RIGHT );                             // "��"

        return true;
    }


