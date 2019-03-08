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
        int                 cellTotal;      // 该col 包含的cell 数量,用来为c_list 申请空间
        int                 sonTotal;       // 子col 数量
        float               x;              // col的 x 坐标
        float               w;              // col的 w 宽度
        float               maxlen;         // col的最大宽度
        int             *   c_list;         // 该col的cell列表
        int             *   son;            // 子col id 列表
        struct __col_   *   parent;         // 父col
        struct __col_   *   prev;           // 前一个col
        struct __col_   *   next;           // 后一个col
    }COL;

    typedef struct __row_ {
        int                 id;
        int                 cellTotal;      // 该row 包含的cell 数量,用来为c_list 申请空间
        int                 sonTotal;       // 子row 数量
        int                 maxlines;       // row的最大行数
        float               y;              // row的 y 坐标
        float               h;              // row的 h 高度
        int             *   c_list;         // 该row的cell列表
        int             *   son;            // 子col id 列表
        struct __row_   *   parent;         // 父row
        struct __row_   *   prev;           // 前一个row
        struct __row_   *   next;           // 后一个row
    }ROW;

    typedef struct __table__ {
        int                     id;             // 表 编号, 从1 开始
        int                     l_txtid;        // 表中的最后一个文本编号
        int                     cellTotal;      //
        int                     colTotal;
        int                     rowTotal;
        int                 *   c_list;         // 表中的cell id列表
        int                 *   col_list;       // 表中的col id列表
        int                 *   row_list;       // 表中的row id列表
        struct __table__    *   prev;           // 前一个table
        struct __table__    *   next;           // 后一个table
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
