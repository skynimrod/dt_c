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
    //        1.   IfOpened()
    //        2.   getFileSize()
    //        4.   
    //
    #include    <stdio.h>
    #include    <stdbool.h>                     //  ʹ�� true, false ֵ�Ķ���

    #include    "file_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

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
    int DLL_EXPORT ifOpened( char * filename ) 
    {
	    if ( _access( filename, 0 ) != 0 ) {
		    printf( "file:%s is not exists!\n", filename );
		    return -2;
	    }

	    int    h = sopen( filename, 2, 0 );
	    if ( h == 3 ) {
		    _close (h );
		    return 0;            // # �ļ�û�б��κ��˴�
	    }

	    return 1;          //  # �ļ��Ѿ�������
    }

    //**************************************************************************
    //  �������� : getFileSize()
    //  ��    �� :
    //		��ȡ�ļ���С
    //  ��ڲ��� :
    //            fname : �ļ�����
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
    size_t DLL_EXPORT getFileSize( char *fname ) 
    {
        size_t   len;
        FILE    *fp;

        if ( ! (fp = fopen(fname, "rb" )) )
                return -1;
        fseek( fp, 0,  SEEK_END );
        len = ftell( fp );
        fclose(fp);
        return len;

    }   


