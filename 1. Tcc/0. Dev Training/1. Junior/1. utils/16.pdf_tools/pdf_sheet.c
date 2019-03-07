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

    typedef struct __colMap_ {
        int                     id;
        int                 *   c_list;         // ��col��cell�б�
        int                     cellTotal;      // ��col ������cell ����,����Ϊc_list ����ռ�
        float                   x;              // col�� x ����
        float                   w;              // col�� w ���
        float                   maxlen;         // col�������
        struct __colMap_    *   parent;         // ��col
        struct __colMap_    *   son;            // ��col
    }COLMAP;

    typedef struct __rowMap_ {
        int                     id;
        int                 *   c_list;         // ��row��cell�б�
        int                     cellTotal;      // ��row ������cell ����,����Ϊc_list ����ռ�
        float                   y;              // row�� y ����
        float                   h;              // row�� h �߶�
        int                     lines;          // row���������
        int                     parentID;       // ��row ID
        int                 *   son;            // ��row ID ����
        int                     sonTotal;       // ��row ����
    }ROWMAP;

    typedef struct __tableMap__ {
        int                     id;             // �� ���
        int                 *   c_list;         // ���е�cell id�б�
        int                 *   row_list;       // ���е�row id�б�
        int                 *   col_list;       // ���е�col id�б�
        int                     l_txtid;        // ���е����һ���ı����
    } TABLEMAP;

    char *  procText4Table( TEXT * textMap_p, CELL * cellMap_p );
    

    void preProcTxtMap( TEXT * textMap_p, CELL * cellMap_p );
    void preProcTxtMap4Cell( TEXT * textMap_p, CELL * cellMap_p );
    /*
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap );
    void buildTableMap( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    char * buildPageTxt( textMap, cellMap, cellIndexMap, tableMap, colIndexMap, rowIndexMap );
    */
    char * procText4Table( TEXT * textMap_p, CELL * cellMap_p )
    {
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
        return NULL;
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

    // # 2. Ԥ���� ����е��ı�, ��ԭ��һ�еĶ���ı�ƴ��Ϊһ���ı�, ɾ��������ı�. ͬʱ����cell�е�txtIDs_p �е��ı������Ϣ
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
            if ( tp->cellID == 0 ) {   // ����0, ��ʾ���ı�û����cell��, ����������
                tp = tp->next;
                continue;        
            }

            cp = getCellByID( cellMap_p, tp->cellID );
            // ��ʱ, ���ı���id ��û�д����cell �е�txtIDs_p ������, ֮ǰ�Ĵ���������������㹻�Ĵ�ſռ�
            // ������㴦��㽫�������ı���id��ŵ�txtIDs_p ������
            for ( i = 0; i < cp->txtTotal; i ++ ) 
                if ( cp->txtIDs_p[i] == 0 )   // ������β��
                    break;
            
            if ( i == 0 ) { // ��ʾ���ı���cell �еĵ�һ���ı�, ��¼��txtIDs_p[0] ��, �����ϲ��ı�����
                cp->txtIDs_p[i] = tp->id;
                tp = tp->next;
                continue;
            } else if ( i == cp->txtTotal ) {   // �ݴ���,��ʾԤ����ռ��Ѿ�������, ��ô��ǰ������ı��Ͳ�Ӧ���ڸ�cell��
                return NULL;
            } else  {
                text_id = cp->txtIDs_p[i-1];        //��ͬһ��cell�е��ı��ڵ��ǰһ���ı��ڵ���
                tpp = searchForward( tp, text_id );
                if ( !tpp )                     // �ݴ��� , û�ҵ�ǰһ���ı��ڵ㡣����(���������Ӧ�÷���)
                    return NULL;

                ox = tp->ox,        oy = tp->oy;
                px = tpp->ox,       py = tpp->oy;

                printf("ox=%.2f, oy=%.2f, px=%.2f, py=%.2f\n", ox, oy, px, py );
            
                if ( abs( py - oy ) < 1 ) {     //  # ����Y����С��1��Ϊͬһ��
                    // �ϲ��ı�, ���� tpp, ��Ϊtpp�Ѿ���¼����cell����
                    combineBuf( tpp, tp );
                    if ( tpp->len > cp->maxlen )    // ����cell�е�����ı�����
                        cp->maxlen = tpp->len;

                    tpn = tp->next;                 // ������һ���ı��ڵ�
                    delCurTxt( tp );                    // ɾ��tpp �ڵ�
                    
                    tp = tpn;                      // tp ���¶�λ����һ����Ҫ����Ľڵ� 
                } else {    // ����Ҫ�ϲ�
                    cp->txtIDs_p[i] = tp->id;       // ���cell�е��ı���¼
                    tp = tp->next;
                }
            }

        }


        return;
    }
    /*
    // # 3. Ԥ���� ���, ���������ϵ���ൽ������, �����Ϣ����� colMap, colIndexMap, rowMap, rowIndexMap
    colMap, colIndexMap, rowMap, rowIndexMap = {}, {}, {}, {}  # �ֵ䲻����Ϊkey, ֻ���ٽ���һ��colIndexMap����¼ÿ��col��Ӧ��cell����
    void preProcColRow( cellMap, cellIndexMap, colMap, colIndexMap, rowMap, rowIndexMap )
    {
        return;
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
*/
