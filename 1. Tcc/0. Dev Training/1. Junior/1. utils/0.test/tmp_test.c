#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <stddef.h>         // wchar_t 
#include <wchar.h>          // wchar_t 类型的字符或字符串处理函数
#include <locale.h>
#include "str_tools.h"


    int Thex2Int(char *hex);
    int Char2Int( char hex ); 
    //"547f" -> 7f, 54
    unsigned int Fhex2int( char * hex)
    {
        if ( !hex )
            return -1;

        if ( strlen(hex) != 4 )
            return -1;

        int h, l;   

        l = Thex2Int( &hex[2] );
        hex[2] = 0;
        h = Thex2Int( hex ); 

        unsigned int  ret;

        ret = (h << 8) + l;
        return ret;
    }

    // 2位hex字符串转整型数值 Two Hex To Int
    int Thex2Int(char *hex)
    {
        if ( !hex )
            return -1;

        if ( strlen(hex) != 2 )
            return -1;
    
        return ( Char2Int( hex[0] ) << 4 ) + Char2Int( hex[1] );
    }

    // 1位hex字符串转整型数值 A Hex To Int
    int Char2Int( char hex ) 
    {
        if ( hex >= '0' && hex <= '9' )
            return hex - '0';
        if ( hex >= 'A' && hex <= 'F' )
            return hex - 'A' + 10;
        if ( hex >= 'a' && hex <= 'f' )
            return hex - 'a' + 10;
        return -1;
    }

    int unicode2str( char * desbuf, int len, wchar_t * wstr ) 
    {
        int ret = 0;
        
        printf("%s\n",setlocale(LC_ALL,""));

        ret = wcstombs( desbuf, wstr, len );
        printf("ret = %d\n",ret);
        printf("%s\n",desbuf );

        return ret;
    }
    char * strsplit2( char * buf, int len, char ch, int n ) 
    {
        int     i = 0 ;
        int     m=0;
        char    retbuf[len+1];
        int     k =0;       // 记录每一项的开始位置

        while( i < len )  {
            if ( buf[i] == ch ) {
                m ++;
                printf("m=%d, n=%d\n", m, n);
                if ( m == n ) {     // 找到第几个项的位置了
                    memset( retbuf, 0, len+1 );   
                    memcpy( retbuf, buf+k, i-k );
                    printf("retbuf=%s\n", retbuf);
                    return retbuf;
                }
                while( buf[i] == ch )  {     // 跳过连续的分隔符
                    i ++;
                }
                k = i;  // 记录寻找项之前的最近的分隔符位置
                continue;
            }
            i ++;
        }

        if ( i == len && m == n-1 ) {   // 最后一项
            memset( retbuf, 0, len+1 );   
            memcpy( retbuf, buf+k, i-k );
            return retbuf;
        }
        return NULL;
    }

void main(void)
{
int ret = 0;
char str[256];
wchar_t wstr[] = {40527,0x50cf, 0x30, 0x31, 0};

printf("%s\n",setlocale(LC_ALL,""));

ret = wcstombs(str,wstr,sizeof(str)/sizeof(char));
printf("ret = %d\n",ret);
printf("%s\n",str);

    unsigned int ui;

    ui = Fhex2int("9E4F");
    printf("0x9E4F=%d-------\n", ui);
    printf("%04X\n", ui);

    
    ret = unicode2str( str, 256, wstr );
    printf("str=%s, len=%d\n", str, ret );

    sprintf( str, "100 beginbfchar");
    printf( "str=%s\n", str);
    printf("???%s???\n", strsplit2( str, 30, ' ', 1 ) );

    ui = wstr[0];

    printf("wstr[0]=%d, ui=%d\n", wstr[0], ui);

    
    return;
}
