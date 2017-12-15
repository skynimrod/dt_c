    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    date_tools
    //
    // Description : about date operation.
    //
    // Interface :
    //        1.   isValidDateStr()         ---  检查目标日期字符串是否是合法的日期时间
    //        2.   IsMaxMonth()             ---  判断指定月份是否是31天的大月份
    //        3.   IsLeapYear()             ---  判断指定年是否是润年
    //        4.   
    //
    #include    <stdio.h>
    #include    <stdbool.h>                  // true, false 等逻辑值定义
    #include    <stdlib.h>                   // atoi()
    #include    "str_tools.h"                // IsNumberStr()

    #include    "length.h"                   // L_YEAR, 等定义

    #define DLL_EXPORT __declspec(dllexport)   //__stdcalll

    bool DLL_EXPORT  ValidShortDateStr( char *DateStr );
    int  DLL_EXPORT  IsMaxMonth( char *MonthStr );
    int  DLL_EXPORT  IsLeapYear( char *Year );

    //*********************************************************************************
    //  函数名称 : ValidShortDateStr()  
    //  功    能 :
    //			检查短日期字符串是否合法
    //  入口参数 :
    //			char	* DateStr	指定短日期字符串
    //  出口参数 :
    //			无
    //  返 回 值 :
    //			bool	值为true 表示指定短日期是合法日期
    //					值为false 表示指定短日期是非法日期
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28
    //  Modify history (modifier and date) :
    //          
    //  使用说明 :
    //			合法的短日期字符串格式如下:
    //				YYYY-MM-DD
    //			如: 2002-02-28 
    //			必须在1900年与2099年之间
    //  Standard .h :
    //      
    //  Develope Tools:
    //		tcc
    //  Environment:
    //           1. Windows 2000 Advanced Server/SP2
    //*********************************************************************************
    //
    bool DLL_EXPORT ValidShortDateStr( char *DateStr )
    {
    	char	year[L_YEAR+1], month[L_MONTH+1], day[L_DAY+1];
    	int		n;
    
    	if ( DateStr == NULL || strlen(DateStr)!=10 ) 
    		return false;
    
    	if ( DateStr[4] != '-' || DateStr[7] != '-' ) 
    		return false;
    
    	memset( year, 0, L_YEAR+1 );
    	memset( month, 0, L_MONTH+1 );
    	memset( day, 0, L_DAY+1 );
    
    	memcpy( year, DateStr, L_YEAR );
    	memcpy( month, DateStr+5, L_MONTH );
    	memcpy( day, DateStr+8, L_DAY );

    	if ( !IsNumberStr( year ) || !IsNumberStr( month ) || !IsNumberStr( day ) ) {
    		return false;
    	}

    	if ( atoi(year) < 1900 || atoi(year) > 2099 )
    		return false;
    
    	if ( atoi(month)< 1 || atoi(month) > 12 )
    		return false;
    
    	if ( IsMaxMonth(month) ) {
    		if ( atoi(day) < 1 || atoi(day) > 31 )
    			return false;
    	}else {
    		if ( atoi(month) == 2 ) {	// 2月需要判断润年
    			if ( IsLeapYear(year) ) {
    				if ( atoi(day) < 1 || atoi(day) > 29 )
    					return false;
    			} else {
    				if ( atoi(day) < 1 || atoi(day) > 28 )
    					return false;
    			}
    		}else{
    			if ( atoi(day) < 1 || atoi(day) > 30 ) 
        			return false;
    		}
    	}
    
    	return true;
    }

    //*********************************************************************************
    //  函数名称 : IsMaxMonth()  
    //  功    能 :
    //			判断指定月份是否是31天的月份
    //  入口参数 :
    //			char	* MonthStr		指定月份字符串
    //  出口参数 :
    //			无
    //  返 回 值 :
    //			int		值为1 表示指定月份是31天的月份
    //					值为0 表示指定月份不是31天的月份
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28            
    //  Modify history (modifier and date) :
    //                                      
    //  使用说明 :
    //			
    //  Standard .h :
    //      
    //  Develope Tools:
    //			  Visual C++ 6.0 /StuSP5
    //  Environment:
    //           1. Windows 2000 Advanced Server/SP2
    //*********************************************************************************
    //
    int DLL_EXPORT IsMaxMonth( char *MonthStr )
    {
    	int		month;
    
    	if ( !IsNumberStr( MonthStr ) ) {
    		return -1;
    	}
    
	month = atoi(MonthStr);

	if ( (month<1) || (month>12) ) return -1;
    
    	if ( month == 1 || month == 3 || month == 5 || month == 7 || 
    			month == 8 || month == 10 || month == 12 ) 
    		return 1;
    	else 
    		return 0;
    
    }

    //*********************************************************************************
    //  函数名称 : IsLeapYear()  
    //  功    能 :
    //			判断指定年是否是润年
    //  入口参数 :
    //			无
    //  出口参数 :
    //			无		
    //  返 回 值 :	
    //			-1	: 出错
    //			0	: 不是润年
    //			1	: 是润年
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28            
    //  Modify history (modifier and date) :
    //                                      
    //  使用说明 :
    //		VB 6.0 调用格式:
    //			Private Declare Function IsLeapYear Lib "TimeApi.dll" (ByVal str As String) As Long
    //				或
    //			Private Declare Function IsLeapYear Lib "TimeApi.dll" (ByVal str As String) As Integer
    //			需要注意的是,  str 是入口参数, VB 6.0 声明中, 用 ByVal, 另外, 调用前, 必须申请足够的空间
    //			可以用如下方式申请:
    //				str = String$(8, " ")
    //  Standard .h :
    //      
    //  Develope Tools: 
    //			  Visual C++ 6.0 /StuSP5
    //  Environment:
    //           1. Windows 2000 Advanced Server/SP2
    //*********************************************************************************
    //
    int DLL_EXPORT IsLeapYear( char *Year )
    {
    	int		year;

        if ( ! IsNumberStr( Year ) ) {	
    		return -1;
    	}
    
	year = atoi(Year);
    	if ( (year%400== 0) || ( (year%4 == 0) && (year%100 != 0) ) ) {
    		return 1;
    	}
    		
    	return 0;
    }
    
