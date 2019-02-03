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
        int     n = 5;

        CONTENTMAP *tt;

        tt = ( CONTENTMAP *)malloc( sizeof(CONTENTMAP) * n + 1);
        memset( tt, 0, sizeof(CONTENTMAP*)+1);

        for ( int i = 0; i < n; i ++) {
            //tt[i] = (CONTENTMAP *)malloc( sizeof( CONTENTMAP ) +1 );
            //memset(tt[i], 0, sizeof(CONTENTMAP)+1);

            tt[i].obj = i;
            tt[i].objlist = &(tt[i].obj);
        }

        for ( int i = 0; i < n; i ++) {
            //printf( "tt[%d]->obj=%d, tt[%d]->oblist address=%p\n", i, tt[i]->obj, i, tt[i]->objlist );
            printf( "tt[%d].obj=%d, tt[%d].oblist address=%p\n", i, tt[i].obj, i, tt[i].objlist );
        }

        /*
        for ( int i = 0; i < n; i ++) {
            free( tt[i] );
        }*/
        free(tt);


        return 0;
    }
    // [5278 0 R 5279 0 R 5280 0]
    char * strsplit( char * buf, char ch, int n ) 
    {
        int     i = 0 ;
        int     len = strlen(buf);
        int     m=0;
        char    * retbuf;
        int     k =0;       // 记录每一项的开始位置

        printf("buf=%s!\n", buf);
        
        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                printf("m=%d, n=%d\n", m, n);
                if ( m == n ) {     // 找到第几个项的位置了
                    retbuf = (char *)malloc( i-k+1);
                    memset( retbuf, 0, i-k+1);   
                    memcpy( retbuf, buf+k, i-k );  
                    return retbuf;
                }
                while( buf[i] == ch )       // 跳过连续的分隔符
                    i ++;
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        
        return NULL;
    }


void main(void)
{
    char    buf[]="FT8 8 0 R>>>>";

    CONTENTMAP  * contentMap;

        test2( contentMap);

    char *item;

    printf( "sizeof(int)=%d\n\n", sizeof(int));
    item= (char*) strsplit( buf, ' ', 1 ) ;

    printf("buf=%s, 第%d 项:item=%s", buf, 1, item );
    free(item);

    item= (char*) strsplit( buf, ' ', 2 ) ;

    printf("buf=%s, 第%d 项:item=%s", buf, 2, item );
    free(item);

    return;
}
