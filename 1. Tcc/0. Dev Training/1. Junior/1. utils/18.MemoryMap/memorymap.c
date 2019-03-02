    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    memorymap
    //
    // Description : about memorymap operation.
    //       �漰  file_tools.h
    // Interface :
    //      0.  printf_mm()       ��ʾ��ǰ mm ������, ������
    //      1.  initMemoryMap()
    //      2.  freeMemoryMap()  
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  ʹ�� true, false ֵ�Ķ���

    #include    "memorymap.h"                  

    #define DLL_EXPORT __declspec(dllexport)

    //**************************************************************************
    //  �������� : print_mm()
    //  ��    �� :
    //		��ʾ  MemoryMap ָ���Ӧ�����ݣ� ���ڵ��ԡ�
    //  ��ڲ��� :
    //            mm_p : MEMORYMAP * ָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
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
    void DLL_EXPORT print_mm( MEMORYMAP * mm_p )
    {
	    if ( mm_p == NULL ) {
		    printf("memorymap is  NULL \n");
	        return;
	    }
            printf("mm_p->stream address=%p\n", mm_p->stream );
            printf("mm_p->streamlen=%d\n", mm_p->streamlen );
            printf("mm_p->pos=%d\n", mm_p->pos );

	    return;
    }

    //**************************************************************************
    //  �������� : initMemoryMap()
    //  ��    �� :
    //		��ʼ�� MemoryMap ָ�룬��Ҫ�������ڴ�, �����ļ����ݰ��ֽ�����ʽд�� �ڴ���.
    //  ��ڲ��� :
    //            stream : �ֽ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           MEMORYMAP ָ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ����ú�����, һ��Ҫע���ͷŷ���ָ���е��ڴ�, ��Ҫ����freeMemoryMap()
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
    MEMORYMAP * DLL_EXPORT initMemoryMap( uchar * stream, long   len )
    {
	    MEMORYMAP     * mm_p;
       
        if ( !stream )    { // �ֽ���Ϊ�����ô���
            //printf("��Ч�ֽ���!\n");
            return NULL;
        }

        mm_p = ( MEMORYMAP * )calloc( sizeof( MEMORYMAP ), 1 );
    	//printf("Size of MEMORYMAP=%d\n", sizeof(MEMORYMAP ) );
	
        mm_p->streamlen = len;                          //�ֽ�������
        mm_p->pos       = 0;                            // pos        λ��ָ��, ��ʼ��Ϊ0

	    //print_mm( mm_p );
	    mm_p->stream       = (uchar *) calloc(  mm_p->streamlen + 1  );   // Ϊ�����������ڴ�ռ�
	    //printf( "mm_p->streamlen=%d\n", mm_p->streamlen );

	    memset( mm_p->stream, 0, mm_p->streamlen + 1 ); // ��0
        memcpy( mm_p->stream, stream, len );

	    //printf( "stream����=%s\n", fm_p->stream );
	    return mm_p;
    }

    //**************************************************************************
    //  �������� : freeMemoryMap()
    //  ��    �� :
    //		�ͷ� MEMORYMAP ָ�룬��Ҫ���ͷ��ڴ�
    //  ��ڲ��� :
    //            mm_p : MEMORYMAPָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           ��
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //          ע��, ����MEMORYMAP��, һ��Ҫע���ͷ��ڴ�, ��Ҫ����freeMemoryMap()
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
    void DLL_EXPORT freeMemoryMap( MEMORYMAP * mm_p ) 
    {
    	if ( mm_p != NULL ) {
    	    if ( mm_p->stream != NULL  )
    	        free( mm_p->stream );
    	    //printf("free 1\n");
    	    
    	    free( mm_p );
    	    //printf("free 2\n");
        } else {
    	    //printf("NULL, ����free!\n");
    	}
    
    	return;
    }

    // ��ȡ�ڴ��ļ������ԣ�ֱ�ӷ���FILEMAP �ĵĳ�Ա��������
    //  1. �ļ�����   2. �ļ�����    3. 
    //
    
    //�ڴ�ӳ���ļ��Ĳ�������, ��ȡ��ǰ�����ļ���һ��, ���Զ��ƶ�ָ��
    //**************************************************************************
    //  �������� : mm_readLine()
    //  ��    �� :
    //		�����ڴ��ļ��� ��ȡ ��ǰλ�ÿ�ʼ��һ��, �Իس�����Ϊ������
    //  ��ڲ��� :
    //            mm_p : MEMORYMAPָ��
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //           uchar  *
    //  Author :
    //            Adams
    //  Create Date :
    //            2019.1.16
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
    uchar * DLL_EXPORT mm_readLine( MEMORYMAP *mm_p )
    {
        long        curpos;         // ��¼�ڴ��ļ��ĵ�ǰλ��
        uchar       ch;
        uchar   *   retbuf = NULL;
        
        if (  mm_p->pos < 0 )               // �ݴ���, �����ǰλ��Ϊ������ ������Ϊͷ��0
            mm_p->pos = 0;

        curpos = mm_p->pos;

        if ( curpos >= mm_p->streamlen )    // �������β�򳬳���βλ�ã� ���ؿ�
            return NULL;

        for ( long i = curpos; i < mm_p->streamlen; i ++ ) {
            ch = mm_p->stream[i];
            
            if ( ch == 13 || ch == 10 ) {  //  �ҵ����лس���   13 = "\0x0D", 10 = "0x0A"
                // �Ȱ�\0A��\0D֮ǰ�����ݻ�ȡ����
                retbuf = ( uchar * ) malloc( i-curpos + 1 );
                memset( retbuf, 0, i-curpos+1 );

                memcpy( retbuf, &mm_p->stream[curpos], i-curpos );

                // �������������ң� �����\0D �� \0A ��������
                          
                curpos = i;      // curpos  ��ֵΪ��ǰ��λ��
                ch = mm_p->stream[curpos];

                while ( ( ch == 13 || ch == 10 ) && ( curpos < mm_p->streamlen ) ) {     // �������� ����Ļس�����
                    curpos += 1;
                    ch = mm_p->stream[curpos];
                }
                mm_p->pos = curpos;
                return retbuf;
            }
        }
        return retbuf;
        
    }


    //**************************************************************************
    //  �������� : mm_read()
    //  ��    �� :
    //		�����ڴ�ӳ�䣬 ��ȡ ��ǰλ�ÿ�ʼָ�����ȵ��ֽ���
    //  ��ڲ��� :
    //            mm_p :    MEMORYMAPָ��
    //            length :  ��Ҫ��ȡ���ֽ�������
    //            start  :  ��ʼλ��ȱʡΪ-1, ��ʾ�ӵ�ǰλ�ö�ȡ, ������ָ����λ��
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
    uchar * DLL_EXPORT mm_read( MEMORYMAP *mm_p, long length, long start )
    {
        long  buflen, rlen, rpos;
        uchar  *desbuf;

        buflen  = mm_p->streamlen;
        rlen    = length;

        //printf(  "start=%d, len=%d, buflen=%d\n", start, length, buflen  );        

        if ( start > buflen )           //       ���Խ��ͷ���NULL
                return NULL;

        //��ʼλ�õĴ���
        if ( start == -1 )
            rpos = mm_p->pos; 
        else 
            rpos = mm_p->pos = start;
            
        // ʵ�ʶ�ȡ���ȵĴ���
        // printf( "start=%d(%08x), len=%d, buflen=%d" , rpos, rpos, length, buflen );
        if ( ( rpos + length ) > buflen )
            rlen = buflen - start;

        desbuf = ( uchar * )malloc( rlen + 1 );
        memset( desbuf, 0, rlen + 1 );
        memcpy( desbuf, &mm_p->stream[rpos], rlen );

        mm_p->pos = rpos + rlen;                //  ��ȡ���޸ĵ�ǰ��λ��

        return desbuf;
    }


    //**************************************************************************
    //  �������� : mm_seek()
    //  ��    �� :
    //		���ļ������Ķ�λ���ܣ� ��������Ķ�λ�ܼ򵥣������޸Ļ������е��α�λ��pos
    //      ����һЩ���࣬ ��λ����Ǹ����� ���0��ʼ (���δ�����ܻ�ı䣬���������ԴӺ���ǰ��)
    //      ������ȴ��ڻ���ȥ����(�ļ�����)��Ҳ���Ƕ�λ�������. 
    //         ��������������ʾ�Ǿ���λ�á�
    //  ��ڲ��� :
    //            mm_p : MEMORYMAPָ��
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
    void DLL_EXPORT mm_seek( MEMORYMAP  * mm_p, long pos )
    {
        long      rpos = pos;

        //printf("\nָ����λ��Ϊ%d\n", pos);
        
        if ( pos < 0 ) {                        // ָ��λ��Ϊ��������ʾ����ָ�����ֽ���
            rpos = mm_p->pos + pos;             // ��Ϊ�Ǹ���, ������Ӳ��ܻ���
            if ( rpos < 0 )                     //  ��ʾ���˷�Χ������ͷ���� ����Ϊ0
                rpos = 0;
        }
        
        if ( pos >= mm_p->streamlen )           // pos �ǷǸ���, ���ʾ����λ��, Ҫ�ж��Ƿ񳬳�β��, ��������Ϊβ��
            rpos = mm_p->streamlen - 1;          // �±��Ǵ�0��ʼ�ģ� ����Ҫ��1
        
        mm_p->pos = rpos;                       // ����λ��
    }

