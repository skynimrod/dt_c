    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    Alog
    //
    // Description : about log operation.
    //       日志操作接口. 
    //            
    // Interface :
    //        1.   wlog()
    //        2.   lwlog()
    //        3.   wlogEx()
    //        4.   
    //
    
    #include    <stdio.h>
    #include    <stdbool.h>                     //  使用 true, false 值的定义
    #include    <io.h>                          //  _sopen()
    #include    <sys\stat.h>                    // 用来获取文件大小
    #include    <time.h>

    #include    "file_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    static inline void aprintf( char  * buf );
    char* strcpy(char *dst, const char *src);
    int  add1( int x, int y ); 
static inline void * my_memcpy(void * to, const void * from, size_t n);
    char *  adprintf( void * buf );
    long DLL_EXPORT __filesize( char * filename );

    /*
     * _access() 如果能以指定模式成功打开文件, 则返回0, 否则返回-1
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
		    return 0;            // # 文件没有被任何人打开
	    }

	    return 1;          //  # 文件已经被打开了
	    /*
	    // check for existence
	    if ( ( _access( filename, 0 ) ) == 0 ) {
		    printf( "file:%s is exists!\n", filename );

		    // check for write permission
		    if ( _access( filename, 2 ) == 0 ) {
			    printf( "file: %s has write permission, can write!", filename );
			    return false;           // 表示文件没有被打开
		    }
                    printf( "file: %s has not write permission!", filename );
		    return true;                     //  没有写权限, 表示文件已经被打开了.
	    }
	    */
    }

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

	
	filemap->filelen =  __filesize( srcFile );     // filelen    文件长度
        filemap->buflen  = filemap->filelen;            // buflen     等同于文件长度, 暂时没用
        filemap->pos     = 0;                          // pos        位置指针, 初始化为0

	printfm( filemap );

	
	
	filemap->buf     = (char *) malloc(  filemap->buflen + 1  );   // 为缓冲区申请内存空间
	printf( "filemap->buflen=%d\n", filemap->buflen );
	memset( filemap->buf, 0, filemap->buflen + 1 ); // 置0

	
	
	
	FILE  *fp = fopen( srcFile, "r" );
	int n = fread( filemap->buf, 1, filemap->buflen, fp );  // 把文件内容读取到内存映射缓冲区中
	printf("读取的文件内容数量:%d\n", n);
	fclose( fp );
	
	
	//printf( "buf内容=%s\n", filemap->buf );
	return filemap;

    }	


    void DLL_EXPORT freeFileMap( FILEMAP *filemap ) 
    {
	printf( "开始释放内存\n");
	if ( filemap != NULL ) {
	    if ( filemap->filename != NULL ) {
		printf("filemap->filename 需要释放内存filename=%s!\n", filemap->filename );
	        free( filemap->filename );
		printf("filemap->filename 释放内存结束\n");
	    }
	    printf("free 1\n");
	    
	    
	    if ( filemap->buf != NULL  )
	        free( filemap->buf );
	    printf("free 2\n");
	    
	    free( filemap );
	    printf("free 3\n");
        } else {
	    printf("NULL, 不用free!\n");
	}

	return;
    }

    void *readLine( FILEMAP * filemap ) 
    {
	    return NULL;
    }

    // Amemcpy() 是为了简化代码长度, 用来实现字符串之间的内存拷贝
    // 如果申请了内存, 则返回申请的内存长度, 否则返回0. 
    // 如果是结构指针内部指针，不能这么申请内存, 因为函数返回后, 结构指针没有变化, 也就是申请的内存结构指针不知道. 造成无法释放, 出现内存错误!
    int Amemcpy( char * dstr, char * sstr ){
	int     len;

	len = strlen( sstr );
	//if ( !dstr || dstr == NULL ) {      // 如果是空指针就申请需要的内存空间， 为什么结构指针申请内存后, 成员指针就不是空指针了????
	    dstr = ( char * ) malloc( len + 1 );
	    printf("\n空指针需要申请内存!\n");
	//}
	memset( dstr, 0, len + 1 );
	memcpy( dstr, sstr, len );

	printf("\nlen=%d, dstr=%s, sstr=%s\n",  len, dstr, sstr);

	return len;
    }
    // ============================ 以下部分都是测试代码, 测试内联汇编等功能  ================
    //
    // 
    /*
    // 获取文件大小
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
  
         // 蛋疼的 add 操作  
         asm(  
             "addl %2, %0"       // 1  
             : "=g"(c)           // 2  
             : "0"(a), "g"(b)    // 3  
             : "memory");        // 4  
  
         printf("现在c是:%d\n", c);  
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
   

