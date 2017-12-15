    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    File_tools
    //
    // Description : about file operation.
    //       �μ�: C99  --- 7.19.3   P277
    //                      7.19.4   P279   �����ļ��Ĳ���
    // Interface :
    //        1.   IfFileExist()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  ʹ�� true, false ֵ�Ķ���
    #include    <io.h>                          //  _sopen()
    #include    <sys\stat.h>                    // ������ȡ�ļ���С
    #include    <time.h>

    #include    "file_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    static inline void aprintf( char  * buf );
    char* strcpy(char *dst, const char *src);
    int  add1( int x, int y ); 
    static inline void * my_memcpy(void * to, const void * from, size_t n);
    char *  adprintf( void * buf );

    //**************************************************************************
    //  �������� : ifOpened()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    /*
     * _access() �������ָ��ģʽ�ɹ����ļ�, �򷵻�0, ���򷵻�-1
     */
    DLL_EXPORT int ifOpened( char * filename ) 
    {
            char dst[512];
	    char src[]="hello!$";

	    memset(dst,0,6);
	    my_memcpy(dst, src,6);
	    printf("src=%s, dst=%s\n\n",src,dst);

	    int c = add1(3,5);
	    printf( " 3+5=%d\n", c );

	    
	    char * rr= adprintf( src );

	    __filesize( filename );


	    if ( _access( filename, 0 ) != 0 ) {
		    printf( "file:%s is not exists!\n", filename );
		    aprintf("Hello,world");
		    return -2;
	    }

	    int    h = sopen( filename, 2, 0 );
	    if ( h == 3 ) {
		    _close (h );
		    return 0;            // # �ļ�û�б��κ��˴�
	    }

	    return 1;          //  # �ļ��Ѿ�������
	    /*
	    // check for existence
	    if ( ( _access( filename, 0 ) ) == 0 ) {
		    printf( "file:%s is exists!\n", filename );

		    // check for write permission
		    if ( _access( filename, 2 ) == 0 ) {
			    printf( "file: %s has write permission, can write!", filename );
			    return false;           // ��ʾ�ļ�û�б���
		    }
                    printf( "file: %s has not write permission!", filename );
		    return true;                     //  û��дȨ��, ��ʾ�ļ��Ѿ�������.
	    }
	    */
    }

    //**************************************************************************
    //  �������� : ifOpened()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    /*
     *
     */
    long DLL_EXPORT __filesize( char * filename )
    {
	    struct stat statbuf;
	    long   	ret;

	    stat( filename, &statbuf );

	    ret = statbuf.st_size;
            printf("Drive letter of file: %c\n", 'A'+statbuf.st_dev); 
            printf("Size of file in bytes: %ld\n", statbuf.st_size); 
            printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime)); 
            return ret; 
    }
    
    //**************************************************************************
    //  �������� : ifOpened()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    void DLL_EXPORT printfm( FILEMAP * filemap )
    {
	    if ( filemap == NULL ) {
		printf("filemap is  NULL \n");
	        return;
	    }
            printf("filemap->filename=%s\n", filemap->filename );
            printf("filemap->filelen=%d\n", filemap->filelen );
            printf("filemap->pos=%d\n", filemap->pos );

	    return;
    }

    //**************************************************************************
    //  �������� : ifOpened()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    //
    FILEMAP * DLL_EXPORT initFileMap( char * srcFile, FILEMAP * filemap )
    {
	filemap = ( FILEMAP * )malloc( sizeof( FILEMAP ) );
	printf("Size of FILEMAP=%d\n", sizeof(FILEMAP ) );
	printf("size of long=%d, sizeof char *=%d\n", sizeof(long), sizeof(char *));
	
	
	int	len = strlen( srcFile );
	filemap->filename = (char *) malloc( len + 1 );
	memset( filemap->filename, 0, len + 1 );
	memcpy( filemap->filename, srcFile, len );

	
	filemap->filelen =  __filesize( srcFile );     // filelen    �ļ�����
        filemap->buflen  = filemap->filelen;            // buflen     ��ͬ���ļ�����, ��ʱû��
        filemap->pos     = 0;                          // pos        λ��ָ��, ��ʼ��Ϊ0

	printfm( filemap );

	
	
	filemap->buf     = (char *) malloc(  filemap->buflen + 1  );   // Ϊ�����������ڴ�ռ�
	printf( "filemap->buflen=%d\n", filemap->buflen );
	memset( filemap->buf, 0, filemap->buflen + 1 ); // ��0

	
	
	
	FILE  *fp = fopen( srcFile, "r" );
	int n = fread( filemap->buf, 1, filemap->buflen, fp );  // ���ļ����ݶ�ȡ���ڴ�ӳ�仺������
	printf("��ȡ���ļ���������:%d\n", n);
	fclose( fp );
	
	
	//printf( "buf����=%s\n", filemap->buf );
	return filemap;

    }	


    //**************************************************************************
    //  �������� : ifOpened()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    void DLL_EXPORT freeFileMap( FILEMAP *filemap ) 
    {
	printf( "��ʼ�ͷ��ڴ�\n");
	if ( filemap != NULL ) {
	    if ( filemap->filename != NULL ) {
		printf("filemap->filename ��Ҫ�ͷ��ڴ�filename=%s!\n", filemap->filename );
	        free( filemap->filename );
		printf("filemap->filename �ͷ��ڴ����\n");
	    }
	    printf("free 1\n");
	    
	    
	    if ( filemap->buf != NULL  )
	        free( filemap->buf );
	    printf("free 2\n");
	    
	    free( filemap );
	    printf("free 3\n");
        } else {
	    printf("NULL, ����free!\n");
	}

	return;
    }

    //**************************************************************************
    //  �������� : readLine()
    //  ��    �� :
    //		�жϸ������ļ��Ƿ��Ѿ���
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    int DLL_EXPORT readLine( FILEMAP * filemap, char * buf, int buflen ) 
    {
	if ( filemap == NULL )          // ����ļ�ӳ��Ϊ�գ��򷵻ؿ�.
            return 0;

	char	ch;
	int 	i = 0;          // �к�
	int	curpos = filemap->pos;   // ������¼�����ļ����ݵ�λ��
	int	len;

        if ( curpos >= filemap->buflen ) {  // ����Ѿ������ļ�ĩβ, �򷵻�0, ��ʾû��������
	    return 0;
	}
        if ( buf == NULL || buflen <= 1 ) {       // ���������Ϊ�գ���û����������, ����0, ���������һ���ֽ�����β0
	    return 0;         
	}
	
	while ( curpos < filemap->buflen ) {
	    curpos ++;                // ��ʼ�µ��ַ��Ķ�ȡ
   	    ch = filemap->buf[curpos];
            if ( ch == 13 || ch == 10 ) {       // �ҵ����лس���   13 = "\0x0D", 10 = "0x0A"
		// ��\0x0A �� \0x0D ֮ǰ������ȡ����, ����һ�е�����
		len = curpos - filemap->pos;    // һ�е��ֽ���
                memset( buf, 0, buflen );
		if ( buflen <= len ) {     // ��������С����Ҫ����Ҫ����������ֽ�����1, ����ֻ�ܻ�ȡbuflen-1����
		    memcpy( buf, filemap->buf, buflen-1 );
		    filemap->pos += buflen-1;
		    return buflen-1;
		} else {
		    memcpy( buf, filemap->buf, len );
		    filemap->pos += len;
	            return len;	    
		}
		
    	    }		
	    
	}

	return len;
    }

    //**************************************************************************
    //  �������� : getLine()
    //  ��    �� :
    //		��ȡָ���е�����, �кŴ�1��ʼ
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    int  DLL_EXPORT  getLine( char * srcFile, int lineNo, char * buf, int buflen )
    {
        FILE   * fp = NULL;
	int	ret;

	if ( lineNo <= 0 ) 
		return -1;       // �Ƿ��к�

	if ( buf == NULL || buflen <= 1 ) 
		return -1;       // ��ָ���޷��洢����

	if ( ! ( fp = fopen( srcFile, "r" ) ) ) {   // ���ļ�ʧ��
		return -1;
	}

	int   	 i = 1;
	char     ch;
 
	while ( !feof( fp ) ) {       // ��ָ������, û�е��ļ���β
	    memset( buf, 0, buflen );
	    fgets( buf, buflen, fp );
            if ( i == lineNo ) {      // �ҵ���
		ret =1;
		break;
	    }
	    i ++;
	}
	
	if ( i < lineNo ) {          // û��ָ����, Ҳ�����ļ��ڵ�������ָ���к�
	    ret = 0;
	}
	
	fclose( fp );
	return ret;
	
    }

    //**************************************************************************
    //  �������� : modifyLine()
    //  ��    �� :
    //		�޸�ָ���е�����
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    /*
    int   DLL_EXPORT modifyLine( srcFile, lineNo, buf )
    {
        FILE   * fp = NULL;
	int	ret;

	if ( lineNo <= 0 ) 
		return -1;       // �Ƿ��к�

	if ( ! ( fp = fopen( srcFile, "w" ) ) ) {   // ���ļ�ʧ��
		return -1;
	}

	int   	 i = 1;
	char     ch;
 
	while ( !feof( fp ) ) {       // ��ָ������, û�е��ļ���β
	    memset( buf, 0, buflen );
	    fgets( buf, buflen, fp );
            if ( i == lineNo ) {      // �ҵ���
		ret =1;
		break;
	    }
	    i ++;
	}
	
	if ( i < lineNo ) {          // û��ָ����, Ҳ�����ļ��ڵ�������ָ���к�
	    ret = 0;
	}
	
	fclose( fp );
	return ret;
	
    }


      */

    // Amemcpy() ��Ϊ�˼򻯴��볤��, ����ʵ���ַ���֮����ڴ濽��
    // ����������ڴ�, �򷵻�������ڴ泤��, ���򷵻�0. 
    // ����ǽṹָ���ڲ���Աָ�룬������ô�����ڴ�, ��Ϊ�������غ�, �ṹָ��û�б仯, Ҳ����������ڴ�ṹָ�벻֪��. ����޷��ͷ�, �����ڴ����!
    int Amemcpy( char * dstr, char * sstr ){
	int     len;

	len = strlen( sstr );
	//if ( !dstr || dstr == NULL ) {      // ����ǿ�ָ���������Ҫ���ڴ�ռ䣬 Ϊʲô�ṹָ�������ڴ��, ��Աָ��Ͳ��ǿ�ָ����????
	    dstr = ( char * ) malloc( len + 1 );
	    printf("\n��ָ����Ҫ�����ڴ�!\n");
	//}
	memset( dstr, 0, len + 1 );
	memcpy( dstr, sstr, len );

	printf("\nlen=%d, dstr=%s, sstr=%s\n",  len, dstr, sstr);

	return len;
    }


    // ============================ ���²��ֶ��ǲ��Դ���, �����������ȹ���  ================
    //
    // 
    /*
    // ��ȡ�ļ���С
    long DLL_EXPORT filesize( char * filename ) 
    {
	    long		ret = 0;
	    long                fcb;
	    asm {
                push    ds
		mov     ax, 0F00h
		lds     dx, filename
		int     21h

		jc      end1

		mov     fcb, dx
                mov     ret, fcb + 10h	
	    end1:
                mov     ah, 10h
                int     21h		

	    }
	    return ret;
    }
    */


    char *  adprintf( void * buf )
    { 
	char *     ret;
	printf("\nbuf=%s\n",buf);
	asm( "movb $0x09, %%ah\n\t"
	     "movl %1, %%ebx\n\t"
            : "d"(ret)
	    : "d" ( (long) buf ) 
	   );
	printf("ret = %s \n", ret);
	return ret;
    }

    int  add1( int x, int y ) 
    {
       
        asm("addl %1, %0"
            : "=r" (y)
            : "r" (x), "0"(y)
	    );
	return y;     
    }
