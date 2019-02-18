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
        int         obj;        
        int         total;     
        CODE        * code_p; 
        float       comprRatio;      
    } CMAP;

    typedef struct __pages__ {
        int         total;   
        int         cursor; 
        int     *   leafs_p;
        int     **  c_objlist_p;
        FONTMAP **  fontmap_p;  
        int         cmaptotal; 
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

    typedef struct __cellMap__ {
        int         x;
        // 单元格信息
    }CELLMAP;

    typedef struct __cellIndexMap__{
        // 单元格索引信息
        int         x;
    }CELLINDEXMAP;

    typedef struct __textMap__ {
        // 文本映射表
        int         x;
    } TEXTMAP;

    // 一直存放的是最新的tm数据
    typedef struct __tf__ {
        // tf 信息
        int         x;
    }TF;

    typedef struct __decode__ {
        CUR_XY          cur_xy;
        TM              tm;
        char            tf[L_FONTNAME];     // 当前使用的字体名称(对应cmap中的fontname)
        TEXTMAP         * textMap;
        CELLMAP         * cellMap_p;        // 处理表格式需要re
        CELLINDEXMAP    * cellIndexMap_p;
    }DECODE;

    int     parsePDF( char * desfile,  char * srcfile );
    PDF   * parsePDF1( char * srcfile );
    void  __declspec(dllexport)  freeALL2( PDF * pdf_p );
    uchar * getSpecPage( PDF *pdf_p, int pageno );
    

    #endif
