#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stddef.h>         // wchar_t 
#include <wchar.h>          // wchar_t ���͵��ַ����ַ���������
#include <locale.h>
#include "str_tools.h"

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
        struct __cell__  *  last;           // ����ָ�����һ��cell
        struct __cell__  *  prev;           // ǰһ����Ԫ��
        struct __cell__  *  next;           // ��һ����Ԫ��
        int              *  txtIDs_p;       // ��Ԫ���е��ı����(����)
        int                 txtTotal;       // ��Ԫ���е��ı��������
    }CELL;

   int newCell( CELL * cellMap_p, CELL *cp )
    {
        printf("newCell() begin. cellMap_p=%p, cp=%p\n", cellMap_p, cp );

        if ( !cellMap_p ) {     // ����Ϊ���ڵ�һ��cellֱ�Ӵ�����,�����Ӧ��Ϊ��, �ݴ�
            printf("============�����ˣ�����������\n");
            return -1;
        } else {        // ��ӵ�β��
            printf("############################cellMap_p ��Ϊ�� ��\n");
            cellMap_p->last->next = cp;
            cp->prev = cellMap_p->last;
            cp->id = cellMap_p->last->id + 1;
            cellMap_p->last = cp;
            return 0;
        }

    }

void main(void)
{

    CELL    * cp, * cellMap_p;

    for ( int i = 0; i < 5; i ++ ) {
        
    }

        
    return;
}
