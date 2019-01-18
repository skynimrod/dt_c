#include <stdio.h>
#include "filemap.h"


void main(void)
{
	char    srcFile[] = "F:/F_T_tmp/1202.PDF";
    char    desFile[] = "F:/F_T_tmp/1202_0118.txt";
    
    int     ret;

    ret = parsePDF( desFile, srcFile );

    printf( "\nret = %d\n", ret );

    return;
}
