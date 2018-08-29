    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    exercise_math
    //
    // Description : about exercise math generation operation.
    //       �μ�: 
    //       
    // Interface :
    //        1.   ()
    //        2.   ()
    //        3.   ()
    //        4.   
    //
    #include    <stdio.h>
    #include    <string.h>
    #include    <time.h>

    #include    "time_tools.h"
    #include    "exercise.h"

    #define DLL_EXPORT __declspec(dllexport)

    //**************************************************************************
    //  �������� : initRand()
    //  ��    �� :
    //		ͨ���㷨����ʼ�������, ��ֹ�������� ������� ��ʱ�� �������������ͬ.
    //  ��ڲ��� :
    //      .
    //
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         void
    //  Author :
    //            Adams
    //  Create Date :
    //            2018.04.28
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		ͨ���㷨����ʼ�������, ȷ��������������rand()��ʱ��, ���������������ͬ.         
    //	Standard .h  :
    //	        stdlib.h  ---- malloc(),
    //	        stdio.h   ---- printf();
    //          string.h  ---- strlen(), memset(),  memcpy(), strtok()
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    void initRand()
    {
        Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	    end = clock();
       
	    //srand( limit - GetCurMilSecond()%limit );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

    }

    //**************************************************************************
    //  �������� : addlimited()
    //  ��    �� :
    //		���������ӷ�, ����20���ڼӷ���.
    //  ��ڲ��� :
    //      .
    //
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //          char   * :  һ���ӷ���ʽ
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		�൱�����ݿ��ṹ.         
    //	Standard .h  :
    //	        stdlib.h  ---- malloc(),
    //	        stdio.h   ---- printf();
    //          string.h  ---- strlen(), memset(),  memcpy(), strtok()
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    char * DLL_EXPORT addlimited ( int limit )       // ���Ʒ�Χ�ڵļӷ���, ����: 20���ڵļӷ�, start ���ⲿ�����ʱ�����, ������������ĳ�ʼ����
    { 
	    int     n1, n2;       // n1 �ű�����1,  n2 �� ����2
        char    buf[512];                   // ���������ʽ
        time_t  end;

        Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	    end = clock();
       
	    //srand( limit - GetCurMilSecond()%limit );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

        while( 1 ) {
            n1 =  rand() % limit;           // �������������Ʒ�Χ�ļ���
            if ( n1 != 0 && n1 != 1 )
                break;
        }

        while( 1 ) {
            Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	        end = clock();
	        //srand(  limit - GetCurMilSecond()%limit  );
            srand(  GetCurMilSecond() );
            srand( limit - rand() % limit );

            while( 1 ) {
                n2 =  rand() % limit;           // �������������Ʒ�Χ�ļ���
                if ( n2 != 0 && n2 != 1 )
                    break;
            }

            if ( n1 > limit /2 || n2 > limit /2  ) 
                break;
        }

        memset( buf, 0, 512 );
        sprintf( buf, "%2d + %2d = ", n1, n2 );    // ͨ�����ᳬ��12���ַ�,  �����Ļ���һ�а���80���ַ���Ļ�, ��5����ʽû����, ÿ����ʽ֮����4���ַ���
        
	    return buf;
    }

    // ������ʽ
    char * DLL_EXPORT sublimited ( int limit )       // ���Ʒ�Χ�ڵļ�������, ����: 20���ڵļӷ�, start ���ⲿ�����ʱ�����, ������������ĳ�ʼ����
    { 
	    int     n1, n2, n3;       // n1 �ű�����,  n2 �� ����,  n3 ��������n1,n2 ��С������.
        char    buf[512];                   // ���������ʽ
        time_t  end;

        Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	    end = clock();
	    //srand(  limit - GetCurMilSecond()%limit  );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

        while( 1 ) {
            n1 =  rand() % limit;           // �������������Ʒ�Χ�ı�����
            if ( n1 != 0 && n1 != 1 )
                break;
        }

        while( 1 ) {
            Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	        end = clock();
	        //srand(  limit - GetCurMilSecond()%limit  );
            srand(  GetCurMilSecond() );
            srand( limit - rand() % limit );
            
            while( 1 ) {
                n2 =  rand() % limit;           // �������������Ʒ�Χ�ļ���
                if ( n2 != 0 && n2 != 1 && n2 != n1 )    // ���� ������0��1 �����뱻������ͬ��
                    break;
            }

            if ( n1 > limit /2 || n2 > limit /2  ) 
                break;
        }

        if ( n1 < n2 ) {
            n3 = n2;   n2 = n1; n1 = n3;     
        }

        memset( buf, 0, 512 );
        sprintf( buf, "%2d - %2d = ", n1, n2 );    // ͨ�����ᳬ��12���ַ�,  �����Ļ���һ�а���80���ַ���Ļ�, ��5����ʽû����, ÿ����ʽ֮����4���ַ�
        
	    return buf;
    }

    // ����ָ��������������ʽ
    int DLL_EXPORT genEquation( int limit, int row, int col, char * title, char * filename, int No ) 
    {
        FILE * fp;
        char   buffs[1024], lbuf[16];
        int    lines = 0;

        fp = fopen( filename, "a+" );
        if ( fp == NULL ) 
            return;
        
        fprintf( fp, "\n%d.%s\n", No, title );
        lines += 2;    // ��1,2��
        
        for ( int i =0; i < 80; i ++) 
            fprintf( fp, "-" );
        fprintf( fp, "\n\n" );

        lines += 2;   // ��3,4��
        
        memset( buffs, 0, 1024 );
        memset( lbuf, 0, 16 );
        srand( limit );

        int count = 0;
        for (int i = 0; i < row; i ++ ) {
            for ( int j = 0; j < col ; j ++ ) {
                while( 1 )  {

                    // ���������, ����ǵ������üӷ�, ˫���ü���        
                    if ( rand() % 2 == 1 ) { // ����, �üӷ�

                       // printf("--�ӷ�--\n");
                        sprintf( lbuf, "%s", addlimited( limit ) );
                    } else {
                        //printf("--����---\n");
                        sprintf( lbuf, "%s", sublimited( limit ) );
                    }

                    //printf("lbuf=%s,\n buffs=%s\nstrstr()=%s\n", lbuf, buffs, strstr(buffs, lbuf) );
                    if ( ! strstr( buffs, lbuf )  ) {   // ��� lbuf ����bufs �г���, ���ʾ����ʽ���µ�, �����ʾ�Ѿ��и���ʽ��, ��Ҫ������.
                        fprintf( fp, "%s", lbuf );
                        strcat(buffs, lbuf);      // ������ʽƴ�ӵ�������.
                        memset( lbuf, 0, 16 );
                        count ++;
                        //printf("%d,-----------�ҵ���һ����ʽ====!\n", count);
                        break;
                    }
                }

                fprintf( fp, "     " );

            }
            fprintf( fp, "\n\n" );
            lines += 2;
        }

        fprintf( fp, "\n\n" );
        lines += 2;
        fclose(fp);

        return lines;
    }    
 
    // �������������Ķ��յ���ϰ��ʽ
    void DLL_EXPORT getDaysExercise( int maxLines, int limit, char *title, int rows, int cols, char * filename, int days )
    {
        int     lines = 0, l ;
        FILE  * fp;
 
        for ( int i = 0; i < days; i ++ ) {
            lines = genEquation( limit, rows, cols, title, filename, i ); 
            l = lines;
            printf("%d..", i);

            while ( (lines + l) < maxLines && i < days ) {   // �����ǰ����û�г���һҳ������, ���ж�����һ�����ܷ񳬹�
                i ++;
                lines += genEquation( limit, rows, cols, title, filename, i ); 
                printf("%d..", i);
            }

            if ( lines < maxLines ) {  // ���û����һҳ, �������
                if ( fp = fopen( filename, "a+" ) ) {
                     int j = maxLines- lines;
                     for ( int k = 0; k < j; k ++ ) {
                         fprintf( fp, "%d\n", k );
                     } 
                }
                fclose( fp );
            }
        
            lines = 0;

        }
   }

   /*
    * �Ӽ��������: AddSubMixedOperation()
    * ����˵��:
    *     
    */
   void AddSubMixedOperation( int limit, char *opers,  int flag )
   {
   }

