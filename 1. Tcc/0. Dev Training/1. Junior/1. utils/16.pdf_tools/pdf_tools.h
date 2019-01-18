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

    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"

    typedef struct __xref__ {
    }XREF;

    typedef struct __pages__ {
    }PAGES;

    typedef struct __cmaplist__ {
    }CMAPLIST;

    typedef struct __cmaps__ {
    } CMAPS;

    typedef struct __contentMap__ {

    }CONTENTMAP;

    typedef struct __tableContentMap__ {
    }TABLECONTENTMAP;

    // Pdf 解析核心 结构体
    typedef struct __pdfParser {
        FILEMAP         *   fm_p;
        MEMORYMAP       *   mm_p;
        XREF            *   xref_p;
        pages           *   pages_p;
        CMAPLIST        *   cmaplist_p;
        CMAPS           *   cmaps_p;
        TABLECONTENTMAP *   tableContentMap_p;        
    } PDFPARSER;
    #define __PDF_TOOLS_H__

