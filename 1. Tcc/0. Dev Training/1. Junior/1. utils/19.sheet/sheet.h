    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    sheet
    //
    // Description : about sheet operation.
    //
    // Interface :
    //      1.  initReadFileMap()
    //
    #ifndef __SHEET_H__    // define sheet.h Start {
    #define __SHEET_H__

    typedef unsigned char  uchar;

    // PDF_Parse-> decode() 中使用
    /*
     *     # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {编号:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {编号:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # 一直存放的是最新的tm数据
     * */
    
    typedef struct __textmap {      // 文本信息节点
        uint        cellid;         // 单元格编号
        struct      txt {
            char  * buf;            // 文本内容
            int     x;              // x 坐标
            int     y;              // y 坐标
        };
        TEXT_MAP *  pre;            // 前一个 文本信息节点
        TEXT_MAP *  next;           // 后一个文本信息节点
    }TEXT_MAP;

    typedef struct __cellmap {
    }CELL_MAP;

    typedef struct __cellIndexMap {
    } CELL_INDEX_MAP;

    // about FILEMAP  的操作
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initReadFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );
    uchar *     fm_readLine( FILEMAP *fm_p );
    uchar *     fm_read( FILEMAP *fm_p, long length, long start );
    void        fm_seek( FILEMAP  * fm_p, long pos );


    #endif                   // define sheet.h ENd }
