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

    // buildColRowMap(), spaceForColRowMap(), fill_c_list()  ���� preProcColRow() ����
    void  buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p );
    void spaceForColRowMap( SHEET * sheet_p );
    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p );

    /*
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    */
    char * procText4Table( TEXT * textMap_p, CELL * cellMap_p )
    {
        SHEET       * sheet_p;          // ��, ��¼��������Ϣ

        sheet_p = (SHEET *)calloc( sizeof(SHEET), 1 );


        // # 1. ����textMap, �����ڱ���е��ı����д���, �����ͬһ�еľͽ��кϲ�, ͬʱ��cell�е�txtIDs_p ��������
        preProcTxtMap( textMap_p, cellMap_p );
                
        // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ
        preProcTxtMap4Cell( textMap_p, cellMap_p );
     
        /*       
        // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
        //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
        self.preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
        
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
        
        // 1. �ͷ� ��col  ��ؽڵ�
        if ( sheet_p->colMap_p ) {
            col_p = sheet_p->lcp;           // �Ӻ���ǰ�ͷ�col �ڵ�
            while( col_p ) {
                col_p = col_p->prev;        // �ȱ�����ǰһ���ڵ�ĵ�ַ( ������׽ڵ㣬�����ֵΪNULL)
                freeCOL( col_p->next );
            }
        }

        // 2. �ͷ� ��row  ��ؽڵ�
        if ( sheet_p->rowMap_p ) {
            row_p = sheet_p->lrp;           // �Ӻ���ǰ�ͷ�row �ڵ�
            while( row_p ) {
                row_p = row_p->prev;        // �ȱ�����ǰһ���ڵ�ĵ�ַ( ������׽ڵ㣬�����ֵΪNULL)
                freeROW( row_p->next );     // row_p->next ��ʵ���ǵ�ǰ�ڵ�, ��Ϊǰһ�����¸�ֵ��
            }
        }

        // 3. �ͷ� ��� table  ��ؽڵ�
        if ( sheet_p->tableMap_p ) {
            table_p = sheet_p->ltp;           // �Ӻ���ǰ�ͷ�table �ڵ�
            while( table_p ) {
                table_p = table_p->prev;        // �ȱ�����ǰһ���ڵ�ĵ�ַ( ������׽ڵ㣬�����ֵΪNULL)
                freeTable( table_p->next );
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

        // 2. ����Ϊ��Ԫ�� ���ı���ż�¼����ռ�(txtIDs_p)
        if ( !cellMap_p )
            return;

        cp = cellMap_p;
        while ( cp ) {
            if ( cp->txtTotal > 0 )
                cp->txtIDs_p = (int *)calloc( sizeof(int), cp->txtTotal );   // Ϊ��ż�¼cell�е��ı��������ռ�
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
            // ��ʱ, ���ı���id ��û�д����cell �е�txtIDs_p ������, ֮ǰ�Ĵ���������������㹻�Ĵ�ſռ�
            // ������㴦��㽫�������ı���id��ŵ�txtIDs_p ������
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->txtIDs_p[i] == 0 )         // ������β��
                    break;
            
            if ( i == 0 ) {                         // ��ʾ���ı���cell �еĵ�һ���ı�, ��¼��txtIDs_p[0] ��, �����ϲ��ı�����
                cp->txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {       // �ݴ���,��ʾԤ����ռ��Ѿ�������, ��ô��ǰ������ı��Ͳ�Ӧ���ڸ�cell��
                return ;
            } else  {
                text_id = cp->txtIDs_p[i-1];        //��ͬһ��cell�е��ı��ڵ��ǰһ���ı��ڵ���
                tpp = searchForward( tp, text_id );
                if ( !tpp )                         // �ݴ��� , û�ҵ�ǰһ���ı��ڵ㡣����(���������Ӧ�÷���)
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
                    cp->txtIDs_p[i] = tp->id;       // ���cell�е��ı���¼
                    tp = tp->next;
                }
            }

        }


        return;
    }

    // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
    //colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
    // �������е�cell, ������(x,w) ���� col, ���� (y,h) ���� row

    // newCol()
    //     ע��, ���û��������cellID ��¼��colMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�col
    // �� ��  buildColRowMap() ����, ����һ��COL �ڵ�
    void newCol( SHEET *sheet_p, CELL * cp )
    {
        COL     * col_p, * lcp;

        if ( !sheet_p || !cp ) 
            return;
        
        if ( !sheet_p->colMap_p ) {                             // colMap_p �ǿյ�, ���ǵ�һ��col
            col_p = ( COL * )calloc( sizeof( COL ), 1 );

            col_p->id       = 1;                                // ��һ�� id Ϊ 1
            col_p->x        = cp->x,    col_p->w    = cp->w;
            col_p->prev     = NULL, col_p->next = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            col_p->parent   = NULL, col_p->son  = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            
            col_p->maxlen   = cp->maxlen;
            col_p->cellTotal = 1;
                        
            sheet_p->colMap_p   = col_p;
            sheet_p->lcp        = col_p;
            sheet_p->colTotal   = 1;
            return;
        } else {
            col_p = ( COL * )calloc( sizeof( COL ), 1 );
            
            lcp = sheet_p->lcp;                                 // ���һ��col pointer

            col_p->id = lcp->id + 1;                            // ��id Ϊ ǰһ��col��id + 1
            col_p->x  = cp->x, col_p->w = cp->w;
            col_p->prev   = lcp, col_p->next = NULL;            // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            col_p->maxlen = cp->maxlen;
            col_p->cellTotal = 1;                               // �����½���col����,  ���ǵ�һ��cell
            
            //col_p->parent = NULL, col_p->son = NULL;    // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            lcp->next = col_p;
            
            sheet_p->lcp = col_p;                               // ���� last col pointer
            sheet_p->colTotal ++;               // ���� shet �е�colTotal ����, ���������û�м�¼�����cellID, ������Ҫ���ݸ���ֵ�����ڴ���ٴ���
            return;
        } 
        
        
    }

    // ������  ӳ�����Ƿ��и�����������
    // �� ��  buildColRowMap() ����, ���Ҹ���������COL 
    COL * inColMap( COL * colMap_p, float x, float w )
    {
        COL     *   col_p;
        float       cx, cw;     // col��x������w���

        if ( !colMap_p )        // ��� colMap_p �ǿ�, ��϶�������
            return NULL;

        col_p = colMap_p;

        while ( !col_p ) {
            cw = col_p->x,  cw = col_p->w;
            
            if ( x == cw && w == cw  ) {    // ˵�������Ѿ���
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
    void newRow( SHEET *sheet_p, CELL *cp )
    {
        ROW     * row_p, * lrp;

        if ( !sheet_p ) 
            return;

        if ( !sheet_p->rowMap_p ) { // rowMap_p �ǿյ�, ���ǵ�һ��row
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );

            row_p->id       = 1;                            // ��һ�� id Ϊ 1
            row_p->y        = cp->y,    row_p->h    = cp->h;
            row_p->prev     = NULL, row_p->next = NULL;     // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            row_p->parent   = NULL, row_p->son  = NULL;     // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���
            
            row_p->maxlines  = cp->txtTotal;                // �����½���row, ����������ǵ�һ��cell���������(txtTotals �����Ѿ���ͬ�еĺϲ���)
            row_p->cellTotal = 1;

            sheet_p->rowMap_p   = row_p;
            sheet_p->lrp        = row_p;
            sheet_p->rowTotal   = 1;
            return;
        } else {
            row_p = ( ROW * )calloc( sizeof( ROW ), 1 );
            
            lrp = sheet_p->lrp;                             // ���һ��col pointer

            row_p->id   = lrp->id + 1;                      // ��id Ϊ ǰһ��col��id + 1
            row_p->y    = cp->y,    row_p->h    = cp->h;
            row_p->prev = lrp,  row_p->next = NULL;         // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            //col_p->parent = NULL, col_p->son = NULL;      // �����ڴ�ʱ�Ѿ���ʼ��Ϊ0�ˣ� �����ֵ��Ϊ�˿ɶ���

            row_p->maxlines  = cp->txtTotal;               // �����½���row, ����������ǵ�һ��cell���������(txtTotals �����Ѿ���ͬ�еĺϲ���)
            row_p->cellTotal = 1;

            lrp->next = row_p;
            
            sheet_p->lrp = row_p;               // ���� last row pointer
            sheet_p->rowTotal ++;               // ���� shet �е�rowTotal ����, ���������û�м�¼�����cellID, ������Ҫ���ݸ���ֵ�����ڴ���ٴ���
            return;
        } 
    }

    // �� ��  buildColRowMap() ����, ���Ҹ���������ROW 
    ROW * inRowMap( ROW * rowMap_p, float y, float h )
    {
        ROW     *   row_p;
        float       cy, ch;     // row��y������h���

        if ( !rowMap_p )        // ��� rowMap_p �ǿ�, ��϶�������
            return NULL;

        row_p = rowMap_p;

        while ( !row_p ) {
            cy = row_p->y,  ch = row_p->h;
            
            if ( y == cy && h == ch  ) {    // ˵�������Ѿ���
                return row_p;
            }
            row_p = row_p->next;
        }

        // ��������궼û���ҵ�, ��˵����cell���ڵ�col û�м�¼
        return NULL;
    }


    // ʵ��˼·:
    //    ����2��cellMap_p �е�����cell, ��һ���Ǵ���cell ������col, row, �γ�����
    //    �ڶ��α��� �Ǹ��ݵ�һ�α�����õ�col, row �ڵ�cell ����, �����Ӧ���ڴ�, 
    //    Ȼ���cell id ��䵽 col, row �ڵ�c_list ������.
    // ����5���ֲ�����
    //   buildColRowMap(), buildColRowMap(), fill_c_list(), fixColMaxlen()
    void preProcColRow( CELL *cellMap_p,  SHEET * sheet_p )
    {
        if ( !sheet_p || !cellMap_p )
            return;

        // ��һ�α���cellMap, ����colMap, rowMap, ����û��c_list, ��Ϊû������֮ǰ��֪���ж���cell��col��row��, 
        // Ϊ c_list �޷�����ռ�
        buildColRowMap( cellMap_p, sheet_p );
        
        // ΪcolMap��rowMap�е�col, row �����Ӧc_list ��Ҫ�Ŀռ�
        spaceForColRowMap( sheet_p );
        
        // �ڶ��α���cellMap_p �е�cell, Ϊ col, row �е�c_list ���cell id
        fill_c_list( cellMap_p, sheet_p );

        // ���� Col �е�maxlen, ��Ҫ�Ǹ�����col ������ �� col �Ŀ��
        //fixColMaxlen( cellIndexMap, colMap, colIndexMap, rowIndexMap )
        // ���� row �е�maxlines, ��Ҫ�Ǹ��� ����row ������ �� row ������
        //fixRows( cellMap, cellIndexMap, rowMap, colIndexMap, rowIndexMap )
        return;
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
    void fixColMaxlen( CELL *cellMap_p, SHEET * sheet_p )
    {
        if ( !cellMap_p || ! sheet_p )
            return;
        
        if ( !sheet_p->colMap_p )
            return;
        
        int     colTotal = sheet_p->colTotal;
        COL     o_colMap[ sheet_p->colTotal ];
        
        // �Ȱ�col ���д�խ���� ����, ��Ϊֻ�п��col������խ��col�ĸ�col 
        //orderColMapByWidth( &o_colMap, sheet_p );      // ��խ����� colMap_p ����, �������� o_colMap
        //x1, x2 �ֱ���� col ��2���ߵ�λ��(x1=x, x2 = x+ w)
        
        
    }

    // �ڶ��α���cellMap_p �е�cell, Ϊ col, row �е�c_list ���cell id
    // fill_c_list() ������Ϊ����߿ɶ��Զ���װ�Ĵ���
    //     ���� preProcColRow() ����
    void fill_c_list( CELL * cellMap_p, SHEET * sheet_p )
    {
        CELL        *   cp;
        COL         *   col_p;
        ROW         *   row_p;
        int             i;          // ������¼ c_list �±�

        cp = cellMap_p;
        while ( cp ) {
            col_p = getColOfID( sheet_p->colMap_p, cp->col );
            i = 0;
            while ( col_p->c_list[i] != 0 )      // Ѱ��c_list��һ�����õ�λ��, �������cell ID
                i ++;
            
            col_p->c_list[i] = cp->id;

            row_p = getRowOfID( sheet_p->rowMap_p, cp->row );
            i = 0;
            while ( row_p->c_list[i] != 0 )      // Ѱ��c_list��һ�����õ�λ��, �������cell ID
                i ++;
            
            row_p->c_list[i] = cp->id;
        }
    }

    // ΪcolMap��rowMap�е�col, row �����Ӧc_list ��Ҫ�Ŀռ�
    //  spaceForColRowMap()  ������Ϊ����߿ɶ��Զ���װ�Ĳ��ִ���
    //      ���� preProcColRow() ����
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

    // ����һ��cellMap, ����colMap, rowMap, ����û��c_list, ��Ϊû������֮ǰ��֪���ж���cell��col��row��, 
    // �޷�Ϊ c_list ����ռ�
    //   buildColRowMap()  ֻ�� preProcColRow() ����
    // ������4���Ӻ���
    //   inColMap(), newCol(),  inRowMap(), newRow() 
    void buildColRowMap( CELL * cellMap_p,  SHEET * sheet_p )
    {
        CELL        *   cp;
        float           x, y, w, h;
        COL         *   col_p;
        ROW         *   row_p;
        
        cp = cellMap_p;
        while( cp ) {
            if( cp->h < 1 || cp->w < 1 )            // �߶Ȼ���С��1 �Ķ�������, һ���ǿ����õ�
                cp = cp->next;

            x = cp->x, y = cp->y, w = cp->w, h = cp->h;
                                                    // 1. �ȴ�����row
            col_p = inColMap( sheet_p->colMap_p, x, w );
            
            if ( !col_p ) {                         // �����cell ������ ����colMap ��, �����colMap
                newCol( sheet_p, cp );              // ע��, ���û��������cellID ��¼��colMap��, ��Ϊֻ�б�����һ��cellMap��֪���ж���cell�ڸ�col
                cp->col = sheet_p->lcp->id;         // �½���col id����  lcp ��id(  ֻ��һ��col �ڵ�)
            } else {                                // cell ���ڵ����Ѿ���colmap���¼��
                                                    // ͬʱ��cell �м�¼col ��Ϣ
                cp->col = col_p->id;      

                col_p->cellTotal ++;                // �������е�cell ����
                
                                                    // ���� col_p ��maxlen
                if ( col_p->maxlen < cp->maxlen ) 
                    col_p->maxlen = cp->maxlen;
            }
            
                                                    // 2. ���� ��row
            row_p = inRowMap( sheet_p->rowMap_p, y, h );
            
            if ( !row_p ) {                         // �����cell ������ û����rowMap�м�¼, �����Ӹ�����rowMap�еļ�¼
                newRow( sheet_p, cp );
                cp->row = sheet_p->lrp->id;         // ֻ��һ��row �ڵ�
            } else {                                // ��cell���ڵ����Ѿ���rowMap����, row id ��¼�� row_id������
                                                    // ��cell �м�¼  row ��Ϣ
                cp->row =row_p->id;

                row_p->cellTotal ++;
                
                if ( row_p->maxlines < cp->txtTotal )
                    row_p->maxlines = cp->txtTotal;
            }

            // ����������һ��cell
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
        // ������û���ҵ�
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
        // ������û���ҵ�
        return NULL;
    }
    
    // # 4. �����ı���cell�Ĺ�ϵ, ��ֳ�table ����, ����tableMap={1:[cellid_list]}�� ͬʱ��cellMap �����tableid ����
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }

    // # 5. ����ı�. �Ѿ������ͬ�ж��ı���ƴ��, ����cell�е��ı����ǵ���һ���ı�
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap )
    {
        return;
    }
    /*
*/