static inline void * my_memcpy(void * to, const void * from, size_t n)
{
int d0, d1, d2;
__asm__ __volatile__(
        "rep ; movsl\n\t"
        "testb $2,%b4\n\t"
        "je 1f\n\t"
        "movsw\n"
        "1:\ttestb $1,%b4\n\t"
        "je 2f\n\t"
        "movsb\n"
        "2:"
        : "=&c" (d0), "=&D" (d1), "=&S" (d2)
        :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
        : "memory");
       printf( "d0=%d, d1=%d, d2=%d\n", d0,d1, d2);

return (to);
}

/*
char* strcpy(char *dst, const char *src)
{
    __asm__("cld\n"
        "1:\tlodsb\n\t"
        "stosb\n\t"
        "testb %%al, %%al\n\t"
        "jne 1b"
        :
        :"S" (src), "D" (dst)
        :"ax");
    return dst;
} 
*/
char* strcpy(char *dst, const char *src)
{
    __asm__("cld\n"
	    "cld\n"	
       	   );
    return dst;

 }

		    /*
    int ad( void ) {
        int a=1, b=2, c=0;  
  
         // ���۵� add ����  
         asm(  
             "addl %2, %0"       // 1  
             : "=g"(c)           // 2  
             : "0"(a), "g"(b)    // 3  
             : "memory");        // 4  
  
         printf("����c��:%d\n", c);  
    }
    */

    /*
    static inline void aprintf( char  * buf )
    {
	    asm( "movl dx,  buf\n\t"
	         "movl ah,  09h\n\t"
		 "int 21h\n\t"
		 "movl ah, 4ch\n\t"
		 "int 21h\n\t");
            return;
    }*/
   

