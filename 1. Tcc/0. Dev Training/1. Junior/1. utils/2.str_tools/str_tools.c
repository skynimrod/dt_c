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
    //        1.   isNumberStr()            ---  检查目标字符串是否是纯数字字符串, 例如: "1234"
    //        2.   ()             ---  
    //        3.   ()             ---  
    //        4.   
    //
    #include    <stdio.h>
    #include 	<stdbool.h>
    #define DLL_EXPORT __declspec(dllexport)   //__stdcall

    //*********************************************************************************
    //  函数名称 : IsNumberStr()
    //  功    能 :
    //			判断给定字符串是否识数字型字符串, 如: "1234"
    //  入口参数 :    
    //			char	*buf		给定字符串缓冲区
    //  出口参数 :
    //			无
    //  返 回 值 :
    //			true	: 表示是数字型字符串
    //		  flase	: 表示不是数字型字符串
    //  Author   :      
    //            Adams
    //  Create Date :
    //            2002.04.23
    //  Modify history (modifier and date) :
    //          
    //  使用说明 :
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


