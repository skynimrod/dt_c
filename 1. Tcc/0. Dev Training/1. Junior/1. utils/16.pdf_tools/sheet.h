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

    // 
    typedef struct __col_ {
        int                 id;
        int                 cellTotal;      // ��col ������cell ����,����Ϊc_list ����ռ�
        int                 sonTotal;       // ��col ����
        float               x;              // col�� x ����
        float               w;              // col�� w ���
        float               maxlen;         // col�������
        int             *   c_list;         // ��col��cell�б�
        int             *   son;            // ��col id �б�
        struct __col_   *   parent;         // ��col
        struct __col_   *   prev;           // ǰһ��col
        struct __col_   *   next;           // ��һ��col
    }COL;

    typedef struct __row_ {
        int                 id;
        int                 cellTotal;      // ��row ������cell ����,����Ϊc_list ����ռ�
        int                 sonTotal;       // ��row ����
        int                 maxlines;       // row���������
        float               y;              // row�� y ����
        float               h;              // row�� h �߶�
        int             *   c_list;         // ��row��cell�б�
        int             *   son;            // ��col id �б�
        struct __row_   *   parent;         // ��row
        struct __row_   *   prev;           // ǰһ��row
        struct __row_   *   next;           // ��һ��row
    }ROW;

    typedef struct __table__ {
        int                     id;             // �� ���, ��1 ��ʼ
        int                     l_txtid;        // ���е����һ���ı����
        int                     cellTotal;      //
        int                     colTotal;
        int                     rowTotal;
        int                 *   c_list;         // ���е�cell id�б�
        int                 *   col_list;       // ���е�col id�б�
        int                 *   row_list;       // ���е�row id�б�
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
