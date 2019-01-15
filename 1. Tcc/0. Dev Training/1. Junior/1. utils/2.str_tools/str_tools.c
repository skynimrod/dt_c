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
    //      37. isNumberStr()            ---  ���Ŀ���ַ����Ƿ��Ǵ������ַ���, ����: "1234"
    //
    // ��ʷ		:
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
    //  �������� : ltrim()
    //  ��    �� :
    //			ɾȥԴ������߿ո�
    //  ��ڲ��� :
    //            str : �ַ���
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���
    //  Author :
    //            Adams
    //  Create Date :
    //            1999.07.19
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			���ú�Դ������, �����Ŀո񲻴���, ֻ����ͷ���Ŀո���Tab��            
    //	Standard .h  :
    //            string.h--strlen()        
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
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
    //  �������� : mtrim()
    //  ��    �� :
    //             ɾȥԴ���м�����Ŀո���Tab��      
    //  ��ڲ��� :
    //            str : �ַ���          
    //  ���ڲ��� :
    //            ��                    
    //  �� �� ֵ :
    //            char  * : Ŀ���ַ���  
    //  Author   :
    //            Adams                 
    //  Create Date :
    //            1999.07.19            
    //  Modify history (modifier and date) :
    //                                      
    //  ʹ��˵�� :
    //			���ú�Դ����Ŀ�괮��ͬ, �����Ŀո񲻴���, ���ַ�����ͷ�����Ŀո���Tab��������
    //			ϵͳ�ܴ��������ַ�������Ϊ1024�ֽ�
    //  Standard .h  :
    //            string.h--strlen()        
    //  Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //  Environment:
    //           1. SCO UNIX 5.0.4          
    //           2. Windows NT WorkStation SimpleChinese 4.0 
    //              Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    DLL_EXPORT char * mtrim( char * str )  
    {
    	unsigned int	i, j, k, m, n;
    	char			tmpstr[1024];
    
    	if ( !str || !*str )
    		return(NULL);
    	
        i = 0;  j = strlen(str)-1; k = 0; m = 1;
    	
        // i Ϊ��һ�����ǿո���ַ�λ��
    	// j Ϊ������һ�����ǿո���ַ�λ��
    	// k Ϊ�ַ����м�ո����Ŀ
    	
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
	//  �������� : rtrim()
	//  ��    �� :
	//             ɾȥԴ�����ұ߿ո�      
	//  ��ڲ��� :
	//            str : �ַ���          
	//  ���ڲ��� :
	//            ��                    
	//  �� �� ֵ :
	//            char  * : Ŀ���ַ���  
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			���ú�Դ����Ŀ�괮��ͬ, �����Ŀո񲻴���, ֻ�����β���Ŀո���Tab��            
	//  Standard .h  :
	//            string.h--strlen()        
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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
	//  �������� : UpperStr()
	//  ��    �� :
	//             ��Դ�ַ����е�Сд��ĸ��Ϊ��д��ĸ
	//  ��ڲ��� :
	//			str : �ַ���ָ��(���ܺ���Сд��ĸ)
	//  ���ڲ��� :
	//			str : �ַ���ָ��(��ĸȫΪ��д)
	//  �� �� ֵ :
	//			char  * : Ŀ���ַ���(��ĸȫΪ��д)
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			���ú�Դ����Ŀ�괮��ͬ
	//  Standard .h  :
	//			1. string.h--strlen()
	//			2. ctype.h--islower()��toupper()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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
	//  �������� : LowerStr()
	//  ��    �� :
	//             ��Դ�ַ����еĴ�д��ĸ��ΪСд��ĸ
	//  ��ڲ��� :
	//			str : �ַ���ָ��(���ܺ��д�д��ĸ)
	//  ���ڲ��� :
	//			str : �ַ���ָ��(��ĸȫΪСд)
	//  �� �� ֵ :
	//			char  * : Ŀ���ַ���(��ĸȫΪСд)
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			���ú�Դ����Ŀ�괮��ͬ
	//  Standard .h  :
	//			1. string.h--strlen()
	//			2. ctype.h--islower()��toupper()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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
	//  �������� : GetQuotePos()
	//  ��    �� :
	//			���Դ�ַ�����ָ�������ַ���λ��
	//  ��ڲ��� :
	//		char	*str	�ַ���ָ��(���ܺ��д�д��ĸ)
	//		char	quote	ָ�����ŵ�ֵ, ���������, 
	//		int		*i		����ָ��,������ŵ�һ�����ŵ�λ��
	//		int		*j		����ָ��,������ŵڶ������ŵ�λ��(����ڵ�һ�����ŵ�λ��)
	//  ���ڲ��� :
	//		int		*i		����ָ��,������ŵ�һ�����ŵ�λ��
	//		int		*j		����ָ��,������ŵڶ������ŵ�λ��(����ڵ�һ�����ŵ�λ��)
	//  �� �� ֵ :
	//		int		: 0 ��ʾ�ɹ�
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			���������ָ�κ�һ��ASCII�ַ������Կ�����һ������, ���������й�
	//			�����Ų�ƥ��: i=��һ�����ŵ�ʵ��λ��, j=0
	//			��������    : i=0, j=0
	//			i��0��ʼ, j�����i��0��ʼ
	//			��: "aaa\"ccc\"bbb", i = 3,j=4
	//
	//			�����з�ָ������, ������0�����Բ��ü�鷵��ֵ
	//  Standard .h  :
	//		1. string.h--strlen()��strchr()��memset()��memcpy
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT int  GetQuotePos( char *str, char quote, int *i, int *j )
	{
		char	tmpstr[256];

	
		if ( strchr(str, quote) == NULL) {
			*i = 0;    *j = 0;    
			return( 0 );
		}

		*i = strlen(str) - strlen( strchr(str, quote) );	/* ���ż�λ�� */

		// tmpstrΪ��һ�����ź���ַ���
		memset( tmpstr, 0, sizeof(tmpstr) );
		memcpy( tmpstr, strchr(str,quote)+1, strlen(strchr(str,quote)) - 1 );

		// j Ϊ�����i�ڶ������ŵ�λ��
		if ( strchr(tmpstr, quote) == NULL ) 
			*j = 0;
		else
			*j = strlen( tmpstr)-strlen(strchr(tmpstr, quote) )+1;

		return 0;
	}

	//**************************************************************************
	//  �������� : CutMemo()
	//  ��    �� :
	//			ɾȥԴ�ַ�����ָ��ɾ����ʶ��(ע�ͷ�)�������ַ�
	//  ��ڲ��� :
	//		char	*str     : Դ�ַ���ָ��
	//		char	CutChar : �ַ�, ע�ͷ�
	//  ���ڲ��� :
	//			str : �ַ���ָ��, �Ѿ�ɾ����ע��
	//  �� �� ֵ :
	//			char  * : Ŀ���ַ���
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            1999.07.19            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			����\'��\"��Ϊ����,ϵͳʹ��,��������ע�ͷ�,��Ϊ#��ʱ����Ϊ�����ַ�
	//			��ע�ͷ�Ϊ�Ƿ�('),Դ�ַ�������,����NULL
	//			��ע�ͷ�������(')(����)��,Դ�ַ�������,����NULL
	//
	//			���ú�Դ����Ŀ�괮��ͬ
	//
	//  Standard .h  :
	//		1. string.h--strlen()��strchr()
	//	User .h : 
	//		1. Astring.h--GetQuotePos()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
	//	��  ��  : 
	//			�ƺ�����ɾ�������ڵ�����
	//***************************************************************************
	//
	DLL_EXPORT char *	CutMemo( char	*str, char CutChar )
	{
		int		i, j, k;
		char	*tmpstr;
		int		pos, len;

		if ( CutChar == '\'' || CutChar == '\"' ) 		// ע�ͼ�����Ϊ\' �� \"
			return( NULL );

		if ( strchr(str,CutChar) == NULL ) 
			return( str );

		pos = 0; len = strlen( str );
		tmpstr = str;

		while ( pos <= len ) {
			//	ע�ͼ�λ��
			if (strchr( tmpstr, CutChar ) != NULL ) 
				k = strlen( tmpstr) - strlen(strchr(tmpstr, CutChar) );	
			else
				k = strlen( tmpstr );

			// ����λ�� 
			GetQuotePos( tmpstr,'\'', &i, &j);

			// ע��λ��������λ��ǰ��,���Ų�ƥ�� 
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
	//  �������� : CutMemoEx()  
	//  ��    �� :
	//			ɾȥԴ�ַ�����ָ��ɾ����ʶ��(ע�ͷ�)�������ַ�, CutMemo()����չ
	//  ��ڲ��� : 
	//		char	*str     : Դ�ַ���ָ��
	//		char	*remstr : �ַ���, ע�ͷ�
	//  ���ڲ��� : 
	//			str : �ַ���ָ��, �Ѿ�ɾ����ע��
	//  �� �� ֵ : 
	//			char  * : Ŀ���ַ���
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.07.01            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			����\'��\"��Ϊ����,ϵͳʹ��,��������ע�ͷ�,��Ϊ#��ʱ����Ϊ�����ַ�
	//			��ע�ͷ�Ϊ�Ƿ�('),Դ�ַ�������,����NULL
	//			��ע�ͷ�������(')(����),Դ�ַ�������,����NULL
	//
	//			���ú�Դ����Ŀ�괮��ͬ
	//
	//		��Ҫע�����, ע�͵��������ڴ�����Ȼ����, ֻ�Ǳ�β��ض�
	//
	//		VB 6.0 ���ø�ʽ:
	//			Private Declare Function CutMemoEx Lib "Astring.dll" (ByVal buf As String, ByVal str As String) As String
	//
	//		��Ҫע�����, VB�е������, (������ʾ������,��ʵ�Ѿ��������)���¸�ֵ�ſ��Կ��������Ľ��
	//
	//		����:
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
	//	ά��˵�� :
	//
	//		Standard .h :
	//			string.h ------ strstr(), strlen() 
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		�������øú����Ŀ⼰������:
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
			//	ע�ͼ�λ��
			if (strstr( tmpstr, remstr ) != NULL ) 
				k = strlen( tmpstr) - strlen(strstr(tmpstr, remstr) );	
			else
				k = strlen( tmpstr );

			// ����λ�� 
			GetQuotePos( tmpstr,'\'', &i, &j);

			// ע��λ��������λ��ǰ��,���Ų�ƥ�� 
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
	//  �������� : IfHaveQuote()
	//  ��    �� :
	//			�ж�Դ�ַ������Ƿ����ָ��ƥ������
	//  ��ڲ��� :
	//			char	*str		�ַ���ָ��
	//			char	Quote		�ַ�, ���������ַ�
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int : 0: ���� -1:������
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0, ���ú�Դ������
	//  Standard .h  :
	//		��GetQuotePos()Ҫ��ı�׼ͷ�ļ���ͬ
	//	User .h : 
	//		1. Astring.h--GetQuotePos()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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
	//  �������� : CutQuote()
	//  ��    �� :
	//			ɾȥԴ�ַ����е�ָ��ƥ������
	//  ��ڲ��� :
	//			char	*str		�ַ���ָ��
	//			char	Quote		ָ���ַ�, ���������ַ�
	//  ���ڲ��� :
	//			char	*str		�ַ���ָ��(����ָ���ַ�)
	//  �� �� ֵ :
	//			char  * : Ŀ���ַ���(����ָ���ַ�)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0, ���ú�Դ������
	//  Standard .h  :
	//		��GetQuotePos()Ҫ��ı�׼ͷ�ļ���ͬ
	//	User .h : 
	//		1. Astring.h--GetQuotePos()��ltrim()��rtrim()��mtrim()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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
	//  �������� : DelCharInString()
	//  ��    �� :
	//			ɾȥԴ�ַ����е�ָ���ַ�
	//  ��ڲ��� :
	//			char	*str		�ַ���ָ��(���ܺ���ָ����ĸ)
	//			char	ch			ָ���ַ�
	//  ���ڲ��� :
	//			char	*str		�ַ���ָ��(����ָ���ַ�)
	//  �� �� ֵ :
	//			char  * : Ŀ���ַ���(����ָ���ַ�)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0, ���ú�Դ������
	//			Ҫ��Դ�����벻�ǳ���
	//  Standard .h  :
	//		1. string.h--strcpy()
	//	User .h : 
	//		1. Astring.h--GetQuotePos()��ltrim()��rtrim()��mtrim()
	//  Develope Tools:
	//			  SCO UNIX Development CC   
	//			  Visual C++ 6.0            
	//  Environment:
	//           1. SCO UNIX 5.0.4          
	//           2. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
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

				while (str[k]==ch)	// �����Һ����Ƿ�����Ҫɾ�����ַ�
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
	//  �������� : ch_monfmt()
	//  ��    �� :
	//			�������������ַ���תΪ��д����ַ���
	//  ��ڲ��� :
	//		char	*out	��д����ַ���(����λ, ��:ʮԪ5��)
	//		char	*in		����������ַ���(�����ָ���, Ҳ������λ, ��:1005, ��ʾ10.05Ԫ)
	//		int		ilen	in�ĳ���
	//  ���ڲ��� :
	//		char	*out	��д����ַ���(����λ, ��:ʮԪ5��)
	//  �� �� ֵ :
	//		char	*out	��д����ַ���(����λ, ��:ʮԪ5��)
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			1999.08.05       
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0, ���ú�Դ������
	//			Ҫ��Դ�����벻�ǳ���
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
	//              Can be used in console��MFC Application for Windows
	//***************************************************************************
	//
	char	chmoney[10][3] = {
		"��",	"Ҽ",	"��",	"��",	"��",	"��",	"½",	"��",	"��",	"��"
	};

	char	chunit[3][3] = {
		"ʰ",	"��",	"Ǫ"
	};
	
	char	chmisc[4][5] = {
		"Ԫ",	"��",	"��",   "����"
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
			strcat(tmpfmt, "��");
			tag = 0;
		} else {
			if ( in[ilen-1] != '0' ) {
				strcat(tmpfmt, "��");
				strcat(tmpfmt, chmoney[in[i] - '0']);
				tag = 1;
			}
		}
	
		i --;
	
		if (i >= j)
			if (in[i] == '0') {
				if (tag == 1) 
					if (i-1>=j) {
						strcat(tmpfmt, "��");
						flag0 = 1;
					}
			} else {
				strcat(tmpfmt, "��");
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
									strcat(tmpfmt, "��");
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
									strcat(tmpfmt, "��");
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
							strcat(tmpfmt, "��");
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
	//  �������� : StrNumAdd()
	//  ��    �� :
	//			����λ�򲻴���λ�������������ַ����ӷ�
	//  ��ڲ��� :
	//		char	*NumStr1	: �����������ַ���1, ����          
	//		char	*NumStr2	: �����������ַ���2, ������ 
	//		char	*deststr	: �����������ַ���, ���ֺ͵��ַ���
	//		int		flag		: ��ʶ��1: ��ʾ ����λ �ļӷ�
	//									2: ��ʾ ������λ �ļӷ�, ȱʡֵ
	//  ���ڲ��� :
	//		char	*deststr	: �����������ַ���, ���ֺ͵��ַ���
	//  �� �� ֵ :
	//		char  * : �����������ַ���, ���ֺ͵��ַ���
	//  Author   :
	//			Adams                 
	//  Create Date :
	//			2001.11.05       
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0, ���ú�Դ������
	//			Ҫ��Դ�����벻�ǳ���
	//			flag	= 1: ��ʾ ����λ �ļӷ�
	//					  2: ��ʾ ������λ �ļӷ�, ȱʡֵ
	//  Standard .h  :
	//		1. string.h--strcpy()
	//	User .h : 
	//		1. 
	//  Develope Tools:
	//			  Visual C++ 6.0
	//  Environment:
	//           1. Windows NT WorkStation SimpleChinese 4.0 
	//              Can be used in console��MFC Application for Windows
	//***************************************************************************
	//
	DLL_EXPORT char *	StrNumAdd(char *NumStr1, char *NumStr2, char *deststr, int flag )
	{
		char	maxstr[15+1], minstr[15+1];		// ��ʾ֧�����Ϊ����������
		int		i, j, k;
     
		if ( !NumStr1 || !*NumStr1 || strlen(NumStr1) > 15 || 
			 !NumStr2 || !*NumStr2 || strlen(NumStr2) > 15 )
			return NULL;                //�������Դ������һ����NULL��մ�,�򷵻�NULL

		i = 0;
		while (NumStr1[i] != '\0') {      //�������Դ������һ�������������ַ�,�򷵻�NULL
			if ( NumStr1[i] < '0' || NumStr1[i] > '9' ) 
				return NULL; 

			i ++;
		}
	
		i = 0;
		while (NumStr2[i] != '\0') {      //�������Դ������һ�������������ַ�,�򷵻�NULL
			if ( NumStr2[i] < '0' || NumStr2[i] > '9' ) 
				return NULL; 

			i ++;
		}


		if ( 1 == flag ) {		// ��ʾ ����λ �ļӷ�
			sprintf( deststr, "%d", atoi(NumStr1) + atoi(NumStr2) );
			return( deststr );
		}

		if ( 2 == flag ) {		// ��ʾ ������λ �ļӷ�
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
	//  �������� : RemoveCrLf()  
	//  ��    �� :
	//			ɾ�����뻺�����Ļس���/����
	//  ��ڲ��� :
	//			LPSTR	instr :	Null ��β���ַ���(����β0)
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			��
	//  Author   :
	//			Adams
	//  Create Date :
	//			2001.12.11
	//  Modify history (modifier and date) :
	//			
	//  ʹ��˵�� :
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
	//  �������� : Space()  
	//  ��    �� :
	//			����ָ�������ո���ַ���
	//  ��ڲ��� :
	//			int		num		ָ���Ŀո����
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			����ָ�������ո���ַ���
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.04.29
	//  Modify history (modifier and date) :
	//			
	//  ʹ��˵�� :
	//			num����С��1024�����򷵻�NULL
	//  Standard .h  :
	//			string.h--strlen()        
	//  Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//  Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	#define MAXSPACE	1024		// Space()֧�ֵ����ո���
	
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
	//  �������� : lpatoi()  
	//  ��    �� :
	//			atoi()ֻ�ܹ�����NEAR host ����, �ڴ���Ӧ�����޷�ʹ�á�
	//			�ú���ͨ�����������ַ������������ر�����"ƭ��"atoi()��Ȼ����ת����
	//  ��ڲ��� :
	//		char	* instr	: ����������ַ���, ����ܳ���35λ
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int :	instr������ֵ
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//			�ú����ǳ����ã������Ǳ���Ҫ��
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
	//  �������� : GetFirstEnWord()  
	//  ��    �� :
	//			��һ���ַ����л�ȡ��һ��Ӣ�ĵ���
	//  ��ڲ��� :
	//		char	* instr	: ����������ַ���, ����ܳ���35λ
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int :	instr������ֵ
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//			�ú����ǳ����ã������Ǳ���Ҫ��
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
	//  �������� : ValidIpstr()
	//  ��    �� :
	//			���ָ���ַ����Ƿ�Ϊ�Ϸ���IP��ַ(IPv4)�ַ�����ʽ
	//  ��ڲ��� :    
	//			char	*IPstr		�����ַ���������
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		0 : ָ���ַ����ǺϷ���IP��ַ(IPv4)�ַ�����ʽ
	//					-1: ָ���ַ����ǷǷ���IP��ַ(IPv4)�ַ�����ʽ
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
				// ���б�Ҫ����Ҫ�ڴ��ж�k==1������£�aoti(buf)�Ƿ�=1
				// һ������£���ַ��һλ������0
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
	//  �������� : ValidIplong()
	//  ��    �� :
	//			���ָ���������Ƿ�Ϊ�Ϸ���IP��ַ(IPv4)�ڲ���ʽ
	//  ��ڲ��� :    
	//			long	IPlong			���쳤����
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		0 : ָ���������ǺϷ���IP��ַ(IPv4)�ڲ���ʽ
	//					-1: ָ���������ǷǷ���IP��ַ(IPv4)�ڲ���ʽ
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
	int  ValidIplong( long IPlong )
	{
		// ���б�Ҫ����Ҫ�ڴ��ж�IPlong�еĵ�һ���ֽ�����ֵ�Ƿ�=1
		// һ������£���ַ��һλ������0

		return 0;
	}

	//*********************************************************************************
	//  �������� : Ipstr2Iplong()
	//  ��    �� :
	//			���ַ�����IP��ַת��Ϊ�ڲ���ʽ��IP��ַ
	//  ��ڲ��� :    
	//			char		*ipstr			�ַ�����IP��ַ������
	//  ���ڲ��� :
	//			long		*iplong			�ڲ���ʽIP��ַ������
	//  �� �� ֵ :
	//			int		0 : ת���ɹ�
	//					-1: ת��ʧ��
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
	DLL_EXPORT int  Ipstr2Iplong( char *ipstr, long *iplong )
	{
		int			i, j, k ;
		char		tmpbuf[4];		// �Ϸ���IP�ַ���û�����Ϊ"255", 3+1
		char		buf[5];			// �������IP�ڲ���ʽ��ֵ 4 + 1
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
	//  �������� : Iplong2Ipstr()
	//  ��    �� :
	//			���ڲ���ʽ��IP��ַת��Ϊ�ַ�����IP��ַ
	//  ��ڲ��� :    
	//			long		*iplong			�ڲ���ʽIP��ַ������
	//  ���ڲ��� :
	//			char		*ipstr			�ַ�����IP��ַ������
	//  �� �� ֵ :
	//			int		0 : ת���ɹ�
	//					-1: ת��ʧ��
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2002.04.23
	//  Modify history (modifier and date) :
	//          
	//  ʹ��˵�� :
	//			����ǰ����Ϊipstr���뵽�㹻���ڴ�ռ䣬����0
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
		unsigned char	buf[5];			// �������IP�ڲ���ʽ��ֵ 4 + 1
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

	//*********************************************************************************
	//  �������� : IsNumericStr()
	//  ��    �� :
	//			�жϸ����ַ����Ƿ�ʶ�������ַ���, ��: "1234"
	//  ��ڲ��� :    
	//			char	*buf		�����ַ���������
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		0	: ��ʾ���������ַ���
	//					-1	: ��ʾ�����������ַ���
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
	//  �������� : IsNumericStr2()
	//  ��    �� :
	//			�жϸ����ַ����Ƿ���ָ������������ַ���, ��: "12,34"
	//  ��ڲ��� :    
	//			char	*buf		�����ַ���������
	//			char	splitchar	�ָ���
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		0	: ��ʾ���������ַ���
	//					-1	: ��ʾ�����������ַ���
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2004.08.12
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
	//  �������� : IsNumericStr2()
	//  ��    �� :
	//			�жϸ����ַ����Ƿ���ָ������������ַ���, ��: "12,34"
	//  ��ڲ��� :    
	//			char	*buf		�����ַ���������
	//			char	splitchar	�ָ���
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		0	: ��ʾ���������ַ���
	//					-1	: ��ʾ�����������ַ���
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2004.08.18
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
	DLL_EXPORT int  NumericStrSort( char * buf, char splitchar )
	{
		if ( IsNumericStr2( buf, splitchar ) < 0 )
			return -1;

		

		return 0;
	}

	//*********************************************************************************
	//  �������� : ReplaceSubStr()
	//  ��    �� :
	//			�滻Դ�ַ����е�ָ���Ӵ�1ΪĿ���Ӵ�2
	//  ��ڲ��� : 
	//			char	*buf			Դ�ַ���
	//			char	*substr1		�����滻���Ӵ�
	//			char	*substr2		Ҫ�滻���ַ���
	//			char	buflen			Դ�ַ����Ļ�������С
	//  ���ڲ��� : 
	//			char	*buf			�滻���Դ�ַ���
	//  �� �� ֵ : 
	//			int		-1 �滻ʧ��, 0 �滻�ɹ�
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.06.18
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		����滻����ַ������ȳ���Դ�ַ����Ļ�������С,ϵͳ�Զ��ض�
	//		buflen ���С�ڻ����0, �򷵻�-1
	//		���buf��substr1Ϊ�մ�, �򷵻�-1
	//		��substr2ΪNULL��'\0'��ͷ, ��substr1��Դ�����޳�
	//
	//		������VB 6.0 �µ��á��� VB 6.0 ����Ӧ���滻����
	//
	//	ά��˵�� :
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
	//		�������øú����Ŀ⼰������:
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

		// ����һ��Դ��
		for ( i = 0; i < len; i ++ ) {
			if ( buf[i] == substr1[0] ) {
				if ( (len - i) < sublen ) {			// ���ʣ��Ĳ��ֱ�substr1��, ��ʾû����Ҫ�滻���Ӵ����滻����
					memcpy( tmpbuf+k, buf+i, len-i );

					memset(buf, 0, buflen);
					memcpy(buf, tmpbuf, k+(len-i) );

					free(tmpbuf);

					return 0;
				}
				if ( memcmp( buf+i, substr1, strlen(substr1) ) == 0 ) { // �ҵ���Ҫ���滻���Ӵ�
					if ( !substr2 || !*substr2 ) {	// ��substr2Ϊ�մ�(����NULL �� ��'\0'��β�Ĵ�), ��ʾɾ��substr1
						i += sublen-1;			// ֱ������substr1
					} else {						// ��substr2��Ϊ�մ�(����NULL �� ��'\0'��β�Ĵ�)
						for ( j = 0; j < strlen(substr2); j ++ ) {
							if ( k >= buflen ) {		// ���Ѿ��ﵽ���ȼ���
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
						i += sublen-1;			// ����substr1
					}
				} else {	// ��û�ҵ���Ҫ���滻���Ӵ�
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
	//  �������� : MaskSubStr()
	//  ��    �� :
	//			����Դ�ַ����е�ָ���Ӵ�1ΪĿ���Ӵ�2
	//  ��ڲ��� : 
	//			char	*buf			Դ�ַ���
	//			char	*SubStr			�����滻���Ӵ�
	//			char	maskchar		�����ַ�
	//  ���ڲ��� : 
	//			char	*buf			���κ��Դ�ַ���
	//  �� �� ֵ : 
	//			char	*				���κ��Դ�ַ���
	//  Author   :
	//			Adams
	//  Create Date :
	//			2004.10.27
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//			maskchar ����ʹ'\0'. ����� ���� substr ���������ַ�, ����ͨ���򵥱��
	//		(ͨ������strstr())ʵ��.
	//
	//		������VB 6.0 �µ��á��� VB 6.0 ����Ӧ���滻����
	//
	//	ά��˵�� :
	//
	//		Standard .h :
	//
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//			
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//
	//		�������øú����Ŀ⼰������:
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
	

	// ����Ϊ���Ĵ��� Start {

	//*********************************************************************************
	//  �������� : IsChinese()
	//  ��    �� :
	//        �ж�Դ���Ƿ������ַ���
	//  ��ڲ��� :    
	//		��
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		1	��, �������ַ���
	//		0	��, ���������ַ���
    //  Author   :      
	//            Adams
	//  Create Date :
	//            2001.12.13
	//  Modify history (modifier and date) :
	//          
	//  ʹ��˵�� :
	//		VB 6.0 ���ø�ʽ:
	//			Private Declare Function IsChinese Lib "Astring.dll" (ByVal str As String) As Long
	//				��
	//			Private Declare Function IsChinese Lib "Astring.dll" (ByVal str As String) As Integer
	//			��Ҫע�����,  str ����ڲ���, VB 6.0 ������, �� ByVal, ����, ����ǰ, ���������㹻�Ŀռ�
	//			���������·�ʽ����:
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
				return 1;		// �������ַ���
		}
		
		return 0;		// ���������ַ���
	}

	//*********************************************************************************
	//  �������� : nAlphaOfStr()  
	//  ��    �� :          
	//			�����ַ�����ASCII���ַ���������Ҫ�Ǵ��������ַ���
	//  ��ڲ��� : 
	//		char		*str	: ���ַ���������
	//  ���ڲ��� :                      
	//		��
	//  �� �� ֵ :                      
	//		int				�ַ�����ASCII���ַ�����		
	//  Author   :                      
	//		Adams                 
	//  Create Date :                   
	//		2001.12.14            
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
	DLL_EXPORT int  nAlphaOfStr( const char *str )
	{
		int		nAlpha = 0;
	
		for ( unsigned int i=0; i < strlen( str ); i ++ )	{
			if ( str[i] < 0 ){		// ����, ����
				i ++; 
				continue;
			}
			nAlpha ++;
		}
		
		return nAlpha;
	}

	//*********************************************************************************
	//  �������� : cstrlen()
	//  ��    �� :
	//			�õ���Ӣ�Ļ��ŵ��ַ�����, ����������һ��, Ҳ��1���ַ�
	//  ��ڲ��� :
	//		const char	*pszSource		Դ�ַ���
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int		�ַ�����, ����������һ��, Ҳ��1���ַ�
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2001.12.13            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��������β0.
	//			�������Ҳ��һ������������
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
		int		nSourceLen = strlen(pszSource);			// ����Դ�ĳ���
		int		nStrLen = 0;							// �������������ַ�����
		int		nAlpha = 0;								// �������������ַ�����
	
		for (int i=0;i<nSourceLen;i++)	{			
			if ( pszSource[i] < 0 ){							// �����Ƿ�Ϊ����
				i ++;		
				nStrLen ++;
			}else {
					nAlpha ++;						// �������ַ�����
			}
		}

		return nStrLen + nAlpha;
	}

	//*********************************************************************************
	//  �������� : cstrcpy()
	//  ��    �� :
	//			�õ���Ӣ�Ļ��ŵ��ַ�����ָ�����ȷ�Χ������Ӵ�, ����Ϊ�ֽڼ���
	//  ��ڲ��� :
	//		const char	*pszSource		Դ�ַ���
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int		ʵ��ȡ�õ��Ӵ��ַ�����,  ���ֽڼ���
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2002.03.14            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��pszSource������β0. 
	//			Ŀ�괮�����Ѿ�������ڴ�ռ�, ����С��nSize, ������memset()��������ʼ��.
	//			�����Ӵ����һ���ַ�Ϊ������ֵ����, �����ð������.
	//			���м�����������ֵ����, ����ֹ(����������򲻻ᷢ��)
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

		while ( *pt && pszDist  ) { // Դ��û�н���, Ŀ�괮��Ч
			memset( ch, 0, 3 );
			ret = GetFirstWord( pt, ch );
			
			if ( ret == 0 ) {		// �Ϸ�����, �����Ǵ�ASCII���ַ�������
				len += strlen( ch );

				pt = pszSource + nPos + len;

				if ( len <= (unsigned int)nSize )
					sprintf( pszDist + strlen(pszDist), "%s", ch );
				else
					break;
			}else		// �������
				break;
		}

		return len;
	}

	//*********************************************************************************
	//  �������� : mstrcpy()
	//  ��    �� :
	//			�õ���Ӣ�Ļ��ŵ��ַ�����ָ�����ȷ�Χ������Ӵ�, ����Ϊ��������
	//  ��ڲ��� :
	//		const char	*pszSource		Դ�ַ���
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int		ʵ��ȡ�õ��Ӵ�����,  ������������������ASCII�ַ�һ��ռ��һ��������λ
    //  Author   :
	//            Adams                 
	//  Create Date :
	//            2002.03.14            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
	//			Ҫ��Դ��pszSource������β0. 
	//			Ŀ�괮�����Ѿ�������ڴ�ռ�, ����С�� nSize*2 Bytes, ������memset()��������ʼ��.
	//			�����ڰ�����ֵ����
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

		while ( *pt && pszDist  ) { // Դ��û�н���, Ŀ�괮��Ч
			memset( ch, 0, 3 );
			ret = GetFirstWord( pt, ch );

			if ( ret == 0 ) {		// �Ϸ�����, �����Ǵ�ASCII���ַ�������
				len ++;
				
				pt += strlen( ch );

				if ( len <= (unsigned int)nSize )
					sprintf( pszDist + strlen(pszDist), "%s", ch );
				else
					break;
			}else					// �������
				break;
		}

		return len;
	}

	//*********************************************************************************
	//  �������� : GetFirstWord()  
	//  ��    �� :
	//        �õ��е�һ������ �� Ӣ�ĵ�һ����ĸ
	//  ��ڲ��� :
	//			��
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			��
    //  Author   :
	//			Adams
	//  Create Date :
	//			2001.12.13
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
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
			deststr[0] = pszSource[0];				// Դ�ַ���ֻ�а������
			return -1;
		}
		
		if ( pszSource[0] < 0 && strlen(pszSource) > 1 ) 
			memcpy( buf, pszSource, 2 );		// ����
		else
			buf[0] = pszSource[0];				// ASCII���ַ�

		memcpy( deststr, buf, 2 );

		return 0;
	}

	// ����Ϊ���Ĵ��� End  }
	


