#include <stdio.h>

#include "file_tools.h"

    
    void main(void)
    {   
        // 1. test ifOpened()    �е�����, ����һֱ��1
        int  flag1 = ifOpened( "./r.dat" );
        printf("\n0  δ����  1  �Ѿ�����   -2  �޴��ļ�  \nr.dat �Ĵ�״̬Ϊ%d\n", flag1);

        // 2. test  getFileSize()
        size_t   size = getFileSize("./r.dat");
        printf("\nsize of r.dat=%ld\n", size);

        return;
    }
