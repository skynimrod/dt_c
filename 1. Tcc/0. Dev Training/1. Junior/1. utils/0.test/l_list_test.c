#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stddef.h>         // wchar_t 
#include <wchar.h>          // wchar_t 类型的字符或字符串处理函数
#include <locale.h>
#include "str_tools.h"

    #define L_FONTNAME      64
    typedef struct __tm__ {
        // tm 信息
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }TM;


    #define L_OPR       32
    typedef struct __cur_xy__ {
        float       x;          // 相对坐标x
        float       y;          // 相对坐标y
        float       ox;         // 原始x 坐标, 经过tm与x计算获得 
        float       oy;         // 原始y 坐标, 经过tm与y计算获得 
        char        opr[L_OPR];    // 操作符, 比如"Td", "TD"等
    }CUR_XY;

    // 单元格信息
    typedef struct __cell__ {
        int                 id;             // 编号从1开始 , 自动产生
        float               x;
        float               y;
        float               w;
        float               h;
        int                 maxlines;
        int                 maxlen;         // 最长的文本长度
        int                 rows;           // 行数
        int                 cols;           // 列数
        struct __cell__  *  prev;           // 前一个单元格
        struct __cell__  *  next;           // 后一个单元格
        int              *  txtIDs_p;       // 单元格中的文本编号(数组)
        int                 txtTotal;       // 单元格中的文本编号数量
    }CELL;

    // 文本信息
    typedef struct __text__ {
        int                 id;
        float               ox;
        float               oy;
        char            *   buf;
        int                 len;
        struct __text__  *  prev;       
        struct __text__  *  next;
        int                 cellID;     // 该文本属于哪个CELL, 如果是0表示不在cell中
    } TEXT;

    typedef struct __decode__ {
        CUR_XY      cur_xy;
        TM          tm;
        char        tf[L_FONTNAME];     // 当前使用的字体名称(对应cmap中的fontname)
        TEXT      * ltp;                // last text pointer
        TEXT      * tp;                 // textMap pointer
        CELL      * lcp;                // last cell pointer
        CELL      * cp;                 // cellMap pointer 处理表格式需要re
    }DECODE;

    void freeDecode( DECODE * decode_p )
    {

        CELL        * cp;
        TEXT        * tp;
        
        if ( !decode_p )
            return;

        // 1. 释放 cellMap 中申请的内存
        cp    = decode_p->cp;
        while ( cp ) {
            if ( cp->txtIDs_p ) 
                free( cp->txtIDs_p );
            cp = cp->next;           
        } 

        // 2. 释放 textMap中申请的内存
        tp = decode_p->tp;
        
        while( tp ) {
            if (tp->buf ) 
                free( tp->buf  );

            tp = tp->next;
        }

        // 3. 释放decode_p
        free( decode_p );

    }

    void newCell( DECODE *decode_p )
    {
        CELL        * cpl;
        CELL        * cp;

        //cp = ( CELL * )calloc( sizeof(CELL), 1 );
        cp = (CELL *)malloc(sizeof(CELL));
        memset( cp, 0, sizeof(CELL));

        srand(50);
        cp->id = rand();
        
        cpl = decode_p->lcp;

        if ( !cpl ){
            decode_p->cp = cp;
            cp->prev = NULL;
            cp->next = NULL;
            cp->txtTotal += 1;
        } else {
            cpl->next = cp;
            cp->prev = cpl;
            cp->id = cpl->id + 1;
            cp->txtTotal += 1;
        }
            decode_p->lcp = cp;

    }

    void genCell( DECODE * decode_p )
    {   
        for ( int i = 0; i < 10; i ++ ) {
            newCell( decode_p );
        }
    }

    void genText( DECODE * decode_p )
    {
    }

    void printCellMap( CELL * cellMap_p )
    {
        CELL        * cp;

        cp = cellMap_p;

        while ( cp ) {
            printf( "-------------cell id=%d-----------\n", cp->id );
            printf( "x=%.2f,y=%.2f,w=%.2f, h=%.2f\n", cp->x, cp->y, cp->w, cp->h );
            printf( "rows=%d, cols=%d\n",  cp->rows, cp->cols );
            printf( "maxlines=%d, maxlen=%d, txtTotal=%d\n", cp->maxlines, cp->maxlen, cp->txtTotal );
            for ( int j = 0; j < cp->txtTotal; j ++ ) {
                if ( cp->txtIDs_p ) {
                        printf("难道不为空!!\n");
                        //printf( "%d ", cp->txtIDs_p[j] ); 
                } else {
                        printf("文本编号是空\n");
                }
            }
            printf("\n");

            cp = cp->next;
        }

    }


char * decode( char * srcbuf , int len ) 
    {
        DECODE      * decode_p;

        decode_p = (DECODE *)calloc( sizeof(DECODE), 1 );

        genCell( decode_p );

        printCellMap( decode_p->cp );

        genText( decode_p );

        freeDecode( decode_p );
    }

void main(void)
{
    char    * tmpbuf;

    for(int i =0; i < 10; i ++ );
    decode( tmpbuf , 10 );
        
    return;
}
