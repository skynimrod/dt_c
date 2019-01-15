    //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.，LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// 模块		: Astring.H
	// 
	// 描述		: 标准公用代码API, 有关字符串访问的应用接口函数, SDK版本, 可以用在许多工具中.
	//
	// 功能函数	:
	//			
	//
	// 历史		:
	//
	// 注意		:
	//		
	//***************************************************************************
	//
	//

	#ifndef _ASTRING_H__  // define Astring.h Start {
	#define _ASTRING_H__

	#pragma once

	char *__stdcall	ltrim( char	*str ); 
	char *__stdcall	mtrim( char	*str ); 
	char *__stdcall	rtrim( char	*str ); 

	char *__stdcall	UpperStr( char	*str ); 
	char *__stdcall	LowerStr( char	*str ); 

	int __stdcall	GetQuotePos( char *str, char quote, int *i, int *j );
	char *__stdcall	CutMemo( char	*str, char CutChar );
	char *__stdcall	CutMemoEx( char	*str, char *remstr );
	int __stdcall	IfHaveQuote( char *str, char Quote );
	char *__stdcall	CutQuote( char	*str, char Quote );

	char *__stdcall	DelCharInString( char *str, char ch );

	char *__stdcall	ch_monfmt( char *out, char *in, int ilen );

	char *__stdcall	StrNumAdd( char *NumStr1, char *NumStr2, char *deststr, int flag=2 );

	void __stdcall RemoveCrLf( char	* instr );
	char * __stdcall Space( int num );

	int __stdcall lpatoi( char * instr );
	int __stdcall IsNumericStr( char *buf );
	int __stdcall IsNumericStr2( char *buf, char splitchar=',' );
	int __stdcall NumericStrSort( char *buf, char splitchar=',' );

	int __stdcall ReplaceSubStr( char *buf, char *substr1, char *substr2, unsigned int buflen );
	int __stdcall MaskSubStr( char *buf, char *substr, char maskchar );

	int __stdcall ValidIpstr( char *IPstr );
	int __stdcall ValidIplong( long IPlong );
	int __stdcall Ipstr2Iplong( char *ipstr, long *iplong );
	int __stdcall Iplong2Ipstr( long iplong, char *ipstr );

	unsigned int __stdcall  IP2HEX( char * pszIP);
	bool __stdcall HexIP2str( unsigned int uiIPaddress, char * pszIPAddress);

	int __stdcall IsChinese( const char *pszSource );
	int __stdcall nAlphaOfStr( const char *str );

	int __stdcall cstrlen( const char *pszSource );
	unsigned int __stdcall cstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
	unsigned int __stdcall mstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
	int __stdcall GetFirstWord( const char *pszSource, char *deststr );


	#endif // define Astring.h End }

