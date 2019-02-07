#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>       // �����������ʱ����ʱ��������
#include <sys/stat.h>
#include "zlib.h"

#if defined(WINDOWS) || defined(WIN32)
#define FLAG  1
#else
#define FLAG 0
#endif

#define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
        fprintf(stderr, "%s error: %d\n", msg, err); \
        exit(1); \
    } \
}

    long  __filesize( char * filename )
    {
	    struct stat statbuf;
	    long   	ret;

	    stat( filename, &statbuf );

	    ret = statbuf.st_size;
            printf("filename=%s,Drive letter of file: %c\n", filename,  'A'+statbuf.st_dev); 
            printf("Size of file in bytes: %ld\n", statbuf.st_size); 
            printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime)); 
            return ret; 
    }

size_t getFileSize( char *fname ) 
{
        size_t   len;
        FILE    *fp;

        if ( ! (fp = fopen(fname, "rb" )) )
                return -1;
        fseek( fp, 0, SEEK_END );
        len = ftell( fp );

        printf("\n%s size = %ld\n", fname, len);

        size_t *buf= (size_t *)malloc(len+1);
        size_t *desbuf = (size_t *) malloc( len+1);
        memset( buf, 0, len+1 );
        memset( desbuf, 0, len + 1);

        fseek( fp, 0, SEEK_SET );   // �ص��ļ�ͷ
        size_t l = fread( buf, 1, len, fp );

        printf("\n��ȡ���������ݳ���:%ld\n", l);
        
        int err;
        size_t  dl;
        err = uncompress( desbuf, &dl, buf, len);

        CHECK_ERR(err, "uncompress");

        printf("\n��ѹ�������Ϊ%s\n---\n����Ϊ%ld\n", desbuf, dl);

        

        fclose(fp);

        fp = fopen( "./runcompress.dat", "wb");
        if ( fp ) {
                fwrite( desbuf, 1, len, fp );
                fclose(fp);
        }

        return len;

}


void main(void)
{
	char buf[]="    hello   world !   ";
   
   printf("\n%s\n", buf);
   printf("\nɾ������Ŀո�:%s, buf=%s\n", ltrim(buf), buf );
   printf("\nɾ���м�Ŀո�:%s\n", mtrim(buf));

   if ( FLAG == 1 ) {
           printf("\nϵͳ������WINDOWS �� WIN32\n");
   }else {
           printf("\nϵͳû����WINDOWS �� WIN32\n");
   }

   //__filesize( "./r.dat");

   getFileSize("F:/F_T_tmp/cmap/c_9_stream.txt");

    return;
}
