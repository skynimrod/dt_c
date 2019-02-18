#include <stdio.h>
#include <malloc.h>
#include "str_tools.h"
   
typedef struct _aaa_ {
    long    total;
    long    *obj;
}AAA;

int * test( int * a, int * b )
{
    *a = 18;
    int     * x;

    x = (int *) malloc(sizeof(int) * 10);

    for (int i =0; i < 10; i ++) {
            x[i] = i;
    }

    printf( "*x=%d,x=%d(%p)\n",*x, &x, x);
    return x;
}
void main(void)
{
    char buf[]="    hello   world !   ";
   
    printf("\n%s\n", buf);
    printf("\n删除左面的空格:%s, buf=%s\n", ltrim(buf), buf );
    printf("\n删除中间的空格:%s\n", mtrim(buf));
    printf("\nbuf=%s\n", buf);

    
    int      a=0;
    int     * b;

    printf("a=%d, address of b = %p", a, b);
    b = test( &a, b );
    printf("a=%d, address of b = %p", a, b);

    for ( int i=0; i < 10; i ++) {
       printf("b[%d]=%d\n",i, b[i]);
    }

    sprintf( buf, "<0005> <0022>" );
    DelCharsInString( buf, "<>" );
    printf( "buf=%s|\n", buf );

    int     ret;

    wchar_t wstr[]={0x9e4f, 0x534e, 0 };
    ret = unicode2str( buf, 12, wstr );

    printf("buf=%s, len=%d\n", buf, ret);

    
    return;
}
