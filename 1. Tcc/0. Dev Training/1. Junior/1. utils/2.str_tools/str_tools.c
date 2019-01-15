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
    //		1.  ltrim()   
    //		2.  mtrim()
    //		3.  rtrim()
    //		4.  UpperStr()
    //		5.  LowerStr()
    //		6.  GetQuotePos()
    //		7.  CutMemo()	
    //		8.  CutMemoEx() 
    //		9.  IfHaveQuote()
    //		10. CutQuote()
    //		11. DelCharInString()
    //		12. ch_monfmt()
    //		13. monfmt()
    //		14. StrNumAdd()
    //		15. RemoveCrLf()
    //		16. Space();
    //		17. ReplaceSubStr()
    //
    //		20. IsChinese()
    //		21. nAlphaOfStr()
    //		22. cstrlen()
    //		23. cstrcpy()
    //		24. mstrcpy()
    //		25. GetFirstWord()
    //
    //		31. lpatoi()
    //
    //		32. IsNumericStr()
    //		33. ValidIpstr()
    //		34. ValidIplong()
    //		35. Ipstr2Iplong()
    //		36. Iplong2Ipstr()
    //      37. isNumberStr()            ---  检查目标字符串是否是纯数字字符串, 例如: "1234"
    //
    // 历史		:
    //
    //***************************************************************************
    //
    //
    //
    //
    #include    <stdio.h>
    #include 	<stdbool.h>
    #include    <stdlib.h>
    #include    <string.h>
    #include    <ctype.h>

    #include    "str_tools.h"

    #define DLL_EXPORT __declspec(dllexport)   //

    //**************************************************************************
    //  函数名称 : ltrim()
    //  功    能 :
    //			删去源串的左边空格
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
    //
    char * DLL_EXPORT ltrim(char	*str)  
    {
    	int			i;
    
        printf("\nHello, this is ltrim()!\n");
    	if ( !str || !*str )
    		return( NULL );
    	
        i = 0;

        while ( (str[i] == 0x20) || (str[i] == 0x9) ) 
    		i++;

        str += i;

        str[0] = 'A';

        return( str );
    
        //return( str + i );
    }
	
    //**************************************************************************
    //  函数名称 : mtrim()
    //  功    能 :
    //             删去源串中间的西文空格与Tab键      
    //  入口参数 :
    //            str : 字符串          
    //  出口参数 :
    //            无                    
    //  返 回 值 :
    //            char  * : 目标字符串  
    //  Author   :
    //            Adams                 
    //  Create Date :
    //            1999.07.19            
    //  Modify history (modifier and date) :
    //                                      
    //  使用说明 :
    //			调用后源串与目标串相同, 对中文空格不处理, 对字符串两头的西文空格与Tab键不处理
    //			系统能处理的最大字符串长度为1024字节
    //  Standard .h  :
    //            string.h--strlen()        
    //  Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //  Environment:
    //           1. SCO UNIX 5.0.4          
    //           2. Windows NT WorkStation SimpleChinese 4.0 
    //              Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    DLL_EXPORT char * mtrim( char * str )  
    {
    	unsigned int	i, j, k, m, n;
    	char			tmpstr[1024];
    
    	if ( !str || !*str )
    		return(NULL);
    	
        i = 0;  j = strlen(str)-1; k = 0; m = 1;
    	
        // i 为第一个不是空格的字符位置
    	// j 为倒数第一个不是空格的字符位置
    	// k 为字符串中间空格的数目
    	
        while ( str[i] == 0x20 || str[i] == 0x9 ) 
    		i ++;  
    
        while ( str[j] == 0x20 || str[j] == 0x9 ) 
    		j --; 
    
    	while ( m < (j-i-1) ) {
    		if ( str[i+m] == 0x20 || str[i+m] == 0x9 )   
    			k ++; 
    			m ++;
    	}

        if ( k == 0 ) 
    		return ( str );

        memset( tmpstr, 0, sizeof(tmpstr) );

        if ( i > 0 ) 
    		memcpy( tmpstr, str, i );

        m = 0; n = 0;
    	while ( i+m <= j ) {
    		if ( str[i+m] != 0x20 && str[i+m] != 0x9 ) {
    			tmpstr[i+n] = str[i+m]; n ++;
    		}
    		m ++;
    	}

        while ( j < strlen(str) ) {
    		tmpstr[i+n] = str[j+1];
    		i ++; j ++;
    	}	

        strcpy( str, tmpstr );
    	str[strlen(tmpstr)] = 0;
    		return( str );
    }

	//**************************************************************************
	//  函数名称 : rtrim()
	//  功    能 :
	//             删去源串的右边空格      
	//  入口参数 :
	//            str : 字符串          
	//  出口参数 :
	//            无                    
	//  返 回 值 :
	//            char  * : 目标字符串  
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			调用后源串与目标串相同, 对中文空格不处理, 只处理结尾西文空格与Tab键            
	//  Standard .h  :
	//            string.h--strlen()        
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	rtrim(char	*str)
	{
		int		i;
	
		if ( !str || !*str )
			return( NULL );
	
		i = strlen(str);

		while ( str[i-1] == 0x20 || str[i-1] == 0x9 ) 
			i --;

		str[i]=0;

		return( str );
	}

	//**************************************************************************
	//  函数名称 : UpperStr()
	//  功    能 :
	//             将源字符串中的小写字母变为大写字母
	//  入口参数 :
	//			str : 字符串指针(可能含有小写字母)
	//  出口参数 :
	//			str : 字符串指针(字母全为大写)
	//  返 回 值 :
	//			char  * : 目标字符串(字母全为大写)
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			调用后源串与目标串相同
	//  Standard .h  :
	//			1. string.h--strlen()
	//			2. ctype.h--islower()、toupper()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	UpperStr( char	*str )
	{
		unsigned int		i;
	
		i = 0;

		while ( i < strlen(str) ) {
			if ( islower( str[i] ) ) 
				str[i] = toupper( str[i] );
			
			i ++;
		}

		return(str);
	}

	//**************************************************************************
	//  函数名称 : LowerStr()
	//  功    能 :
	//             将源字符串中的大写字母变为小写字母
	//  入口参数 :
	//			str : 字符串指针(可能含有大写字母)
	//  出口参数 :
	//			str : 字符串指针(字母全为小写)
	//  返 回 值 :
	//			char  * : 目标字符串(字母全为小写)
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			调用后源串与目标串相同
	//  Standard .h  :
	//			1. string.h--strlen()
	//			2. ctype.h--islower()、toupper()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	LowerStr( char	*str )
	{
		unsigned int		i;
	
		i = 0;

		while ( i < strlen(str) ) {
			if ( isupper(str[i]) ) 
				str[i] = tolower( str[i] );

			i ++;
		}

		return( str );
	}

	//**************************************************************************
	//  函数名称 : GetQuotePos()
	//  功    能 :
	//			获得源字符串中指定引号字符的位置
	//  入口参数 :
	//		char	*str	字符串指针(可能含有大写字母)
	//		char	quote	指定引号的值, 广义的引号, 
	//		int		*i		整型指针,用来存放第一个引号的位置
	//		int		*j		整型指针,用来存放第二个引号的位置(相对于第一个引号的位置)
	//  出口参数 :
	//		int		*i		整型指针,用来存放第一个引号的位置
	//		int		*j		整型指针,用来存放第二个引号的位置(相对于第一个引号的位置)
	//  返 回 值 :
	//		int		: 0 表示成功
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			广义的引号指任何一个ASCII字符都可以看作是一个引号, 与上下文有关
	//			若引号不匹配: i=第一个引号的实际位置, j=0
	//			若无引号    : i=0, j=0
	//			i从0开始, j相对于i从0开始
	//			例: "aaa\"ccc\"bbb", i = 3,j=4
	//
	//			无论有否指定引号, 都返回0，所以不用检查返回值
	//  Standard .h  :
	//		1. string.h--strlen()、strchr()、memset()、memcpy
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT int  GetQuotePos( char *str, char quote, int *i, int *j )
	{
		char	tmpstr[256];

	
		if ( strchr(str, quote) == NULL) {
			*i = 0;    *j = 0;    
			return( 0 );
		}

		*i = strlen(str) - strlen( strchr(str, quote) );	/* 引号键位置 */

		// tmpstr为第一个引号后的字符串
		memset( tmpstr, 0, sizeof(tmpstr) );
		memcpy( tmpstr, strchr(str,quote)+1, strlen(strchr(str,quote)) - 1 );

		// j 为相对于i第二个引号的位置
		if ( strchr(tmpstr, quote) == NULL ) 
			*j = 0;
		else
			*j = strlen( tmpstr)-strlen(strchr(tmpstr, quote) )+1;

		return 0;
	}

	//**************************************************************************
	//  函数名称 : CutMemo()
	//  功    能 :
	//			删去源字符串中指定删除标识符(注释符)后所有字符
	//  入口参数 :
	//		char	*str     : 源字符串指针
	//		char	CutChar : 字符, 注释符
	//  出口参数 :
	//			str : 字符串指针, 已经删除了注释
	//  返 回 值 :
	//			char  * : 目标字符串
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			符号\'与\"做为引号,系统使用,不能用作注释符,因为#有时会作为有用字符
	//			若注释符为非法('),源字符串不变,返回NULL
	//			若注释符在引号(')(引号)内,源字符串不变,返回NULL
	//
	//			调用后源串与目标串相同
	//
	//  Standard .h  :
	//		1. string.h--strlen()、strchr()
	//	User .h : 
	//		1. Astring.h--GetQuotePos()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//	问  题  : 
	//			似乎不是删除引号内的内容
	//***************************************************************************
	//
	DLL_EXPORT char *	CutMemo( char	*str, char CutChar )
	{
		int		i, j, k;
		char	*tmpstr;
		int		pos, len;

		if ( CutChar == '\'' || CutChar == '\"' ) 		// 注释键不能为\' 或 \"
			return( NULL );

		if ( strchr(str,CutChar) == NULL ) 
			return( str );

		pos = 0; len = strlen( str );
		tmpstr = str;

		while ( pos <= len ) {
			//	注释键位置
			if (strchr( tmpstr, CutChar ) != NULL ) 
				k = strlen( tmpstr) - strlen(strchr(tmpstr, CutChar) );	
			else
				k = strlen( tmpstr );

			// 引号位置 
			GetQuotePos( tmpstr,'\'', &i, &j);

			// 注释位置在引号位置前面,引号不匹配 
			if ( k < i || j == 0 ) {	
				GetQuotePos( tmpstr,'\"', &i, &j);
				if ( k < i || j == 0 ) {
					pos = pos + k ;
					break;
				}
			}

			pos = pos + i + j + 1;
	
			if ( pos >= len) 
				break;

			tmpstr = tmpstr + i + j + 1;
		}
	
		if ( pos < len ) 
			str[pos] = 0;

		return( str );
	}

	//*********************************************************************************
	//  函数名称 : CutMemoEx()  
	//  功    能 :
	//			删去源字符串中指定删除标识符(注释符)后所有字符, CutMemo()的扩展
	//  入口参数 : 
	//		char	*str     : 源字符串指针
	//		char	*remstr : 字符串, 注释符
	//  出口参数 : 
	//			str : 字符串指针, 已经删除了注释
	//  返 回 值 : 
	//			char  * : 目标字符串
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.07.01            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			符号\'与\"做为引号,系统使用,不能用作注释符,因为#有时会作为有用字符
	//			若注释符为非法('),源字符串不变,返回NULL
	//			若注释符在引号(')(引号),源字符串不变,返回NULL
	//
	//			调用后源串与目标串相同
	//
	//		需要注意的是, 注释的内容在内存中仍然存在, 只是被尾零截断
	//
	//		VB 6.0 调用格式:
	//			Private Declare Function CutMemoEx Lib "Astring.dll" (ByVal buf As String, ByVal str As String) As String
	//
	//		需要注意的是, VB中调用完后, (由于显示的问题,其实已经处理过了)重新赋值才可以看到处理后的结果
	//
	//		用例:
	//			...
	//			Dim buf As String, str As String, ret As String
	//			...
	//			buf = "Hello '//' List a '#' Adams"
	//			str = "//"
	//			ret = string$(len(buf), " ")
	//			...
	//			ret = CutMemoEx(buf, str)
	//			Text1.Text = ret
	//			...
	//
	//
	//	维护说明 :
	//
	//		Standard .h :
	//			string.h ------ strstr(), strlen() 
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		其他调用该函数的库及函数名:
	//			CutMemo()
	//
	//*********************************************************************************
	//
	DLL_EXPORT char *	CutMemoEx( char	*str, char *remstr )
	{
		int		i, j, k;
		char	*tmpstr;
		int		pos, len;

		if ( !str || !*str || !remstr || !*remstr )
			return str;

		if ( strlen(remstr) == 1 )
			return CutMemo( str, remstr[0] );

		if ( strstr(str,remstr) == NULL ) 
			return( str );

		pos = 0; len = strlen( str );
		tmpstr = str;

		while ( pos <= len ) {
			//	注释键位置
			if (strstr( tmpstr, remstr ) != NULL ) 
				k = strlen( tmpstr) - strlen(strstr(tmpstr, remstr) );	
			else
				k = strlen( tmpstr );

			// 引号位置 
			GetQuotePos( tmpstr,'\'', &i, &j);

			// 注释位置在引号位置前面,引号不匹配 
			if ( k < i || j == 0 ) {
				GetQuotePos( tmpstr,'\"', &i, &j);
				if ( k < i || j == 0 ) {
					pos = pos + k ;
					break;
				}
			}

			pos = pos + i + j + 1;
	
			if ( pos >= len) 
				break;

			tmpstr = tmpstr + i + j + 1;
		}
	
		if ( pos < len ) 
			str[pos] = 0;

		return( str );
	}


	//**************************************************************************
	//  函数名称 : IfHaveQuote()
	//  功    能 :
	//			判断源字符串中是否包含指定匹配引号
	//  入口参数 :
	//			char	*str		字符串指针
	//			char	Quote		字符, 广义引号字符
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int : 0: 包含 -1:不包含
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0, 调用后源串不变
	//  Standard .h  :
	//		与GetQuotePos()要求的标准头文件相同
	//	User .h : 
	//		1. Astring.h--GetQuotePos()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT int	 IfHaveQuote( char *str, char Quote )
	{
		int		i, j;
	
		GetQuotePos( str, Quote, &i, &j );

		if ( j <= 0 ) 
			return( -1 );
		
		return(0);
	}

	//**************************************************************************
	//  函数名称 : CutQuote()
	//  功    能 :
	//			删去源字符串中的指定匹配引号
	//  入口参数 :
	//			char	*str		字符串指针
	//			char	Quote		指定字符, 广义引号字符
	//  出口参数 :
	//			char	*str		字符串指针(不含指定字符)
	//  返 回 值 :
	//			char  * : 目标字符串(不含指定字符)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0, 调用后源串不变
	//  Standard .h  :
	//		与GetQuotePos()要求的标准头文件相同
	//	User .h : 
	//		1. Astring.h--GetQuotePos()、ltrim()、rtrim()、mtrim()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	CutQuote( char	*str, char Quote )
	{
		int		i,j;
	
		GetQuotePos( str, Quote, &i, &j );

		if ( j <= 0 ) 
			return( str );

		str[i] = 0x20;
		str[i+j] = 0x20;

		return( mtrim( ltrim( rtrim( str ) ) ) );
	}

	//**************************************************************************
	//  函数名称 : DelCharInString()
	//  功    能 :
	//			删去源字符串中的指定字符
	//  入口参数 :
	//			char	*str		字符串指针(可能含有指定字母)
	//			char	ch			指定字符
	//  出口参数 :
	//			char	*str		字符串指针(不含指定字符)
	//  返 回 值 :
	//			char  * : 目标字符串(不含指定字符)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0, 调用后源串不变
	//			要求源串必须不是常量
	//  Standard .h  :
	//		1. string.h--strcpy()
	//	User .h : 
	//		1. Astring.h--GetQuotePos()、ltrim()、rtrim()、mtrim()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	DelCharInString( char *str, char ch )
	{
		char	tmp[BUFSIZ+1];

		int		i, k, j;

		i = 0; k = 0;
		while (str[i]!='\0') {
			if (str[i]==ch) {
				k = i;

				while (str[k]==ch)	// 继续找后面是否有需要删除的字符
					k++;
			
				strcpy(tmp,str+k);
				j = 0;
				while ( tmp[j] != 0 ){
					if ( tmp[j] != ch ) {
						str[i] = tmp[j];
						i ++;
					}
					j ++; 
				}
				str[i] = 0;

			}
			i ++;
		}
		
		return(str);
	}

	//**************************************************************************
	//  函数名称 : ch_monfmt()
	//  功    能 :
	//			将金额阿拉伯数字字符串转为大写金额字符串
	//  入口参数 :
	//		char	*out	大写金额字符串(含单位, 如:十元5分)
	//		char	*in		阿拉伯金额字符串(不含分隔符, 也不含单位, 如:1005, 表示10.05元)
	//		int		ilen	in的长度
	//  出口参数 :
	//		char	*out	大写金额字符串(含单位, 如:十元5分)
	//  返 回 值 :
	//		char	*out	大写金额字符串(含单位, 如:十元5分)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0, 调用后源串不变
	//			要求源串必须不是常量
	//  Standard .h  :
	//		1. string.h--strcpy()
	//	User .h : 
	//		1. 
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	char	chmoney[10][3] = {
		"零",	"壹",	"贰",	"叁",	"肆",	"伍",	"陆",	"柒",	"捌",	"玖"
	};

	char	chunit[3][3] = {
		"拾",	"佰",	"仟"
	};
	
	char	chmisc[4][5] = {
		"元",	"万",	"亿",   "亿万"
	};


	DLL_EXPORT char * ch_monfmt(char *out, char *in, int ilen)
	{
		int		i, j, k, l, tag;
		char	tmpfmt[100];
		int 	flag[4], flag0;

		for (j = 0; j < ilen && in[j] == '0'; j ++);

		i = ilen - 1;
		tmpfmt[0] = 0;

		if ((in[ilen-1] == '0') && (in[ilen-2] == '0')) {
			strcat(tmpfmt, "整");
			tag = 0;
		} else {
			if ( in[ilen-1] != '0' ) {
				strcat(tmpfmt, "分");
				strcat(tmpfmt, chmoney[in[i] - '0']);
				tag = 1;
			}
		}
	
		i --;
	
		if (i >= j)
			if (in[i] == '0') {
				if (tag == 1) 
					if (i-1>=j) {
						strcat(tmpfmt, "零");
						flag0 = 1;
					}
			} else {
				strcat(tmpfmt, "角");
				strcat(tmpfmt, chmoney[in[i] - '0']);
				tag=1;
			}
		
		i --;

		flag[0]=1;
		for ( l=1; l<4; l++ ) {
			flag[l]=0;
			for (k=i-4*l;k>i-4*l-4 && k>=j;k--)
				if (in[k] != '0')
					flag[l]=1;
		}
			
		if (flag[3])
			flag[2]=1;

		for (l = 0; l < 4 && i >= j; l ++) {
			if (flag[l]) {
				if (i >= j) 
					if (in[i] == '0') {
						if ( !flag0 ) 
							if (i-1>=j)
								if (tag) {
									strcat(tmpfmt, "零");
									flag0 = 1;
			     				}
						strcat(tmpfmt, chmisc[l]);
					} else {
						strcat(tmpfmt, chmisc[l]);
						strcat(tmpfmt, chmoney[in[i] - '0']);
						flag0 = 0;
						tag=1;
					}
							
				i --;
				for (k = 0; k < 3 && i >= j ; k ++, i --)
					if (in[i] == '0') {
						if ( !flag0 ) 
							if (i-1>=j)
								if (tag) {
									strcat(tmpfmt, "零");
									flag0 = 1;
								}
					} else {
						strcat(tmpfmt, chunit[k]);
						strcat(tmpfmt, chmoney[in[i] - '0']);
						flag0 = 0;
						tag=1;
					}
			} else 
				if (i-1>=j) {
					i-=4;
					if (flag0==0) {
						if (tag) {
							strcat(tmpfmt, "零");
							flag0=1;
						}
					}
				}
		}
				
		out[0] = 0;
		for (i = strlen(tmpfmt) - 1; -- i >= 0; i --) {
			strcat(out, &(tmpfmt[i]));
			tmpfmt[i] = 0;
		}
	
		return(out);
	}



	monfmt(char *obuf,char *ibuf,int ilen)	
	{
		int	o, i;

		for (o = 0, i = 0; i < ilen - 3 && ibuf[i] == '0'; o ++, i ++) {
			obuf[o] = ' ';

			if (((ilen - 3 - i) % 3)==0)
				obuf[++o] = ' ';
        };

		for (; i < ilen-3; o ++, i ++) {
			obuf[o] = ibuf[i];

			if (((ilen - 3 - i) % 3)==0)
				obuf[++ o]= ',';
		};

		obuf[o ++]=ibuf[ilen-3];
		obuf[o ++]='.';
		obuf[o ++]=ibuf[ilen-2];
		obuf[o ++]=ibuf[ilen-1];
		obuf[o]=0;

		return(0);
	}

	//**************************************************************************
	//  函数名称 : StrNumAdd()
	//  功    能 :
	//			带进位或不带进位的整数型数字字符串加法
	//  入口参数 :
	//		char	*NumStr1	: 整数型数字字符串1, 加数          
	//		char	*NumStr2	: 整数型数字字符串2, 被加数 
	//		char	*deststr	: 整数型数字字符串, 数字和的字符串
	//		int		flag		: 标识。1: 表示 带进位 的加法
	//									2: 表示 不带进位 的加法, 缺省值
	//  出口参数 :
	//		char	*deststr	: 整数型数字字符串, 数字和的字符串
	//  返 回 值 :
	//		char  * : 整数型数字字符串, 数字和的字符串
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			2001.11.05       
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0, 调用后源串不变
	//			要求源串必须不是常量
	//			flag	= 1: 表示 带进位 的加法
	//					  2: 表示 不带进位 的加法, 缺省值
	//  Standard .h  :
	//		1. string.h--strcpy()
	//	User .h : 
	//		1. 
	//  Develope Tools:
	//			  Visual C++ 6.0
	//  Environment:
	//           1. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console、MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	StrNumAdd(char *NumStr1, char *NumStr2, char *deststr, int flag )
	{
		char	maxstr[15+1], minstr[15+1];		// 表示支持最大为百万亿数字
		int		i, j, k;
     
		if ( !NumStr1 || !*NumStr1 || strlen(NumStr1) > 15 || 
			 !NumStr2 || !*NumStr2 || strlen(NumStr2) > 15 )
			return NULL;                //如果两个源串中有一个是NULL或空串,则返回NULL

		i = 0;
		while (NumStr1[i] != '\0') {      //如果两个源串中有一个包含非数字字符,则返回NULL
			if ( NumStr1[i] < '0' || NumStr1[i] > '9' ) 
				return NULL; 

			i ++;
		}
	
		i = 0;
		while (NumStr2[i] != '\0') {      //如果两个源串中有一个包含非数字字符,则返回NULL
			if ( NumStr2[i] < '0' || NumStr2[i] > '9' ) 
				return NULL; 

			i ++;
		}


		if ( 1 == flag ) {		// 表示 带进位 的加法
			sprintf( deststr, "%d", atoi(NumStr1) + atoi(NumStr2) );
			return( deststr );
		}

		if ( 2 == flag ) {		// 表示 不带进位 的加法
			memset( maxstr, 0, 15+1 );	
			memset( minstr, 0, 15+1 );

			if ( atoi(NumStr1) >= atoi (NumStr2) ) {
				memcpy( maxstr, NumStr1, strlen(NumStr1) );
				memcpy( minstr, NumStr2, strlen(NumStr2) );
			} else {
				memcpy( maxstr, NumStr2, strlen(NumStr2) );
				memcpy( minstr, NumStr1, strlen(NumStr1) );
			}

			i = strlen( minstr ) - 1;	
			j = strlen( maxstr ) - 1;

			while (i >= 0 ) {
				k = minstr[i] - '0' + maxstr[j] - '0';

				if ( k >= 10 ) 
					k = k - 10;

				maxstr[j] = k + '0';

				i --;	
				j --;
			}
		
			memcpy( deststr, maxstr, strlen(maxstr) );

			return( deststr );
		}

		return NULL;
	}


	//*********************************************************************************
	//  函数名称 : RemoveCrLf()  
	//  功    能 :
	//			删除输入缓冲区的回车和/或换行
	//  入口参数 :
	//			LPSTR	instr :	Null 结尾的字符串(既有尾0)
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			无
	//  Author   :
	//			Adams
	//  Create Date :
	//			2001.12.11
	//  Modify history (modifier and date) :
	//			
	//  使用说明 :
	//			
	//  Standard .h  :
	//			string.h--strlen()        
	//  Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//  Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT void  RemoveCrLf( char	* instr )
	{
		char * str=instr;

		if ( !str || !*str )
			return;

		while ( instr[strlen(instr)-1] == '\r' || instr[strlen(instr)-1] == '\n') 
			instr[strlen(instr)-1] = 0;

	}

	//*********************************************************************************
	//  函数名称 : Space()  
	//  功    能 :
	//			返回指定个数空格的字符串
	//  入口参数 :
	//			int		num		指定的空格个数
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			返回指定个数空格的字符串
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.04.29
	//  Modify history (modifier and date) :
	//			
	//  使用说明 :
	//			num必须小于1024，否则返回NULL
	//  Standard .h  :
	//			string.h--strlen()        
	//  Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//  Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	#define MAXSPACE	1024		// Space()支持的最多空格数
	
	DLL_EXPORT char *  Space( int num )
	{
		char	buf[MAXSPACE+1];

		if ( num > MAXSPACE )
			return NULL;

		memset( buf, 0, MAXSPACE+1 );
		memset( buf, ' ', num );

		return buf;
	}


	//*********************************************************************************
	//  函数名称 : lpatoi()  
	//  功    能 :
	//			atoi()只能工作在NEAR host 变量, 在大型应用中无法使用。
	//			该函数通过将长类型字符串拷贝到本地变量来"骗过"atoi()，然后做转换。
	//  入口参数 :
	//		char	* instr	: 输入的数字字符串, 最大不能超过35位
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int :	instr的整型值
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//			该函数非常有用，但不是必须要用
	//  Standard .h :
	//            string.h--strlen()
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT int  lpatoi( char * instr )
	{
		char szStr[35];

//		lstrcpy((char *)szStr, instr);

		return atoi(szStr);
	}


	//*********************************************************************************
	//  函数名称 : GetFirstEnWord()  
	//  功    能 :
	//			从一个字符串中获取第一个英文单词
	//  入口参数 :
	//		char	* instr	: 输入的数字字符串, 最大不能超过35位
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int :	instr的整型值
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//			该函数非常有用，但不是必须要用
	//  Standard .h :
	//            string.h--strlen()
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int  GetFirstEnWord( char * instr, char *buf, char splitchar )
	{


		return 0;
	}


	//*********************************************************************************
	//  函数名称 : ValidIpstr()
	//  功    能 :
	//			检查指定字符串是否为合法的IP地址(IPv4)字符串格式
	//  入口参数 :    
	//			char	*IPstr		待检字符串缓冲区
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		0 : 指定字符串是合法的IP地址(IPv4)字符串格式
	//					-1: 指定字符串是非法的IP地址(IPv4)字符串格式
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
	DLL_EXPORT int  ValidIpstr( char *IPstr )
	{
		int			i, j, k;
		char		buf[BUFSIZ+1];

		if ( !IPstr || !*IPstr )
			return -1;

		i = 0; j = 0; k = 0;
		memset( buf, 0, BUFSIZ+1 );

		while ( IPstr[i] != 0 && k < 4 ) {
			if ( IPstr[i] == '.' ) {
				if ( IsNumericStr(buf) != 0 || atoi(buf) > 255 )
					return -1;

				j = 0;
				memset( buf, 0, BUFSIZ+1 );
				k ++;
				// 若有必要，需要在此判断k==1的情况下，aoti(buf)是否〉=1
				// 一般情况下，地址第一位不能是0
			}else {
				buf[j] = IPstr[i];
				j ++;
			}
			i ++; 
		}

		if ( k != 3 ) 
			return -1;

		if ( IsNumericStr(buf) != 0 || atoi(buf) > 255 )
			return -1;

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : ValidIplong()
	//  功    能 :
	//			检查指定长整数是否为合法的IP地址(IPv4)内部格式
	//  入口参数 :    
	//			long	IPlong			待检长整数
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		0 : 指定长整数是合法的IP地址(IPv4)内部格式
	//					-1: 指定长整数是非法的IP地址(IPv4)内部格式
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
	int  ValidIplong( long IPlong )
	{
		// 若有必要，需要在此判断IPlong中的第一个字节内数值是否〉=1
		// 一般情况下，地址第一位不能是0

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : Ipstr2Iplong()
	//  功    能 :
	//			将字符串型IP地址转换为内部格式的IP地址
	//  入口参数 :    
	//			char		*ipstr			字符串型IP地址缓冲区
	//  出口参数 :
	//			long		*iplong			内部格式IP地址缓冲区
	//  返 回 值 :
	//			int		0 : 转换成功
	//					-1: 转换失败
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
	DLL_EXPORT int  Ipstr2Iplong( char *ipstr, long *iplong )
	{
		int			i, j, k ;
		char		tmpbuf[4];		// 合法的IP字符串没端最大为"255", 3+1
		char		buf[5];			// 用来存放IP内部格式的值 4 + 1
		long		tmpip=0;

		if ( ValidIpstr( ipstr ) != 0  )
			return -1;

		*iplong = 0;

		i = 0; j = 0; k = 0;
		memset( tmpbuf, 0, 4 );
		memset( buf, 0, 5 );

		while ( ipstr[i] != 0 ) {
			if ( ipstr[i] == '.' ) {
				buf[k] = atoi(tmpbuf);

				j = 0;
				memset( tmpbuf, 0, 4 );
				k ++;
			}else {
				tmpbuf[j] = ipstr[i];
				j ++;
			}
			i ++; 
		}

		buf[k] = atoi(tmpbuf);

		memcpy( (char *)&tmpip, buf, 4 );

		*iplong = tmpip;

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : Iplong2Ipstr()
	//  功    能 :
	//			将内部格式的IP地址转换为字符串型IP地址
	//  入口参数 :    
	//			long		*iplong			内部格式IP地址缓冲区
	//  出口参数 :
	//			char		*ipstr			字符串型IP地址缓冲区
	//  返 回 值 :
	//			int		0 : 转换成功
	//					-1: 转换失败
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2002.04.23
	//  Modify history (modifier and date) :
	//          
	//  使用说明 :
	//			调用前必须为ipstr申请到足够的内存空间，并清0
	//  Standard .h  : 
	//           
	//  Develope Tools: 
	//			  Visual C++ 6.0 /StuSP5
	//  Environment: 
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT int  Iplong2Ipstr( long iplong, char *ipstr )
	{
		unsigned char	buf[5];			// 用来存放IP内部格式的值 4 + 1
		long			tmpip=iplong;

		if ( ValidIplong( iplong ) != 0  )
			return -1;

		memset( buf, 0, 5 );
		memcpy( buf, (unsigned char *)&tmpip, 4 );

		sprintf( ipstr, "%d.%d.%d.%d", 
				buf[0], buf[1], buf[2], buf[3] );
		

		return 0;
	}

	unsigned int  IPtoHex(char *ip)
	{
	char ip1[4], ip2[4], ip3[4], ip4[4];
	int p = 0, length = 0, j;

	int nLength = strlen(ip);

	for (;p<nLength; p++)
		if (ip[p] == '.') break;
	length = p;
	strncpy(ip1, ip, length);
	ip1[length] = '\0';
	p++;
	j = p;

	for (;p<nLength; p++)
		if (ip[p] == '.') break;
	length = p - j;
	strncpy(ip2, &ip[j], length);
	ip2[length] = '\0';
	p++;
	j = p;

	for (;p<nLength; p++)
		if (ip[p] == '.') break;
	length = p - j;
	strncpy(ip3, &ip[j], length);
	ip3[length] = '\0';
	p++;
	j = p;

	for (;p<nLength; p++)
		if (ip[p] < '0' || ip[p] > '9') break;
	length = p - j;
	strncpy(ip4, &ip[j], length);
	ip4[length] = '\0';

	return ((atoi(ip4)*0x100+atoi(ip3))*0x100+atoi(ip2))*0x100+atoi(ip1);
	}
bool  HexIP2Str(unsigned int uiIPAddress, char * sIPAddress)
{
	int i;
	unsigned char A[4];	
	unsigned int  b=0;
	unsigned char *pBYTE;
	char buff[16];

	b = uiIPAddress;
	pBYTE = (unsigned char *)&b;
	
	for(i=0; i<4; i++)
	{
		A[i] = *pBYTE;

		pBYTE = pBYTE +1;
	}

	sprintf(buff, "%d.%d.%d.%d", A[0],A[1],A[2],A[3]);
	strcpy (sIPAddress, buff);

	return true;
}


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

	//*********************************************************************************
	//  函数名称 : IsNumericStr()
	//  功    能 :
	//			判断给定字符串是否识数字型字符串, 如: "1234"
	//  入口参数 :    
	//			char	*buf		给定字符串缓冲区
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		0	: 表示是数字型字符串
	//					-1	: 表示不是数字型字符串
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
	DLL_EXPORT int  IsNumericStr( char *buf ) 
	{
		unsigned int		i;

		if ( !buf || !*buf )
			return -1;

		for ( i = 0; i < strlen(buf); i ++ ) 
			if ( buf[i] < '0' || buf[i] > '9' )
				return -1;

		return 0;
	}


	//*********************************************************************************
	//  函数名称 : IsNumericStr2()
	//  功    能 :
	//			判断给定字符串是否带分隔符的数字型字符串, 如: "12,34"
	//  入口参数 :    
	//			char	*buf		给定字符串缓冲区
	//			char	splitchar	分隔符
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		0	: 表示是数字型字符串
	//					-1	: 表示不是数字型字符串
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2004.08.12
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
	DLL_EXPORT int  IsNumericStr2( char *buf, char splitchar ) 
	{
		unsigned int		i;

		if ( !buf || !*buf )
			return -1;

		for ( i = 0; i < strlen(buf); i ++ ) 
			if ( ( buf[i] < '0' || buf[i] > '9' ) && buf[i] != splitchar )
				return -1;

		return 0;
	}


	//*********************************************************************************
	//  函数名称 : IsNumericStr2()
	//  功    能 :
	//			判断给定字符串是否带分隔符的数字型字符串, 如: "12,34"
	//  入口参数 :    
	//			char	*buf		给定字符串缓冲区
	//			char	splitchar	分隔符
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		0	: 表示是数字型字符串
	//					-1	: 表示不是数字型字符串
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2004.08.18
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
	DLL_EXPORT int  NumericStrSort( char * buf, char splitchar )
	{
		if ( IsNumericStr2( buf, splitchar ) < 0 )
			return -1;

		

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : ReplaceSubStr()
	//  功    能 :
	//			替换源字符串中的指定子串1为目标子串2
	//  入口参数 : 
	//			char	*buf			源字符串
	//			char	*substr1		将被替换的子串
	//			char	*substr2		要替换的字符串
	//			char	buflen			源字符串的缓冲区大小
	//  出口参数 : 
	//			char	*buf			替换后的源字符串
	//  返 回 值 : 
	//			int		-1 替换失败, 0 替换成功
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.06.18
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		如果替换后的字符串长度超过源字符串的缓冲区大小,系统自动截断
	//		buflen 如果小于或等于0, 则返回-1
	//		如果buf或substr1为空串, 则返回-1
	//		若substr2为NULL或'\0'开头, 则将substr1从源串中剔除
	//
	//		不能在VB 6.0 下调用。再 VB 6.0 中相应的替换函数
	//
	//	维护说明 :
	//
	//		Standard .h :
	//			winnls.h		MultiByteToWideChar()
	//
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//			
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//
	//		其他调用该函数的库及函数名:
	//			SysMng.dll-------------GetEnvVar()
	//
	//*********************************************************************************
	//
	DLL_EXPORT int  ReplaceSubStr( char *buf, char *substr1, char *substr2, unsigned int buflen ) 
	{
		unsigned int		i, j, k;
		unsigned int		sublen, len;
		char				*tmpbuf;

		if ( !buf || !*buf || !substr1 || !*substr1 || buflen <= 0 )
			return -1;

		sublen = strlen(substr1);
		len = strlen(buf);

		tmpbuf = (char *)malloc(buflen);
		memset( tmpbuf, 0, buflen );

		k = 0;

		// 遍历一变源串
		for ( i = 0; i < len; i ++ ) {
			if ( buf[i] == substr1[0] ) {
				if ( (len - i) < sublen ) {			// 如果剩余的部分比substr1短, 表示没有需要替换的子串了替换结束
					memcpy( tmpbuf+k, buf+i, len-i );

					memset(buf, 0, buflen);
					memcpy(buf, tmpbuf, k+(len-i) );

					free(tmpbuf);

					return 0;
				}
				if ( memcmp( buf+i, substr1, strlen(substr1) ) == 0 ) { // 找到需要被替换的子串
					if ( !substr2 || !*substr2 ) {	// 若substr2为空串(包括NULL 或 以'\0'结尾的串), 表示删除substr1
						i += sublen-1;			// 直接跳过substr1
					} else {						// 若substr2不为空串(包括NULL 或 以'\0'结尾的串)
						for ( j = 0; j < strlen(substr2); j ++ ) {
							if ( k >= buflen ) {		// 若已经达到长度极限
								tmpbuf[k] = substr2[j];

								memset(buf, 0, buflen);
								memcpy(buf, tmpbuf, k+(len-i) );

								free(tmpbuf);

								return 0;
							} else {
								tmpbuf[k] = substr2[j];
								k ++;
							}
						}
						i += sublen-1;			// 跳过substr1
					}
				} else {	// 若没找到需要被替换的子串
					tmpbuf[k] = buf[i];
					k ++;
				}
			} else {
				tmpbuf[k] = buf[i];
				k ++;
			}
		}

		memset(buf, 0, buflen);
		memcpy(buf, tmpbuf, k );

		free(tmpbuf);

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : MaskSubStr()
	//  功    能 :
	//			屏蔽源字符串中的指定子串1为目标子串2
	//  入口参数 : 
	//			char	*buf			源字符串
	//			char	*SubStr			将被替换的子串
	//			char	maskchar		屏蔽字符
	//  出口参数 : 
	//			char	*buf			屏蔽后的源字符串
	//  返 回 值 : 
	//			char	*				屏蔽后的源字符串
	//  Author   :
	//			Adams
	//  Create Date :
	//			2004.10.27
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//			maskchar 不能使'\0'. 如果想 屏蔽 substr 及其后面的字符, 可以通过简单编成
	//		(通过调用strstr())实现.
	//
	//		不能在VB 6.0 下调用。在 VB 6.0 中相应的替换函数
	//
	//	维护说明 :
	//
	//		Standard .h :
	//
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//			
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//
	//		其他调用该函数的库及函数名:
	//			SysMng.dll------------- GetEnvVar()
	//			Astring.dll ----------- ReplaceSubStr()
	//
	//*********************************************************************************
	//
	DLL_EXPORT int  MaskSubStr( char *buf, char *substr, char maskchar )
	{
		int			sublen = 0, i=0;
		char		tmpbuf[ BUFSIZ + 1 ];

		if ( !buf || !*buf || !substr || !*substr || maskchar == '\0' )
			return -1;

		sublen = strlen( substr );
	

		memset( tmpbuf, 0, BUFSIZ+1 );
		for ( i = 0; i < sublen; i ++ ) 
			tmpbuf[i] = maskchar;

		ReplaceSubStr( buf, substr, tmpbuf, strlen(buf) );

		return 0;

	}
	

	// 以下为中文处理 Start {

	//*********************************************************************************
	//  函数名称 : IsChinese()
	//  功    能 :
	//        判断源串是否中文字符串
	//  入口参数 :    
	//		无
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		1	真, 是中文字符串
	//		0	假, 不是中文字符串
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2001.12.13
	//  Modify history (modifier and date) :
	//          
	//  使用说明 :
	//		VB 6.0 调用格式:
	//			Private Declare Function IsChinese Lib "Astring.dll" (ByVal str As String) As Long
	//				或
	//			Private Declare Function IsChinese Lib "Astring.dll" (ByVal str As String) As Integer
	//			需要注意的是,  str 是入口参数, VB 6.0 声明中, 用 ByVal, 另外, 调用前, 必须申请足够的空间
	//			可以用如下方式申请:
	//				str = String$(4, " ")
	//  Standard .h  : 
	//           
	//  Develope Tools: 
	//			  Visual C++ 6.0 /StuSP5
	//  Environment: 
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT int  IsChinese( const char *pszSource )
	{
		for (int i=0; i<(int)strlen(pszSource); i++) {
			if (pszSource[i] < 0)
				return 1;		// 是中文字符串
		}
		
		return 0;		// 不是中文字符串
	}

	//*********************************************************************************
	//  函数名称 : nAlphaOfStr()  
	//  功    能 :          
	//			计算字符串中ASCII码字符个数，主要是处理中文字符串
	//  入口参数 : 
	//		char		*str	: 子字符串缓冲区
	//  出口参数 :                      
	//		无
	//  返 回 值 :                      
	//		int				字符串中ASCII码字符个数		
	//  Author   :                      
	//		Adams                 
	//  Create Date :                   
	//		2001.12.14            
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
	DLL_EXPORT int  nAlphaOfStr( const char *str )
	{
		int		nAlpha = 0;
	
		for ( unsigned int i=0; i < strlen( str ); i ++ )	{
			if ( str[i] < 0 ){		// 中文, 跳过
				i ++; 
				continue;
			}
			nAlpha ++;
		}
		
		return nAlpha;
	}

	//*********************************************************************************
	//  函数名称 : cstrlen()
	//  功    能 :
	//			得到中英文混排的字符个数, 中文与西文一样, 也算1个字符
	//  入口参数 :
	//		const char	*pszSource		源字符串
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int		字符个数, 中文与西文一样, 也算1个字符
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2001.12.13            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串必须有尾0.
	//			半个汉字也当一个汉字来计数
	//  Standard .h :
	//                    
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT int  cstrlen( const char *pszSource )
	{
		int		nSourceLen = strlen(pszSource);			// 发送源的长度
		int		nStrLen = 0;							// 用来计算中文字符个数
		int		nAlpha = 0;								// 用来计算西文字符个数
	
		for (int i=0;i<nSourceLen;i++)	{			
			if ( pszSource[i] < 0 ){							// 测试是否为中文
				i ++;		
				nStrLen ++;
			}else {
					nAlpha ++;						// 非中文字符个数
			}
		}

		return nStrLen + nAlpha;
	}

	//*********************************************************************************
	//  函数名称 : cstrcpy()
	//  功    能 :
	//			得到中英文混排的字符串中指定长度范围内最长的子串, 长度为字节计数
	//  入口参数 :
	//		const char	*pszSource		源字符串
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int		实际取得的子串字符个数,  按字节计数
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2002.03.14            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串pszSource必须有尾0. 
	//			目标串必须已经申请好内存空间, 不得小于nSize, 并且用memset()等作过初始化.
	//			对于子串最后一个字符为半个汉字的情况, 丢弃该半个汉字.
	//			若中间碰到半个汉字的情况, 会中止(这种情况好向不会发生)
	//  Standard .h :
	//                    
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT unsigned int  cstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize )
	{
		const char		* pt = pszSource + nPos;
		char			ch[3];
		int				ret;
		unsigned int	len=0;

		if ( strlen(pt) <= (unsigned int)nSize ) {
			memcpy( pszDist, pt, strlen(pt) );
			return strlen(pt);
		}

		while ( *pt && pszDist  ) { // 源串没有结束, 目标串有效
			memset( ch, 0, 3 );
			ret = GetFirstWord( pt, ch );
			
			if ( ret == 0 ) {		// 合法单字, 可能是纯ASCII码字符或中文
				len += strlen( ch );

				pt = pszSource + nPos + len;

				if ( len <= (unsigned int)nSize )
					sprintf( pszDist + strlen(pszDist), "%s", ch );
				else
					break;
			}else		// 半个汉字
				break;
		}

		return len;
	}

	//*********************************************************************************
	//  函数名称 : mstrcpy()
	//  功    能 :
	//			得到中英文混排的字符串中指定长度范围内最长的子串, 长度为个数计数
	//  入口参数 :
	//		const char	*pszSource		源字符串
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int		实际取得的子串长度,  按个数计数，中文与ASCII字符一样占用一个计数单位
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2002.03.14            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			要求源串pszSource必须有尾0. 
	//			目标串必须已经申请好内存空间, 不得小于 nSize*2 Bytes, 并且用memset()等作过初始化.
	//			不存在半个汉字的情况
	//			
	//  Standard .h :
	//                    
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT unsigned int  mstrcpy( char *pszDist, const char *pszSource, int nPos, int nSize )
	{
		const char		* pt = pszSource + nPos;
		char			ch[3];
		int				ret;
		unsigned int	len=0;

		if ( cstrlen(pt) <= nSize ) {
			memcpy( pszDist, pt, strlen(pt) );
			return cstrlen(pt);
		}

		while ( *pt && pszDist  ) { // 源串没有结束, 目标串有效
			memset( ch, 0, 3 );
			ret = GetFirstWord( pt, ch );

			if ( ret == 0 ) {		// 合法单字, 可能是纯ASCII码字符或中文
				len ++;
				
				pt += strlen( ch );

				if ( len <= (unsigned int)nSize )
					sprintf( pszDist + strlen(pszDist), "%s", ch );
				else
					break;
			}else					// 半个汉字
				break;
		}

		return len;
	}

	//*********************************************************************************
	//  函数名称 : GetFirstWord()  
	//  功    能 :
	//        得到中第一个汉字 或 英文第一个字母
	//  入口参数 :
	//			无
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			无
    //  Author   :
	//			Adams
	//  Create Date :
	//			2001.12.13
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//
	//  Standard .h  :
	//
	//  Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	DLL_EXPORT int  GetFirstWord( const char *pszSource, char *deststr )
	{
		char	buf[3];

		memset( buf, 0, 3 );

		if ( !pszSource || !*pszSource ) 
			return -1;
		
		if ( pszSource[0] < 0 && strlen(pszSource) == 1 ) {
			deststr[0] = pszSource[0];				// 源字符串只有半个汉字
			return -1;
		}
		
		if ( pszSource[0] < 0 && strlen(pszSource) > 1 ) 
			memcpy( buf, pszSource, 2 );		// 中文
		else
			buf[0] = pszSource[0];				// ASCII码字符

		memcpy( deststr, buf, 2 );

		return 0;
	}

	// 以上为中文处理 End  }
	


