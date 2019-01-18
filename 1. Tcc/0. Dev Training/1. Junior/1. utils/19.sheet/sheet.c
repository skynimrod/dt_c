    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    filemap
    //
    // Description : about filemap operation.
    //       �漰  file_tools.h
    // Interface :
    //      0.  printf_fm()       ��ʾ��ǰ fm ������, ������
    //      1.  initReadFileMap()
    //      2.  freeFileMap()  
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  ʹ�� true, false ֵ�Ķ���

    #include    "file_tools.h"                  // getFileSize()
    #include    "filemap.h"

    #define DLL_EXPORT __declspec(dllexport)

    //**************************************************************************
    //  �������� : print_fm()
    //  ��    �� :
    //		��ʾ  filemap ָ���Ӧ�����ݣ� ���ڵ��ԡ�
    //  ��ڲ��� :
    //            fm_p : FILEMAP * ָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			            
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    void DLL_EXPORT print_fm( FILEMAP * fm_p )
    {
	    if ( fm_p == NULL ) {
		    printf("filemap is  NULL \n");
	        return;
	    }
            printf("filemap->filename=%s\n", fm_p->filename );
            printf("filemap->filelen=%d\n", fm_p->filelen );
            printf("filemap->pos=%d\n", fm_p->pos );

	    return;
    }

    //**************************************************************************
    //  �������� : initReadFileMap()
    //  ��    �� :
    //		��ʼ�� FILEMAP ָ�룬��Ҫ�������ڴ�, �����ļ����ݰ��ֽ�����ʽд�� �ڴ���.
    //  ��ڲ��� :
    //            srcFile :  �ַ����� �ļ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           FILEMAP ָ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ����ú�����, һ��Ҫע���ͷŷ���ָ���е��ڴ�, ��Ҫ����freeFileMap()
    //	Standard .h  :
    //          file_tools.h  /  file_tools.dll  ------------- getFileSize()          
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    FILEMAP * DLL_EXPORT initReadFileMap( char * srcFile )
    {
	    FILEMAP     * fm_p;
       
        if ( !srcFile || ifOpened( srcFile ) == -2 )    {// ���ļ����򲻴��ڵ��ļ����Ͳ�������
            //printf("��Ч�ļ���!\n");
            return NULL;
        }

        fm_p = ( FILEMAP * )malloc( sizeof( FILEMAP ) );
    	//printf("Size of FILEMAP=%d\n", sizeof(FILEMAP ) );
	    //printf("size of long=%d, sizeof char *=%d\n", sizeof(long), sizeof(char *));
	
	    int 	len = strlen( srcFile );

	    fm_p->filename = (char *) malloc( len + 1 );
	    memset( fm_p->filename, 0, len + 1 );
	    memcpy( fm_p->filename, srcFile, len );

	
	    fm_p->filelen   = getFileSize( srcFile );       // filelen    �ļ�����
        fm_p->streamlen = fm_p->filelen;                // buflen     ��ͬ���ļ�����, ��ʱû��
        fm_p->pos       = 0;                            // pos        λ��ָ��, ��ʼ��Ϊ0

	    print_fm( fm_p );
	    fm_p->stream       = (uchar *) malloc(  fm_p->streamlen + 1  );   // Ϊ�����������ڴ�ռ�
	    //printf( "fm_p->streamlen=%d\n", fm_p->streamlen );
	    memset( fm_p->stream, 0, fm_p->streamlen + 1 ); // ��0

	    FILE  *fp = fopen( srcFile, "rb" );
	    long n = fread( fm_p->stream, 1, fm_p->streamlen, fp );  // ���ļ����ݶ�ȡ���ڴ�ӳ�仺������
	    //printf("��ȡ���ļ���������:%d\n", n);
	    fclose( fp );

	    //printf( "stream����=%s\n", fm_p->stream );
	    return fm_p;
    }

    //**************************************************************************
    //  �������� : freeFileMap()
    //  ��    �� :
    //		�ͷ� FILEMAP ָ�룬��Ҫ���ͷ��ڴ�
    //  ��ڲ��� :
    //            fm_p : FILEMAPָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ����FILEMAP��, һ��Ҫע���ͷ��ڴ�, ��Ҫ����freeFileMap()
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    void DLL_EXPORT freeFileMap( FILEMAP * fm_p ) 
    {
    	if ( fm_p != NULL ) {
    	    if ( fm_p->filename != NULL ) {
    		    //printf("filemap->filename ��Ҫ�ͷ��ڴ�filename=%s!\n", fm_p->filename );
    	        free( fm_p->filename );
    		    //printf("fm_p->filename �ͷ��ڴ����\n");
    	    }
    	    //printf("free 1\n");
    	    
    	    
    	    if ( fm_p->stream != NULL  )
    	        free( fm_p->stream );
    	    //printf("free 2\n");
    	    
    	    free( fm_p );
    	    //printf("free 3\n");
        } else {
    	    ;
            //printf("NULL, ����free!\n");
    	}
    
    	return;
    }

    // ��ȡ�ڴ��ļ������ԣ�ֱ�ӷ���FILEMAP �ĵĳ�Ա��������
    //  1. �ļ�����   2. �ļ�����    3. 
    //
    
    //�ڴ�ӳ���ļ��Ĳ�������, ��ȡ��ǰ�����ļ���һ��, ���Զ��ƶ�ָ��
    //**************************************************************************
    //  �������� : fm_readLine()
    //  ��    �� :
    //		�����ڴ��ļ��� ��ȡ ��ǰλ�ÿ�ʼ��һ��, �Իس�����Ϊ������
    //  ��ڲ��� :
    //            fm_p : FILEMAPָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           uchar  *
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ����FILEMAP��, һ��Ҫע���ͷ��ڴ�, ��Ҫ����freeFileMap()
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    uchar * DLL_EXPORT fm_readLine( FILEMAP *fm_p )
    {
        long        curpos;         // ��¼�ڴ��ļ��ĵ�ǰλ��
        uchar       ch;
        uchar   *   retbuf = NULL;
        
        if ( fm_p->pos < 0 )               // �ݴ���, �����ǰλ��Ϊ������ ������Ϊͷ��0
            fm_p->pos = 0;

        curpos = fm_p->pos;

        if ( curpos >= fm_p->streamlen )    // �������β�򳬳���βλ�ã� ���ؿ�
            return NULL;

        for ( long i = curpos; i <= fm_p->streamlen; i ++ ) {
            ch = fm_p->stream[i];
            
            if ( ch == 13 || ch == 10 ) {  //  �ҵ����лس���   13 = "\0x0D", 10 = "0x0A"
                // �Ȱ�\0A��\0D֮ǰ�����ݻ�ȡ����
                retbuf = ( uchar * ) malloc( i-curpos + 1 );
                memset( retbuf, 0, i-curpos+1 );

                memcpy( retbuf, &fm_p->stream[curpos], i-curpos );

                // �������������ң� �����\0D �� \0A ��������
                          
                curpos = i;      // curpos  ��ֵΪ��ǰ��λ��
                ch = fm_p->stream[curpos];

                while ( ch == 13 || ch == 10 ) {     // �������� ����Ļس�����
                    curpos += 1;
                    ch = fm_p->stream[curpos];
                }
                fm_p->pos = curpos;
                break;
            }
        }
        return retbuf;
        
    }

    //**************************************************************************
    //  �������� : fm_read()
    //  ��    �� :
    //		�����ڴ��ļ��� ��ȡ ��ǰλ�ÿ�ʼָ�����ȵ��ֽ���
    //  ��ڲ��� :
    //            fm_p : FILEMAPָ��
    //            length :  ��Ҫ��ȡ���ֽ�������
    //            start  :  ��ʼλ��ȱʡΪ-1, ��ʾ�ӵ�ǰλ�ö�ȡ, ������ָ����λ��
    //  ���ڲ��� :
    //           ��
    //  �� �� ֵ :
    //           uchar  *   ��ȡ���ֽ���
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.15
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ���ص�ָ���п����������ڴ�ģ� ʹ�����Ҫ�ͷ��ڴ�
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    uchar * DLL_EXPORT fm_read( FILEMAP *fm_p, long length, long start )
    {
        long    buflen, rlen, rpos;
        uchar  *desbuf;

        buflen  = fm_p->streamlen;
        rlen    = length;

        //printf(  "start=%d, len=%d, buflen=%d\n", start, length, buflen  );        

        if ( start > buflen )           //       ���Խ��ͷ���NULL
                return NULL;

        //��ʼλ�õĴ���
        if ( start == -1 )
            rpos = fm_p->pos; 
        else 
            rpos = fm_p->pos = start;
            
        // ʵ�ʶ�ȡ���ȵĴ���
        // printf( "start=%d(%08x), len=%d, buflen=%d" , rpos, rpos, length, buflen );
        if ( ( rpos + length ) > buflen )
            rlen = buflen - start;

        desbuf = ( uchar * )malloc( rlen + 1 );
        memset( desbuf, 0, rlen + 1 );
        memcpy( desbuf, &fm_p->stream[rpos], rlen );

        fm_p->pos = rpos + rlen;                //  ��ȡ���޸ĵ�ǰ��λ��

        return desbuf;
    }

    //**************************************************************************
    //  �������� : fm_seek()
    //  ��    �� :
    //		���ļ������Ķ�λ���ܣ� ��������Ķ�λ�ܼ򵥣������޸Ļ������е��α�λ��pos
    //      ����һЩ���࣬ ��λ����Ǹ����� ���0��ʼ (���δ�����ܻ�ı䣬���������ԴӺ���ǰ��)
    //      ������ȴ��ڻ���ȥ����(�ļ�����)��Ҳ���Ƕ�λ�������. 
    //         ��������������ʾ�Ǿ���λ�á�
    //  ��ڲ��� :
    //            fm_p : FILEMAPָ��
    //            pos  : ��ָ����λ��
    //  ���ڲ��� :
    //           ��
    //  �� �� ֵ :
    //           uchar  *   ��ȡ���ֽ���
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ���ص�ָ���п����������ڴ�ģ� ʹ�����Ҫ�ͷ��ڴ�
    //	Standard .h  :
    //                 
    //	Develope Tools:
    //			  TCC   
    //			  
    //	Environment:
    //			1. Win7
    //
    //***************************************************************************
    //
    void DLL_EXPORT fm_seek( FILEMAP  * fm_p, long pos )
    {
        long      rpos = pos;

        //printf("\nָ����λ��Ϊ%d\n", pos);
        
        if ( pos < 0 ) {                        // ָ��λ��Ϊ��������ʾ����ָ�����ֽ���
            //printf("\nf�����ܴ�����?\n");
            rpos = fm_p->pos + pos;             // ��Ϊ�Ǹ����� ������Ӳ��ܻ���
            if ( rpos < 0 )                     //  ��ʾ���˷�Χ������ͷ���� ����Ϊ0
                rpos = 0;
        }
        
        if ( pos >= fm_p->streamlen )           // pos �ǷǸ���, ���ʾ����λ��, Ҫ�ж��Ƿ񳬳�β��, ��������Ϊβ��
            rpos = fm_p->streamlen - 1;          // �±��Ǵ�0��ʼ�ģ� ����Ҫ��1
        
        fm_p->pos = rpos;                       // ����λ��
    }

