    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    str_tools
    //
    // Description : about string operation.
    //
    // Interface :
    //        1.   IsNumberStr()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    #ifndef __STR_TOOLS_H__    // define str_tools.h Start {
    #define __STR_TOOLS_H__

    #include <stdbool.h>                // bool 类型的定义在该文件中, 如果没有包含该文件,  引用 str_tools.h时, 总会出现inc/str_tools.h:49: error: ';' expected (got "IsNumberStr")

    char *	ltrim( char	*str ); 
    char *	mtrim( char	*str ); 
    char *	rtrim( char	*str ); 

    char *	UpperStr( char	*str ); 

    char *	LowerStr( char	*str ); 
    
    int 	GetQuotePos( char *str, char quote, int *i, int *j );

    char *	CutMemo( char	*str, char CutChar );
    char *	CutMemoEx( char	*str, char *remstr );
    int 	IfHaveQuote( char *str, char Quote );
    char *	CutQuote( char	*str, char Quote );
    
    char *	DelCharInString( char *str, char ch );
	char *  DelCharsInString( char *str, char *chs );

    char *	ch_monfmt( char *out, char *in, int ilen );

    char *	StrNumAdd( char *NumStr1, char *NumStr2, char *deststr, int flag );   // flag 缺省=2
    

    void  RemoveCrLf( char	* instr );

    char *  Space( int num );

    int  lpatoi( char * instr );

    bool IsNumberStr( char *buf );

    int  IsNumericStr( char *buf );
    int  IsNumericStr2( char *buf, char splitchar );                        // splitchar 缺省=','
    int  NumericStrSort( char *buf, char splitchar );                       // splitchar 缺省=','

    int  ReplaceSubStr( char *buf, char *substr1, char *substr2, unsigned int buflen );

    int  MaskSubStr( char *buf, char *substr, char maskchar );

    int  ValidIpstr( char *IPstr );

    int  ValidIplong( long IPlong );
    int  Ipstr2Iplong( char *ipstr, long *iplong );
    int  Iplong2Ipstr( long iplong, char *ipstr );

    unsigned int   IP2HEX( char * pszIP);

    //bool  HexIP2str( unsigned int uiIPaddress, char * pszIPAddress);     // 已经编码，未整理

    int  IsChinese( const char *pszSource );

    int  nAlphaOfStr( const char *str );

    int  cstrlen( const char *pszSource );

    unsigned int  cstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
    unsigned int  mstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
    int  GetFirstWord( const char *pszSource, char *deststr );
        
    #endif
