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
    //      1.  parsePDF()  
    //      2.  getSpecPage()
    //   --- 下面的部分属于应用部分， 需要摘出去
    //      3.  getSpecKeyLine()
    //      4.  getNDYJYG()
    //      
    #ifndef __PDF_TOOLS_H__    // define pdf_tools.h Start {
    #define __PDF_TOOLS_H__

    #include "filemap.h"

    // 
    #define L_pos       128
    typedef struct __posmap__{
        int     cursor;                
        long    xrefpos[L_pos];         
    }POSMAP;

    typedef struct __xref__ {
        int                 retcode;
        int                 objTotal;  
        long              * objpos_p; 
    } XREF;

    #define L_ID  128       
    typedef struct __trailer__ {
        int         Root;    
        int         Size;   
        long        Info;  
        char        ID[L_ID+1];
        long        Prev;       
        long        xrefStm;   
        int         retcode;  
    } TRAILER;

    #define L_FONTNAME      64
    typedef struct __fontmap__ { 
        int         obj;        
        char        fontname[L_FONTNAME+1]; 
        int         isType0;               
    }FONTMAP;

    typedef struct __code_ {  
        unsigned int    code;    
        unsigned int    decode; 

    } CODE;

    typedef struct __cmap__ {
        char        fontname[L_FONTNAME];
        float       comprRatio;      
        int         obj;        
        int         total;     
        CODE        * code_p; 
    } CMAP;

    typedef struct __pages__ {
        int         total;   
        int         cursor; 
        int         cmaptotal; 
        int     *   leafs_p;
        int     **  c_objlist_p;
        FONTMAP **  fontmap_p;  
        CMAP    *   cmaps_p;  
        float   **  comprRatio_p;    
    }PAGES;

    typedef struct __pdf__ {
        POSMAP      * posmap_p;
        FILEMAP     * fm_p;
        XREF        * xref_p;
        TRAILER     * trailer_p;
        PAGES       * pages_p;
    } PDF;

    // 解码相关数据信息
    // 1. tm 信息表
    typedef struct __tm__ {
        // tm 信息
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }TM;

    // 2. cm 信息表
    typedef struct __cm__ {
        // tm 信息
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }CM;

    // 坐标信息
    #define L_OPR       32
    typedef struct __cur_xy__ {
        float       x;          // 相对坐标x
        float       y;          // 相对坐标y
        float       ox;         // 原始x 坐标, 经过tm与x计算获得 
        float       oy;         // 原始y 坐标, 经过tm与y计算获得 
        char        opr[L_OPR];    // 操作符, 比如"Td", "TD"等
    }CUR_XY;

    // 文本信息
    typedef struct __text__ {
        int                 id;
        float               ox;
        float               oy;
        int                 len;
        int                 cellID;     // 该文本属于哪个CELL, 如果是0表示不在cell中
        char            *   buf;
        struct __text__  *  prev;       
        struct __text__  *  next;
    } TEXT;

    // 单元格信息
    typedef struct __cell__ {
        int                 id;             // 编号从1开始 , 自动产生
        float               x;
        float               y;
        float               w;
        float               h;
        int                 maxlines;
        int                 maxlen;         // 最长的文本长度
        int                 row;            // 所属行ID
        int                 col;            // 所属列ID
        int                 table_id;       // 所属  表id
        int                 txtTotal;       // 单元格中的文本编号数量
        struct __cell__  *  prev;           // 前一个单元格
        struct __cell__  *  next;           // 后一个单元格
        TEXT             ** tps;            // 单元格中TEXT指针数组
    }CELL;

    typedef struct __decode__ {
        CUR_XY      cur_xy;
        TM          tm;
        char        tf[L_FONTNAME];     // 当前使用的字体名称(对应cmap中的fontname)
        TEXT    *   ltp;                // last text pointer
        TEXT    *   tp;                 // text map pointer, 链表, 每个节点是个TEXT 指针
        CELL    *   lcp;                // last cell Pointer
        CELL    *   cp;                 // Cell Map pointer, 链表, 每个节点是个CELL指针。 处理表格式需要re
    }DECODE;

    int     parsePDF( char * desfile,  char * srcfile );
    PDF   * parsePDF1( char * srcfile );
    void  __declspec(dllexport)  freeALL2( PDF * pdf_p );
    uchar * getSpecPage( PDF *pdf_p, int pageno );
    

    #endif
