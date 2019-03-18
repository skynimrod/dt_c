    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :   sheet
    //
    // Description : about sheet operation.
    //
    // Interface :
    //      
    //      
    #ifndef __SHEET_H__    // define sheet.h Start {
    #define __SHEET_H__
    #include "pdf_tools.h"

    // 
    typedef struct __col_ {
        int                 id;
        int                 cellTotal;      // ��col ������cell ����,����Ϊc_list ����ռ�
        int                 sonTotal;       // ��col ����
        int                 table_id;       // ����table id
        int                 maxlen;         // col�������, �ַ�������󳤶�
        float               x;              // col�� x ����
        float               w;              // col�� w ���
        CELL            **  c_list;         // ��col��cell�б�
        struct __col_   **  son;            // ��col �ڵ�ָ������
        struct __col_   *   parent;         // ��col
        struct __col_   *   prev;           // ǰһ��col
        struct __col_   *   next;           // ��һ��col
    }COL;

    typedef struct __row_ {
        int                 id;
        int                 cellTotal;      // ��row ������cell ����,����Ϊc_list ����ռ�
        int                 sonTotal;       // ��row ����
        int                 maxlines;       // row���������
        int                 table_id;       // ����table id
        float               y;              // row�� y ����
        float               h;              // row�� h �߶�
        CELL            **  c_list;         // ��row��cell�б�
        struct __row_   **  son;            // ��row �ڵ�ָ������
        struct __row_   *   parent;         // ��row �ڵ�ָ��
        struct __row_   *   prev;           // ǰһ��row
        struct __row_   *   next;           // ��һ��row
    }ROW;

    typedef struct __table__ {
        int                     id;             // �� ���, ��1 ��ʼ
        int                     cellTotal;      //
        int                     colTotal;
        int                     rowTotal;
        TEXT                *   l_tp;           // �������һ����Ч�ı�ָ��,����ǿձ���Ϊ�� 
        CELL               **   c_list;         // ���е�cell ָ������, ����
        COL                **   col_list;       // ���е�col ָ������, ����
        ROW                **   row_list;       // ���е�row ָ������
        struct __table__    *   prev;           // ǰһ��table
        struct __table__    *   next;           // ��һ��table
    } TABLE;

    typedef struct __sheet__ {
        int             colTotal;
        int             rowTotal;
        int             tableTotal;
        COL         *   colMap_p;
        COL         *   lcp;                    // last col pointer
        ROW         *   rowMap_p;
        ROW         *   lrp;                    // last row pointer
        TABLE       *   tableMap_p;
        TABLE       *   ltp;                    // last table pointer
    }SHEET;


    #endif
