#include <stdio.h>
#include "filemap.h"


void main(void)
{
	char srcFile[]="readme.txt";
    uchar   * buf = NULL;

    FILEMAP     *fp;
    
    // 1. 测试初始化
    fp = initReadFileMap( srcFile );
    

    // 2. 测试 读取一行
    buf = fm_readLine( fp );

    if ( buf ) {
        printf("第一行:\n%s\n", buf );
        free(buf);
    }

    buf = fm_readLine( fp );

    if ( buf ) {
        printf("第二行:\n%s\n", buf );
        free(buf);
    }

    // 3. 测试从当前位置读取指定字节流
    buf = fm_read( fp, 11, 0 );   // 从开始读取11个字节, 内容为". 如果使用"

    if ( buf ) {
        printf("前11字节为:\n%s\n", buf );
        free(buf);
    }

    // 4. 测试fm_seek()
    printf("\n定位前\n");
    print_fm( fp );
    fm_seek( fp, 101 );

    printf("\n定位后\n");
    print_fm( fp );

    buf = fm_readLine( fp );
    if ( buf ) {
        printf("定位到101后取一行:\n%s\n", buf );
        free(buf);
    }
    
    printf("\n定位前\n");
    print_fm( fp );
    fm_seek( fp, -100 );

    printf("\n定位后\n");
    print_fm( fp );

    buf = fm_readLine( fp );
    if ( buf ) {
        printf("定位到101后取一行:\n%s\n", buf );
        free(buf);
    }
    
    freeFileMap( fp );

    return;
}
