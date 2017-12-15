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
    //        1.   isValidDateStr()         ---  ���Ŀ�������ַ����Ƿ��ǺϷ�������ʱ��
    //        2.   IsMaxMonth()             ---  �ж�ָ���·��Ƿ���31��Ĵ��·�
    //        3.   IsLeapYear()             ---  �ж�ָ�����Ƿ�������
    //        4.   
    //
    #include    <stdio.h>
    #include    <stdbool.h>                  // true, false ���߼�ֵ����
    #include    <stdlib.h>                   // atoi()
    #include    "str_tools.h"                // IsNumberStr()

    #include    "length.h"                   // L_YEAR, �ȶ���

    #define DLL_EXPORT __declspec(dllexport)   //__stdcalll

    bool DLL_EXPORT  ValidShortDateStr( char *DateStr );
    int  DLL_EXPORT  IsMaxMonth( char *MonthStr );
    int  DLL_EXPORT  IsLeapYear( char *Year );

    //*********************************************************************************
    //  �������� : ValidShortDateStr()  
    //  ��    �� :
    //			���������ַ����Ƿ�Ϸ�
    //  ��ڲ��� :
    //			char	* DateStr	ָ���������ַ���
    //  ���ڲ��� :
    //			��
    //  �� �� ֵ :
    //			bool	ֵΪtrue ��ʾָ���������ǺϷ�����
    //					ֵΪfalse ��ʾָ���������ǷǷ�����
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28
    //  Modify history (modifier and date) :
    //          
    //  ʹ��˵�� :
    //			�Ϸ��Ķ������ַ�����ʽ����:
    //				YYYY-MM-DD
    //			��: 2002-02-28 
    //			������1900����2099��֮��
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
    		if ( atoi(month) == 2 ) {	// 2����Ҫ�ж�����
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
    //  �������� : IsMaxMonth()  
    //  ��    �� :
    //			�ж�ָ���·��Ƿ���31����·�
    //  ��ڲ��� :
    //			char	* MonthStr		ָ���·��ַ���
    //  ���ڲ��� :
    //			��
    //  �� �� ֵ :
    //			int		ֵΪ1 ��ʾָ���·���31����·�
    //					ֵΪ0 ��ʾָ���·ݲ���31����·�
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28            
    //  Modify history (modifier and date) :
    //                                      
    //  ʹ��˵�� :
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
    //  �������� : IsLeapYear()  
    //  ��    �� :
    //			�ж�ָ�����Ƿ�������
    //  ��ڲ��� :
    //			��
    //  ���ڲ��� :
    //			��		
    //  �� �� ֵ :	
    //			-1	: ����
    //			0	: ��������
    //			1	: ������
    //  Author   :
    //		Adams                 
    //  Create Date :
    //		2002.02.28            
    //  Modify history (modifier and date) :
    //                                      
    //  ʹ��˵�� :
    //		VB 6.0 ���ø�ʽ:
    //			Private Declare Function IsLeapYear Lib "TimeApi.dll" (ByVal str As String) As Long
    //				��
    //			Private Declare Function IsLeapYear Lib "TimeApi.dll" (ByVal str As String) As Integer
    //			��Ҫע�����,  str ����ڲ���, VB 6.0 ������, �� ByVal, ����, ����ǰ, ���������㹻�Ŀռ�
    //			���������·�ʽ����:
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
    
