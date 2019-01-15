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

    #include    "file_tools.h"
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
    //                 
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
       
        if ( !srcFile || ifOpened( srcFile ) != -2 )     // ���ļ����򲻴��ڵ��ļ����Ͳ�������
              return NULL;

        fm_p = ( FILEMAP * )malloc( sizeof( FILEMAP ) );
    	printf("Size of FILEMAP=%d\n", sizeof(FILEMAP ) );
	    printf("size of long=%d, sizeof char *=%d\n", sizeof(long), sizeof(char *));
	
	    int 	len = strlen( srcFile );
	    fm_p->filename = (char *) malloc( len + 1 );
	    memset( fm_p->filename, 0, len + 1 );
	    memcpy( fm_p->filename, srcFile, len );

	
	    fm_p->filelen   = getFileSize( srcFile );       // filelen    �ļ�����
        fm_p->buflen    = fm_p->filelen;             // buflen     ��ͬ���ļ�����, ��ʱû��
        fm_p->pos       = 0;                            // pos        λ��ָ��, ��ʼ��Ϊ0

	    print_fm( fm_p );
	    fm_p->stream       = (uchar *) malloc(  fm_p->streamlen + 1  );   // Ϊ�����������ڴ�ռ�
	    printf( "fm_p->streamlen=%d\n", fm_p->streamlen );
	    memset( fm_p->stream, 0, fm_p->streamlen + 1 ); // ��0

	    FILE  *fp = fopen( srcFile, "rb" );
	    size_t n = fread( fm_p->stream, 1, fm_p->streamlen, fp );  // ���ļ����ݶ�ȡ���ڴ�ӳ�仺������
	    printf("��ȡ���ļ���������:%d\n", n);
	    fclose( fp );
	
	    //printf( "buf����=%s\n", fm_p->stream );
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
    		    printf("filemap->filename ��Ҫ�ͷ��ڴ�filename=%s!\n", fm_p->filename );
    	        free( fm_p->filename );
    		    printf("fm_p->filename �ͷ��ڴ����\n");
    	    }
    	    printf("free 1\n");
    	    
    	    
    	    if ( fm_p->stream != NULL  )
    	        free( fm_p->stream );
    	    printf("free 2\n");
    	    
    	    free( fm_p );
    	    printf("free 3\n");
        } else {
    	    printf("NULL, ����free!\n");
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
    /*
    uchar DLL_EXPORT fm_readLine( FILEMAP *fm_p )
    {
        size_t      curpos;         // ��¼�ڴ��ļ��ĵ�ǰλ��

        curpos = self._readBuffer["pos"]

        if ( curpos >= self._readBuffer["buflen"] ):    # �������β�򳬳���βλ�ã� ���ؿ�
            return ""
        
        timeStar = datetime.datetime.now() # �õ���ǰ��ʱ��

        for  i in range( curpos, self._readBuffer["buflen"]) :
            ch = self._readBuffer["buf"][i]
            if ( ch == 13 or ch == 10 ): # �ҵ����лس���   13 = "\0x0D", 10 = "0x0A"
                # �Ȱ�\0A��\0D֮ǰ�����ݻ�ȡ����
                tmp = self._readBuffer["buf"][curpos:i]
                # retbuf = tmp.decode('utf-8')   # ��������н������, ֻ���漰�����ݴ����ʱ�����Ҫ����
                retbuf = tmp
                # �������������ң� �����\0D �� \0A ��������
                          
                curpos = i      # curpos  ��ֵΪ��ǰ��λ��
                ch = self._readBuffer["buf"][curpos]

                while ( ch == 13 or ch == 10 ) :  # �������� ����Ļس�����
                    curpos += 1
                    ch = self._readBuffer["buf"][curpos]
                
                self._readBuffer["pos"] = curpos

                break
            
            
        
        timeEnd = datetime.datetime.now()  # �õ���ǰ��ʱ��  

        #if ( (timeEnd - timeStar) > 0 ) log.info "readLine() time :" + (timeEnd - timeStar) + "ms"
        return retbuf
        
    }
    */

