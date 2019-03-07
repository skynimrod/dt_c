#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stddef.h>         // wchar_t 
#include <wchar.h>          // wchar_t ���͵��ַ����ַ���������
#include <locale.h>
#include "str_tools.h"

    #define L_FONTNAME      64
    typedef struct __tm__ {
        // tm ��Ϣ
        float       a;
        float       b;
        float       c;
        float       d;
        float       e;
        float       f;
    }TM;


    #define L_OPR       32
    typedef struct __cur_xy__ {
        float       x;          // �������x
        float       y;          // �������y
        float       ox;         // ԭʼx ����, ����tm��x������ 
        float       oy;         // ԭʼy ����, ����tm��y������ 
        char        opr[L_OPR];    // ������, ����"Td", "TD"��
    }CUR_XY;

    // ��Ԫ����Ϣ
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
        struct __cell__  *  prev;           // ǰһ����Ԫ��
        struct __cell__  *  next;           // ��һ����Ԫ��
        int              *  txtIDs_p;       // ��Ԫ���е��ı����(����)
        int                 txtTotal;       // ��Ԫ���е��ı��������
    }CELL;

    // �ı���Ϣ
    typedef struct __text__ {
        int                 id;
        float               ox;
        float               oy;
        char            *   buf;
        int                 len;
        struct __text__  *  prev;       
        struct __text__  *  next;
        int                 cellID;     // ���ı������ĸ�CELL, �����0��ʾ����cell��
    } TEXT;

    typedef struct __decode__ {
        CUR_XY      cur_xy;
        TM          tm;
        char        tf[L_FONTNAME];     // ��ǰʹ�õ���������(��Ӧcmap�е�fontname)
        TEXT      * ltp;                // last text pointer
        TEXT      * tp;                 // textMap pointer
        CELL      * lcp;                // last cell pointer
        CELL      * cp;                 // cellMap pointer ������ʽ��Ҫre
    }DECODE;

    void freeDecode( DECODE * decode_p )
    {

        CELL        * cp;
        TEXT        * tp;
        
        if ( !decode_p )
            return;

        // 1. �ͷ� cellMap ��������ڴ�
        cp    = decode_p->cp;
        while ( cp ) {
            if ( cp->txtIDs_p ) 
                free( cp->txtIDs_p );
            cp = cp->next;           
        } 

        // 2. �ͷ� textMap��������ڴ�
        tp = decode_p->tp;
        
        while( tp ) {
            if (tp->buf ) 
                free( tp->buf  );

            tp = tp->next;
        }

        // 3. �ͷ�decode_p
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
                        printf("�ѵ���Ϊ��!!\n");
                        //printf( "%d ", cp->txtIDs_p[j] ); 
                } else {
                        printf("�ı�����ǿ�\n");
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
