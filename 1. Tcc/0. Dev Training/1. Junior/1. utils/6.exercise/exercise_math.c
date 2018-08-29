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
    //       参见: 
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
    //  函数名称 : initRand()
    //  功    能 :
    //		通过算法来初始化随机数, 防止连续调用 随机函数 的时候 产生的随机数相同.
    //  入口参数 :
    //      .
    //
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         void
    //  Author :
    //            Adams
    //  Create Date :
    //            2018.04.28
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		通过算法来初始化随机数, 确保后面连续调用rand()的时候, 产生的随机数不相同.         
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
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    void initRand()
    {
        Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	    end = clock();
       
	    //srand( limit - GetCurMilSecond()%limit );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

    }

    //**************************************************************************
    //  函数名称 : addlimited()
    //  功    能 :
    //		限制条件加法, 比如20以内加法等.
    //  入口参数 :
    //      .
    //
    //  出口参数 :
    //            无
    //  返 回 值 :
    //          char   * :  一个加法算式
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		相当于数据库表结构.         
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
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    char * DLL_EXPORT addlimited ( int limit )       // 限制范围内的加法题, 例如: 20以内的加法, start 是外部传入的时间参数, 用来做随机数的初始化的
    { 
	    int     n1, n2;       // n1 放被加数1,  n2 放 加数2
        char    buf[512];                   // 用来存放算式
        time_t  end;

        Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	    end = clock();
       
	    //srand( limit - GetCurMilSecond()%limit );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

        while( 1 ) {
            n1 =  rand() % limit;           // 产生不超过限制范围的加数
            if ( n1 != 0 && n1 != 1 )
                break;
        }

        while( 1 ) {
            Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	        end = clock();
	        //srand(  limit - GetCurMilSecond()%limit  );
            srand(  GetCurMilSecond() );
            srand( limit - rand() % limit );

            while( 1 ) {
                n2 =  rand() % limit;           // 产生不超过限制范围的加数
                if ( n2 != 0 && n2 != 1 )
                    break;
            }

            if ( n1 > limit /2 || n2 > limit /2  ) 
                break;
        }

        memset( buf, 0, 512 );
        sprintf( buf, "%2d + %2d = ", n1, n2 );    // 通常不会超过12个字符,  这样的话，一行按照80个字符算的话, 列5个算式没问题, 每个算式之间间隔4个字符发
        
	    return buf;
    }

    // 减法算式
    char * DLL_EXPORT sublimited ( int limit )       // 限制范围内的减法法题, 例如: 20以内的加法, start 是外部传入的时间参数, 用来做随机数的初始化的
    { 
	    int     n1, n2, n3;       // n1 放被减数,  n2 放 减数,  n3 用来根据n1,n2 大小来调整.
        char    buf[512];                   // 用来存放算式
        time_t  end;

        Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	    end = clock();
	    //srand(  limit - GetCurMilSecond()%limit  );
        srand(  GetCurMilSecond() );
        srand( limit - rand() % limit );

        while( 1 ) {
            n1 =  rand() % limit;           // 产生不超过限制范围的被减数
            if ( n1 != 0 && n1 != 1 )
                break;
        }

        while( 1 ) {
            Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	        end = clock();
	        //srand(  limit - GetCurMilSecond()%limit  );
            srand(  GetCurMilSecond() );
            srand( limit - rand() % limit );
            
            while( 1 ) {
                n2 =  rand() % limit;           // 产生不超过限制范围的减数
                if ( n2 != 0 && n2 != 1 && n2 != n1 )    // 丢弃 减数是0，1 或者与被减数相同的
                    break;
            }

            if ( n1 > limit /2 || n2 > limit /2  ) 
                break;
        }

        if ( n1 < n2 ) {
            n3 = n2;   n2 = n1; n1 = n3;     
        }

        memset( buf, 0, 512 );
        sprintf( buf, "%2d - %2d = ", n1, n2 );    // 通常不会超过12个字符,  这样的话，一行按照80个字符算的话, 列5个算式没问题, 每个算式之间间隔4个字符
        
	    return buf;
    }

    // 创建指定限制条件的算式
    int DLL_EXPORT genEquation( int limit, int row, int col, char * title, char * filename, int No ) 
    {
        FILE * fp;
        char   buffs[1024], lbuf[16];
        int    lines = 0;

        fp = fopen( filename, "a+" );
        if ( fp == NULL ) 
            return;
        
        fprintf( fp, "\n%d.%s\n", No, title );
        lines += 2;    // 第1,2行
        
        for ( int i =0; i < 80; i ++) 
            fprintf( fp, "-" );
        fprintf( fp, "\n\n" );

        lines += 2;   // 第3,4行
        
        memset( buffs, 0, 1024 );
        memset( lbuf, 0, 16 );
        srand( limit );

        int count = 0;
        for (int i = 0; i < row; i ++ ) {
            for ( int j = 0; j < col ; j ++ ) {
                while( 1 )  {

                    // 创建随机数, 如果是单数则用加法, 双数用减法        
                    if ( rand() % 2 == 1 ) { // 单数, 用加法

                       // printf("--加法--\n");
                        sprintf( lbuf, "%s", addlimited( limit ) );
                    } else {
                        //printf("--减法---\n");
                        sprintf( lbuf, "%s", sublimited( limit ) );
                    }

                    //printf("lbuf=%s,\n buffs=%s\nstrstr()=%s\n", lbuf, buffs, strstr(buffs, lbuf) );
                    if ( ! strstr( buffs, lbuf )  ) {   // 如果 lbuf 不在bufs 中出现, 则表示该算式是新的, 否则表示已经有该算式了, 需要重新找.
                        fprintf( fp, "%s", lbuf );
                        strcat(buffs, lbuf);      // 把新算式拼接到缓冲中.
                        memset( lbuf, 0, 16 );
                        count ++;
                        //printf("%d,-----------找到了一个算式====!\n", count);
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
 
    // 创建符合条件的多日的练习算式
    void DLL_EXPORT getDaysExercise( int maxLines, int limit, char *title, int rows, int cols, char * filename, int days )
    {
        int     lines = 0, l ;
        FILE  * fp;
 
        for ( int i = 0; i < days; i ++ ) {
            lines = genEquation( limit, rows, cols, title, filename, i ); 
            l = lines;
            printf("%d..", i);

            while ( (lines + l) < maxLines && i < days ) {   // 如果当前行数没有超过一页的内容, 则判断增加一倍后能否超过
                i ++;
                lines += genEquation( limit, rows, cols, title, filename, i ); 
                printf("%d..", i);
            }

            if ( lines < maxLines ) {  // 如果没有满一页, 补充空行
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
    * 加减混合运算: AddSubMixedOperation()
    * 参数说明:
    *     
    */
   void AddSubMixedOperation( int limit, char *opers,  int flag )
   {
   }

