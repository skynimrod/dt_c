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

    char *	ch_monfmt( char *out, char *in, int ilen );

    char *	StrNumAdd( char *NumStr1, char *NumStr2, char *deststr, int flag );   // flag ȱʡ=2
    

    void  RemoveCrLf( char	* instr );

    char *  Space( int num );

    int  lpatoi( char * instr );

    int  IsNumericStr( char *buf );
    int  IsNumericStr2( char *buf, char splitchar );                        // splitchar ȱʡ=','
    int  NumericStrSort( char *buf, char splitchar );                       // splitchar ȱʡ=','

    int  ReplaceSubStr( char *buf, char *substr1, char *substr2, unsigned int buflen );

    int  MaskSubStr( char *buf, char *substr, char maskchar );

    int  ValidIpstr( char *IPstr );

    int  ValidIplong( long IPlong );
    int  Ipstr2Iplong( char *ipstr, long *iplong );
    int  Iplong2Ipstr( long iplong, char *ipstr );

    unsigned int   IP2HEX( char * pszIP);

    //bool  HexIP2str( unsigned int uiIPaddress, char * pszIPAddress);     // �Ѿ����룬δ����

    int  IsChinese( const char *pszSource );

    int  nAlphaOfStr( const char *str );

    int  cstrlen( const char *pszSource );

    unsigned int  cstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
    unsigned int  mstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize );
    int  GetFirstWord( const char *pszSource, char *deststr );
        
