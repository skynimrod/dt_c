#include <stdio.h>
#include <malloc.h>

    int test( int count, int    * list )
    {
            for (int i = 0; i < count; i ++) {
                list[i] += i;
            }
            return 0;
    }


void main(void)
{

    int      * list;    
    int     count;
    int     i;

    count = 3;
    list = (int *)malloc( count + 1);
    memset(list, 0, sizeof(int)*count+1);

    for ( i = 0; i < count; i ++) {
        printf( " before list[%d] = %d\n", i, list[i]);
    }    

    test( count , list );
    for ( i = 0; i < count; i ++) {
        printf( " after list[%d] = %d\n", i, list[i]);
    }    
    return;
}
