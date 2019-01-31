#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include "str_tools.h"

    typedef struct __contentMap__ {     // 记录每页中的内容页面对象
        int         obj;            // 内容对象编号
        int         * objlist;      // 内容对象编号所含的对象列表
    }CONTENTMAP;

// 计算指定字符在字符串中出现的次数
    int  test2( CONTENTMAP *cm ) 
    {
        srand(time(NULL));
        cm->obj =  rand()%10;
        while( !cm->obj)
            rand() % 10;
        cm->objlist = (int *)malloc(sizeof(int)*cm->obj + 1);

        for ( int i =0; i < cm->obj; i ++ )
            cm->objlist[i] = rand()%10;


        return 0;
    }

void main(void)
{
    char    buf[]="Kids[3 0 R 12 0 R] ";

    CONTENTMAP  ** contentMap;

    contentMap = (CONTENTMAP **)malloc(sizeof(CONTENTMAP *)*5+1);
    for ( int i = 0; i < 5; i ++) {
        contentMap[i] = (CONTENTMAP *)malloc( sizeof(CONTENTMAP) + 1);
        test2( contentMap[i]);
    }


    for ( int i = 0; i < 5; i ++) {
        printf("\n第%d个: obj=%d\n",i, contentMap[i]->obj);
        for( int j = 0; j < contentMap[i]->obj; j ++ ) {
            printf( "   %d:%d;", j, contentMap[i]->objlist[j] );
        }

    }
    
    for ( int i = 0; i < 5; i ++) {
            if ( contentMap[i]->objlist ) 
               free(contentMap[i]->objlist);
        free( contentMap[i]);
    }
    free( contentMap );
    return;
}
