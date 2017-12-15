    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    str_tools
    //
    // Description : about str operation.
    //
    // Interface :
    //        1.   isNumberStr()            ---  ���Ŀ���ַ����Ƿ��Ǵ������ַ���, ����: "1234"
    //        2.   ()             ---  
    //        3.   ()             ---  
    //        4.   
    //
    #include    <stdio.h>
    #include 	<stdbool.h>
    #define DLL_EXPORT __declspec(dllexport)   //__stdcall

    //*********************************************************************************
    //  �������� : IsNumberStr()
    //  ��    �� :
    //			�жϸ����ַ����Ƿ�ʶ�������ַ���, ��: "1234"
    //  ��ڲ��� :    
    //			char	*buf		�����ַ���������
    //  ���ڲ��� :
    //			��
    //  �� �� ֵ :
    //			true	: ��ʾ���������ַ���
    //		  flase	: ��ʾ�����������ַ���
    //  Author   :      
    //            Adams
    //  Create Date :
    //            2002.04.23
    //  Modify history (modifier and date) :
    //          
    //  ʹ��˵�� :
    //            
    //  Standard .h  : 
    //           
    //  Develope Tools: 
    //			  Visual C++ 6.0 /StuSP5
    //  Environment: 
    //           1. Windows 2000 Advanced Server/SP2
    //*********************************************************************************
    //
    bool DLL_EXPORT IsNumberStr( char *buf ) 
    {
    	unsigned int		i;
    
    	if ( !buf || !*buf )
    		return false;
  
    	for ( i = 0; i < strlen(buf); i ++ ) 
    		if ( buf[i] < '0' || buf[i] > '9' )
    			return false;
    
    	return true;
    }


