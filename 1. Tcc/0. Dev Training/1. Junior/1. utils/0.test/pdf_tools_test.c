#include <stdio.h>
#include "filemap.h"
#include "pdf_tools.h"


void main(void)
{
	char    srcFile[] = "F:/F_T_tmp/1202.PDF";
    char    desFile[] = "F:/F_T_tmp/1202_0118.txt";
    
    int     ret;
    PDF     * pdf_p;
    int     * ta;

    //ret = parsePDF( desFile, srcFile );

    pdf_p = parsePDF1( srcFile );

    //printf( "\nret = %d\n", ret );

    getSpecPage( pdf_p, 1 );

    freeALL2( pdf_p );


    return;
}
