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
    //       参见: C99  --- 7.19.3   P277
    //                      7.19.4   P279   关于文件的操作
    // Interface :
    //        1.   GetCurSecond()
    //        2.   ()
    //        3.   ()
    //        4.   
    //
    #include    <windows.h>
    #include    <stdio.h>
    #include    <stdbool.h>                     //  使用 true, false 值的定义
    #include    <io.h>                          //  _sopen()
    #include    <sys\stat.h>                    // 用来获取文件大小
    #include    <time.h>

    #include    "time_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    int GetCurSecond();
    
    //**************************************************************************
    //  函数名称 : GetCurSecond()
    //  功    能 :
    //		获取当前时间的秒数
    //  入口参数 :
    //            str : 字符串
    //  出口参数 :
    //            无
    //  返 回 值 :
    //            char  * : 目标字符串
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			调用后源串不变, 对中文空格不处理, 只处理开头西文空格与Tab键            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    DLL_EXPORT int GetCurSecond( ) 
    {
		time_t		now;
		struct tm	*ts;

		time( &now );
		ts = localtime( &now );

		return ( ts->tm_sec );
    }


    DLL_EXPORT int GetCurMilSecond( ) 
    {
        SYSTEMTIME currentTime;
        GetSystemTime(&currentTime);
        /*
       printf("time: %u/%u/%u %u:%u:%u:%u %d\n",           
               currentTime.wYear,currentTime.wMonth,currentTime.wDay,
               currentTime.wHour,currentTime.wMinute,currentTime.wSecond,
               currentTime.wMilliseconds,currentTime.wDayOfWeek);
        */
		return (  currentTime.wMilliseconds );
    }


