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
    //   --- ����Ĳ�������Ӧ�ò��֣� ��Ҫժ��ȥ
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

    // �������������Ϣ
    // 1. tm ��Ϣ��
    typedef struct __tm__ {
        // tm ��Ϣ
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }TM;

    // 2. cm ��Ϣ��
    typedef struct __cm__ {
        // tm ��Ϣ
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }CM;

    // ������Ϣ
    #define L_OPR       32
    typedef struct __cur_xy__ {
        float       x;          // �������x
        float       y;          // �������y
        float       ox;         // ԭʼx ����, ����tm��x������ 
        float       oy;         // ԭʼy ����, ����tm��y������ 
        char        opr[L_OPR];    // ������, ����"Td", "TD"��
    }CUR_XY;

    // ��Ԫ����Ϣ
    /*
    typedef struct __cell__ {
        int                 id;             // ��Ŵ�1��ʼ , �Զ�����
        float               x;
        float               y;
        float               w;
        float               h;
        int                 maxlines;
        int                 maxlen;         // ����ı�����
        int                 rows;           // ����
        int                 cols;           // ����
        struct __cell__  *  last;           // ����ָ�����һ��cell
        struct __cell__  *  prev;           // ǰһ����Ԫ��
        struct __cell__  *  next;           // ��һ����Ԫ��
        int              *  txtIDs_p;       // ��Ԫ���е��ı����(����)
        int                 txtTotal;       // ��Ԫ���е��ı��������
    }CELL;
    */
    // ���� �����ʽ���������, ���� last ָ�븳ֵ�ܳ��ڴ�й¶�Ĵ���
    // �Ų���鷳, ��������, ��������, ����һҳ�еı���������ᳬ��512��
    // �������, �����α�cursor, ��Ϊ�±����ȡ�������
    // ɾ�� last, prev, next ָ��, ��Ϊ����ͨ���±���з�����
    // cursor ���ⲿ����, ������λ��Ӧ��cell
    typedef struct __cell__ {
        float               x;
        float               y;
        float               w;
        float               h;
        int                 maxlines;
        int                 maxlen;         // ����ı�����
        int                 rows;           // ����
        int                 cols;           // ����
        int                 txtTotal;       // ��Ԫ���е��ı��������
        int              *  txtIDs_p;       // ��Ԫ���е��ı����(����)
    }CELL;

    // �ı���Ϣ
    /*
    typedef struct __text__ {
        int                 id;
        float               ox;
        float               oy;
        char            *   buf;
        int                 len;
        struct __text__  *  last;
        struct __text__  *  prev;       
        struct __text__  *  next;
        int                 cellID;     // ���ı������ĸ�CELL, �����0��ʾ����cell��
    } TEXT;
    */

    typedef struct __text__ {
        int                 id;
        float               ox;
        float               oy;
        char            *   buf;
        int                 len;
        int                 cellID;     // ���ı������ĸ�CELL, �����0��ʾ����cell��

    } TEXT;

    #define L_MAXCELL   512     // һҳ�е���� ��Ԫ������
    #define L_MAXROWS   512     // һҳ�е���� �ı����� , ������Ԫ���ڵ��ı�����

    typedef struct __decode__ {
        CUR_XY      cur_xy;
        TM          tm;
        char        tf[L_FONTNAME];     // ��ǰʹ�õ���������(��Ӧcmap�е�fontname)
        int         txtRows;                // �ı������� Ҳ����textMap �����е���Ч�ı�����
        TEXT        textMap[L_MAXROWS];
        int         c_cursor;               // ��Ԫ����α�, ����ָ�����һ��֮����±�(���λ���Ǵ����cell��λ��),ȱʡ��0, Ҳ���ǵ�һ��
        CELL        cellMap[L_MAXCELL];        // ������ʽ��Ҫre
    }DECODE;

    int     parsePDF( char * desfile,  char * srcfile );
    PDF   * parsePDF1( char * srcfile );
    void  __declspec(dllexport)  freeALL2( PDF * pdf_p );
    uchar * getSpecPage( PDF *pdf_p, int pageno );
    

    #endif
