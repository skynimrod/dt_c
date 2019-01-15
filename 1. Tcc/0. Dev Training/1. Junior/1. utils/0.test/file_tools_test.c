#include <stdio.h>

#include "file_tools.h"

    
    void main(void)
    {   
        // 1. test ifOpened()    有点问题, 返回一直是1
        int  flag1 = ifOpened( "./r.dat" );
        printf("\n0  未被打开  1  已经被打开   -2  无此文件  \nr.dat 的打开状态为%d\n", flag1);

        // 2. test  getFileSize()
        size_t   size = getFileSize("./r.dat");
        printf("\nsize of r.dat=%ld\n", size);

        return;
    }
