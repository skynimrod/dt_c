#include <stdio.h>
#include "filemap.h"
#include "memorymap.h"

/* ���� FileMap ����ȡ�ļ����ֽ���*/
void main(void)
{
	char srcFile[]="readme.txt";
    uchar   * buf = NULL;

    FILEMAP     *fm_p;
    MEMORYMAP   *mm_p;
    
    // 1. ���Գ�ʼ�� 
    fm_p = initReadFileMap( srcFile );

    mm_p = initMemoryMap( fm_p->stream, fm_p->streamlen );
   
    // 2. ���� ��ȡһ��
    buf = mm_readLine( mm_p );

    if ( buf ) {
        printf("��һ��:\n%s\n", buf );
        free(buf);
    }

    buf = mm_readLine( mm_p );

    if ( buf ) {
        printf("�ڶ���:\n%s\n", buf );
        free(buf);
    }

    // 3. ���Դӵ�ǰλ�ö�ȡָ���ֽ���
    buf = mm_read( mm_p, 11, 0 );   // �ӿ�ʼ��ȡ11���ֽ�, ����Ϊ". ���ʹ��"

    if ( buf ) {
        printf("ǰ11�ֽ�Ϊ:\n%s\n", buf );
        free(buf);
    }

    // 4. ����fm_seek()
    printf("\n��λǰ\n");
    print_mm( mm_p );
    mm_seek( mm_p, 101 );

    printf("\n��λ��\n");
    print_mm( mm_p );

    buf = mm_readLine( mm_p );
    if ( buf ) {
        printf("��λ��101��ȡһ��:\n%s\n", buf );
        free(buf);
    }
    
    printf("\n��λǰ\n");
    print_mm( mm_p );
    mm_seek( mm_p, -100 );

    printf("\n��λ��\n");
    print_mm( mm_p );

    buf = mm_readLine( mm_p );
    if ( buf ) {
        printf("��λ��101��ȡһ��:\n%s\n", buf );
        free(buf);
    }

    freeFileMap( fm_p );

    freeMemoryMap( mm_p);

    return;
}
