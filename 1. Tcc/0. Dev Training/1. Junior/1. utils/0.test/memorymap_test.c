#include <stdio.h>
#include "filemap.h"
#include "memorymap.h"

/* 借用 FileMap 来获取文件的字节流*/
void main(void)
{
	char srcFile[]="readme.txt";
    uchar   * buf = NULL;

    FILEMAP     *fm_p;
    MEMORYMAP   *mm_p;
    
    // 1. 测试初始化 
    fm_p = initReadFileMap( srcFile );

    mm_p = initMemoryMap( fm_p->stream, fm_p->streamlen );
   
    // 2. 测试 读取一行
    buf = mm_readLine( mm_p );

    if ( buf ) {
        printf("第一行:\n%s\n", buf );
        free(buf);
    }

    buf = mm_readLine( mm_p );

    if ( buf ) {
        printf("第二行:\n%s\n", buf );
        free(buf);
    }

    // 3. 测试从当前位置读取指定字节流
    buf = mm_read( mm_p, 11, 0 );   // 从开始读取11个字节, 内容为". 如果使用"

    if ( buf ) {
        printf("前11字节为:\n%s\n", buf );
        free(buf);
    }

    // 4. 测试fm_seek()
    printf("\n定位前\n");
    print_mm( mm_p );
    mm_seek( mm_p, 101 );

    printf("\n定位后\n");
    print_mm( mm_p );

    buf = mm_readLine( mm_p );
    if ( buf ) {
        printf("定位到101后取一行:\n%s\n", buf );
        free(buf);
    }
    
    printf("\n定位前\n");
    print_mm( mm_p );
    mm_seek( mm_p, -100 );

    printf("\n定位后\n");
    print_mm( mm_p );

    buf = mm_readLine( mm_p );
    if ( buf ) {
        printf("定位到101后取一行:\n%s\n", buf );
        free(buf);
    }

    freeFileMap( fm_p );

    freeMemoryMap( mm_p);

    return;
}
