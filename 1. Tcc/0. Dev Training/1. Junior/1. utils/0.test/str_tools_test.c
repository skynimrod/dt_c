#include <stdio.h>
#include "str_tools.h"
   
void main(void)
{
    char buf[]="    hello   world !   ";
   
    printf("\n%s\n", buf);
    printf("\nɾ������Ŀո�:%s, buf=%s\n", ltrim(buf), buf );
    printf("\nɾ���м�Ŀո�:%s\n", mtrim(buf));

    return;
}
