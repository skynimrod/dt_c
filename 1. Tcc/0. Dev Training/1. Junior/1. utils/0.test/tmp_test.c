#include <stdio.h>
#include <stdlib.h>

    char * nchar( char * buf, int n, char ch )
    {

        if ( n < 1 )
            return NULL;
        
        for ( int i = 0; i < n; i ++ ) 
            buf[i] = ch;

        return buf;
    }

    char * nstr( char * buf, int buflen, int n, char  *str )
    {
        int     len;

        if ( !buf || n < 1 || !str )
            return NULL;

        if ( buflen < n*strlen(str) )
            return NULL;
        
        len = strlen( str );
        for ( int i = 0; i < n; i ++ ) {
            memcpy( buf + i * len, str, len );
        }

        return buf;
    }


void main(void)
{

    int     src[]={6,2,1,9,12,3,28,5,4,8,7};

    int     step;

    int     i = 0;
    int     mid;
    
    int     len = sizeof(src)/sizeof(int);

    step = len/2;
    
    printf("≈≈–Ú«∞. sizeof(src)=%d\n", len );
    
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

    printf("≈≈–Ú∫Û\n");
    for ( i = 0; i < len; i ++ )
        printf(" %d", src[i] );
    printf("\n");

    char buf[512];
    memset( buf, 0, 512 );
    nchar( buf, 10, 'a' );
    printf( "%s\n", buf );
    
    #define ROWLINE         "©§"
    memset( buf, 0, 512 );
    nstr( buf, 512, 10, ROWLINE );
    printf( "%s\n", buf );

    memset( buf, 0, 512 );
    memcpy( buf, ROWLINE, strlen(ROWLINE));
    printf( "%s\n", buf );

    return;
}
