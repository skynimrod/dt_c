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
        int         code;    
        int         decode; 

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

    int     parsePDF( char * desfile,  char * srcfile );
    PDF   * parsePDF1( char * srcfile );
    void  __declspec(dllexport)  freeALL2( PDF * pdf_p );
    uchar * getSpecPage( PDF *pdf_p, int pageno );
    

    #endif
