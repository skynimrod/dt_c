#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include "str_tools.h"

    typedef struct __contentMap__ {     // ��¼ÿҳ�е�����ҳ�����
        int         obj;            // ���ݶ�����
        int         * objlist;      // ���ݶ����������Ķ����б�
    }CONTENTMAP;

// ����ָ���ַ����ַ����г��ֵĴ���
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
        int     k =0;       // ��¼ÿһ��Ŀ�ʼλ��

        printf("buf=%s!\n", buf);
        
        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                printf("m=%d, n=%d\n", m, n);
                if ( m == n ) {     // �ҵ��ڼ������λ����
                    retbuf = (char *)malloc( i-k+1);
                    memset( retbuf, 0, i-k+1);   
                    memcpy( retbuf, buf+k, i-k );  
                    return retbuf;
                }
                while( buf[i] == ch )       // ���������ķָ���
                    i ++;
                k = i;  // ��¼Ѱ����֮ǰ������ķָ���λ��
                continue;
            }
            i ++;
        }

        
        return NULL;
    }

    void tests( int count, int * tat) 
    {
        char    buf[count];

        sprintf(buf, "Hello world\n");
        printf("buf=%s\n", buf);
    }


void main(void)
{
    char    buf[]="FT8 8 0 R>>>>";

    CONTENTMAP  * contentMap;

        test2( contentMap);

    char *item;

    printf( "sizeof(int)=%d\n\n", sizeof(int));
    item= (char*) strsplit( buf, ' ', 1 ) ;

    printf("buf=%s, ��%d ��:item=%s", buf, 1, item );
    free(item);

    item= (char*) strsplit( buf, ' ', 2 ) ;

    printf("buf=%s, ��%d ��:item=%s\n", buf, 2, item );
    free(item);

    printf("1/2+1%%2-1=%d\n", 1/2+1%2-1);
    printf("3/2+3%%2-1=%d\n", 3/2+3%2-1);
    printf("5/2+5%%2-1=%d\n", 5/2+5%2-1);
    printf("7/2+7%%2-1=%d\n", 7/2+7%2-1);

    float       r;
    int     a, b;
    a = 2806, b = 563;

    r = (float)a/b;

    printf("%d/%d=%.2f\n", a, b, r );

    tests( 5, &a);
    
    char	tmp[]="0 Tc 0 Tr 48.26 0 Td";
    char    buf1[12], buf2[12], buf3[12];

    int     i = 0,j = 0;
    while ( i <= strlen(tmp) ) {  // û��β9
        printf("i=%d, tmp[%d]=%d\n", i, i, tmp[i] );
        if ( tmp[i] == ' ' || tmp[i] == 0 ) {  // �����ո���β
            strcpy( buf1, buf2 );
            strcpy( buf2, buf3 );
            memset( buf3, 0, 12 );
            memcpy( buf3, &tmp[j], i-j );
            if ( strstr(buf3, "Td" ) )
                break;
            j = i+1;  // �ո����һ���ַ�
        }
        i ++;
    }

    printf("buf1=%s, buf2=%s, buf3=%s\n", buf1, buf2, buf3 );


    return;
}
