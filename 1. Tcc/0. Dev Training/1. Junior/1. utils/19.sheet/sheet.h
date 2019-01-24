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

    // PDF_Parse-> decode() ��ʹ��
    /*
     *     # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {���:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {���:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # һֱ��ŵ������µ�tm����
     * */
    
    typedef struct __textmap {      // �ı���Ϣ�ڵ�
        uint        cellid;         // ��Ԫ����
        struct      txt {
            char  * buf;            // �ı�����
            int     x;              // x ����
            int     y;              // y ����
        };
        TEXT_MAP *  pre;            // ǰһ�� �ı���Ϣ�ڵ�
        TEXT_MAP *  next;           // ��һ���ı���Ϣ�ڵ�
    }TEXT_MAP;

    typedef struct __cellmap {
    }CELL_MAP;

    typedef struct __cellIndexMap {
    } CELL_INDEX_MAP;

    // about FILEMAP  �Ĳ���
    void        print_fm( FILEMAP * fm_p );
    FILEMAP *   initReadFileMap( char * srcFile );
    void        freeFileMap( FILEMAP * fm_p );
    uchar *     fm_readLine( FILEMAP *fm_p );
    uchar *     fm_read( FILEMAP *fm_p, long length, long start );
    void        fm_seek( FILEMAP  * fm_p, long pos );


    #endif                   // define sheet.h ENd }