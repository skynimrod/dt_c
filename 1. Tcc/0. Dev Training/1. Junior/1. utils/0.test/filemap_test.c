#include <stdio.h>
#include "filemap.h"


void main(void)
{
	char srcFile[]="readme.txt";
    uchar   * buf = NULL;

    FILEMAP     *fp;
    
    // 1. ���Գ�ʼ��
    fp = initReadFileMap( srcFile );
    

    // 2. ���� ��ȡһ��
    buf = fm_readLine( fp );

    if ( buf ) {
        printf("��һ��:\n%s\n", buf );
        free(buf);
    }

    buf = fm_readLine( fp );

    if ( buf ) {
        printf("�ڶ���:\n%s\n", buf );
        free(buf);
    }

    // 3. ���Դӵ�ǰλ�ö�ȡָ���ֽ���
    buf = fm_read( fp, 11, 0 );   // �ӿ�ʼ��ȡ11���ֽ�, ����Ϊ". ���ʹ��"

    if ( buf ) {
        printf("ǰ11�ֽ�Ϊ:\n%s\n", buf );
        free(buf);
    }

    // 4. ����fm_seek()
    printf("\n��λǰ\n");
    print_fm( fp );
    fm_seek( fp, 101 );

    printf("\n��λ��\n");
    print_fm( fp );

    buf = fm_readLine( fp );
    if ( buf ) {
        printf("��λ��101��ȡһ��:\n%s\n", buf );
        free(buf);
    }
    
    printf("\n��λǰ\n");
    print_fm( fp );
    fm_seek( fp, -100 );

    printf("\n��λ��\n");
    print_fm( fp );

    buf = fm_readLine( fp );
    if ( buf ) {
        printf("��λ��101��ȡһ��:\n%s\n", buf );
        free(buf);
    }
    
    freeFileMap( fp );

    return;
}
