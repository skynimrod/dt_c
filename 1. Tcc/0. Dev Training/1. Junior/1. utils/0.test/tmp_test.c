#include <stdio.h>
#include <stdlib.h>

void main(void)
{

    int     src[]={6,2,1,9,12,3,28,5,4,8,7};

    int     step;

    int     i = 0;
    int     mid;
    
    int     len = sizeof(src)/sizeof(int);

    step = len/2;
    
    printf("����ǰ. sizeof(src)=%d\n", len );
    
    for ( i = 0; i < len; i ++ )
        printf(" %d", src[i] );
    printf("\n");

    while( step > 0 ) {
        i = 0;
        while( i < len-step ){
            if ( src[i] > src[i +step] ) {
                mid         = src[i];
                src[i]      = src[i+step];
                src[i+step] = mid;
            }
            i ++;
        }
        step --;
    }

    printf("�����\n");
    for ( i = 0; i < len; i ++ )
        printf(" %d", src[i] );
    printf("\n");

    
    return;
}
