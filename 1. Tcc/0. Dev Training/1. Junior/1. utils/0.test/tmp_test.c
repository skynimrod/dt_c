#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stddef.h>         // wchar_t 
#include <wchar.h>          // wchar_t 类型的字符或字符串处理函数
#include <locale.h>
#include "str_tools.h"

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
        struct __cell__  *  last;           // 总是指向最后一个cell
        struct __cell__  *  prev;           // 前一个单元格
        struct __cell__  *  next;           // 后一个单元格
        int              *  txtIDs_p;       // 单元格中的文本编号(数组)
        int                 txtTotal;       // 单元格中的文本编号数量
    }CELL;

   int newCell( CELL * cellMap_p, CELL *cp )
    {
        printf("newCell() begin. cellMap_p=%p, cp=%p\n", cellMap_p, cp );

        if ( !cellMap_p ) {     // 链表为空在第一个cell直接处理了,这儿不应该为空, 容错
            printf("============出错了！！！！！！\n");
            return -1;
        } else {        // 添加到尾部
            printf("############################cellMap_p 不为空 ！\n");
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
