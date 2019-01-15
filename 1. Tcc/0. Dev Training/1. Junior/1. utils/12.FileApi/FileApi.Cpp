    //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.，LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// 模块		: FileApi.Cpp
	// 
	// 描述		: 标准公用代码API, 有关文件操作访问的应用接口函数, SDK版本, 可以用在许多工具中.
	//
	// 功能函数	:
	//		1.  GetFullPath()
	//
	//		21. GetTmpFile()
	//		22. IfFileExist()
	//		23. GetSizeOfFile()
	//		24. GetNoPathFileName()
	//
	//		41. GetDirectoryOfFile()  
	//		42. CreateDirOfFile()
	//		43. CreateDir() 
	//		44. IfDirectoryExist()
	//		45. CopyDir()
	//		46. ifSubDir()
	//		47. RemoveDir()
	//		48. GetCapacityOfDir()
	//
	// 历史		:
	//
	//***************************************************************************
	//
	//
    //
	#include	<stdio.h>
	#include	<time.h>
	#include	<string.h>
	#include	<windows.h>
	#include	<winbase.h>
	#include	<fcntl.h>
	#include	<io.h>
	

	#include	"FileApi.h"

	#define PrintAppStyleAPIError(ApiTxt,MsgTxt) {							\
		DWORD dwLastError;													\
		dwLastError = GetLastError();										\
		switch (dwLastError)  {												\
		case ERROR_FILE_NOT_FOUND :											\
			printf("\nFile not found (%s) line %d",MsgTxt,__LINE__);		\
			break;															\
		case ERROR_INVALID_NAME   :											\
			printf("\nInvalid name (%s) line %d",MsgTxt,__LINE__);			\
			break;															\
		case ERROR_PATH_NOT_FOUND :											\
			printf("\nError path not found (%s) line %d",MsgTxt,__LINE__);	\
			break;															\
		case ERROR_SHARING_VIOLATION :										\
			printf("\nSharing violation - shut down net and/or stop other sessions (%s) line %d",MsgTxt,__LINE__);	\
			break;															\
		case ERROR_ACCESS_DENIED  :											\
			printf("\nAccess denied (%s) line %d",MsgTxt,__LINE__);			\
			break;															\
		default                   :											\
			printf("\n" #ApiTxt " - unexpected return code=%d (%s) line %d",dwLastError,MsgTxt,__LINE__);			\
			break;															\
		}																	\
	}

	//*********************************************************************************
	//  函数名称 : GetFullPath()  
	//  功    能 :
	//			获取指定文件(或目录)的全路经名称
	//  入口参数 :
	//		char	*in				文件或路径名称，可能不含路径名称
	//  出口参数 :
	//		char	*out			文件或路径名称，含全路径名称
	//  返 回 值 :
	//		int		0 表示 成功; -1 表示 失败
	//  Author   :
	//		Adams                 
	//  Create Date :
	//		2002.06.03            
	//  Modify history (modifier and date) :
	//		
	//  使用说明 :
	//			调用前，out 必须申请足够的空间(MAX_PAHT+2), 并用memset()初始化为0, 必须将
	//		输出缓冲区的大小作为参数outsize传入.
	//		不检查盖目录是否存在
	//	维护说明 :
	//		由于上月硬盘坏, 该程序重写
	//		Standard .h :
	//            string.h--strlen()        
	//		Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int	__stdcall GetFullPath( char * in, char *out, int outsize )
	{
		DWORD			dwRet;
		LPTSTR			lpsz;

		if ( !in || !*in )
			return -1;

		dwRet = GetFullPathName( in,
                    (DWORD)outsize,
                    (LPTSTR)out,
                    (LPTSTR *)&lpsz);		// 最后一个参数用来取in的最后一部分, 本函数中无用
		if ( 0 == dwRet )
			switch ( GetLastError() ) {
			case ERROR_INVALID_NAME   :
				printf("\nError invalid file full-name (on GetFullPathName)");
				return -1;
			default                   :
				//PERR("GetFullPathName - unexpected return code");
				return -1;
			}

		if ( dwRet > (DWORD)outsize ) { 
			//PERR("GetFullPathName - buffer too small");
			return -1;
		}

		lpsz = CharLower((LPTSTR)out);

		if (!lpsz) { 
			//PERR("CharLower failure");
			return -1;
		}

		return 0;
	}

	// 以下是对文件的访问操作		Start {

	//*********************************************************************************
	//  函数名称 : GetTmpFile()  
	//  功    能 :
	//			获取临时文件名称
	//  入口参数 :
	//		int		i				文件编号
	//		char	* tmpfile
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            2001.12.11            
	//  Modify history (modifier and date) :
	//                                      
	//  使用说明 :
	//			
	//  Standard .h :
	//            string.h--strlen()        
	//  Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//  Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int __stdcall GetTmpFile( char	*tmpfile )
	{
		time_t	now;
		struct tm *ts;

		time(&now);
		ts = localtime(&now);

		sprintf( tmpfile, "tmp\\~%4d%02d%02d%02d%02d%02d", ts->tm_year+1900,
			ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec );

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : IfFileExist()  
	//  功    能 :
	//		判断指定文件是否存在
	//  入口参数 :
	//		char	* szFileName		:	指定文件名(可能是相对路径)
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		bool	true : 表示指定文件存在, false : 表示指定文件不存在
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.04
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		
	//	维护说明 :
	//		
	//		Standard .h :
	//            string.h--strlen()
	//			winbase.h--------GetCurrentDirectory()
	//		Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	bool __stdcall IfFileExist( char * szFileName )
	{
		OFSTRUCT		of;

		if ( !szFileName || !*szFileName )
			return false;

		if ( OpenFile( szFileName, &of, OF_EXIST ) == -1 ) {	// 打开后自动关闭，用来监测制定文件是否存在
			return false;
		}

		return true;
	}

	//*********************************************************************************
	//  函数名称 : GetSizeOfFile()  
	//  功    能 :
	//			获取指定文件的大小(字节数)
	//  入口参数 :
	//		char	* szFileName		:	指定文件名(可能是相对路径)
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		unsigned long	0xFFFFFFFF : 表示失败(-1)
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.04
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		
	//	维护说明 :
	//		是系统函数GetFileSize()的一个应用, 主要是为了简化调用
	//		假设制定的文件大小均在2的32次方Byte之内，即小于2的12次方MB=4GB
	//		偌处理大于4GB的文件，需要修正该函数，将GetFileSize()的第二个参数进行利用
	//		Standard .h :
	//            string.h--strlen()
	//			winbase.h--------GetCurrentDirectory()
	//		Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	unsigned long __stdcall GetSizeOfFile( char * filename )
	{
		DWORD			num = 0;
		HANDLE			hFile;

		if ( !IfFileExist(filename) )
			return 0xFFFFFFFF;


		// CreateFile用来打开文件
		hFile = CreateFile(
						filename, 
						GENERIC_READ,	// 
						FILE_SHARE_READ,// 若为0,只有该句柄关闭后,其他对该文件的操作才可以正确执行
						NULL,			// 子进程不能继承该句柄
						OPEN_EXISTING,	// 打开一存在的文件,若不存在,出错返回
						FILE_ATTRIBUTE_NORMAL,	//
						(HANDLE)NULL);
		if ( hFile == INVALID_HANDLE_VALUE ) {
			return 0xFFFFFFFF;
		}

		// 只有磁盘文件才可以判断大小
		if ( GetFileType( hFile ) != FILE_TYPE_DISK ) {
			CloseHandle( hFile );
			return 0xFFFFFFFF;
		}

		if ( (num = GetFileSize( hFile, NULL ) ) == 0xFFFFFFFF ) {
			// 第二个参数用来放文件大小的高位双字(暂时不考虑)
			CloseHandle( hFile );
			return 0xFFFFFFFF;
		}
	
		CloseHandle( hFile );

		return num;
	}

	//*********************************************************************************
	//  函数名称 : GetNoPathFileName()  
	//  功    能 :
	//		获取指定文件的不带路径的文件名称
	//  入口参数 :
	//		char	* filename		:	指定文件名(可能带路径)
	//		char	* buf			:	存放不带路径文件名的缓冲区
	//  出口参数 :
	//		char	* buf			:	存放不带路径文件名
	//  返 回 值 :
	//		int		-1: 表示获取失败; 0 表示获取成功
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.07.03
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		如果filename为NULL或长度为0, 则返回-1.
	//		buf 必须申请足够的空间(L_FILENAME=256), 并且用memset()初始化为0
	//
	//		VB 6.0 调用格式:
	//			Private Declare Function GetNoPathFileName Lib "FileApi.dll" (ByVal filename As String, ByVal str As String) As Long
	//
	//		需要注意的是, VB中调用完后, (由于显示的问题,其实已经处理过了)重新赋值才可以看到处理后的结果
	//
	//		用例:
	//			...
	//			Dim buf As String, str As String
	//			dim ret As Long
	//			...
	//			buf = string$( 256, " ")
	//			...
	//			ret = GetNoPathFileName("./dir1/dir2/filename", str)
	//			Text1.Text = str
	//			...
	//
	//
	//	维护说明 :
	//		Standard .h :
	//			string.h  ---- strlen(), memcpy()
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int __stdcall GetNoPathFileName( char * filename, char * buf )
	{
		unsigned int		i;

		if ( !filename || !* filename )
			return -1;

		i = strlen(filename);
		while ( i > 0 ) {
			i --;
			if ( filename[i] == '\\' || filename[i] == '/' )
				break;
		}

		if ( i == strlen(filename)-1 )		// filename 只是路径名(目录)
			return -1;

		if ( i == 0 )
			memcpy( buf, filename, strlen(filename) );
		else
			memcpy( buf, &filename[i+1], strlen(filename)-i-1);

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : DosGetFileAttributes()  
	//  功    能 :
	//		DOS 环境获取文件属性
	//  入口参数 : 
	//      LPTSTR pszFilePath:
	//  出口参数 : 
	//		无
	//  返 回 值 : 
	//		Return (DWORD):
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.08.27 ---- 
	//  Modify history (modifier and date) :
	//      
	//  使用说明 :
	//
	//	维护说明 :
	//		该函数用到了以下DOS中断.
	//      INT 21 - DOS 2+ - GET FILE ATTRIBUTES
	//          AX = 4300h
	//          DS:DX -> ASCIZ 文件名称 或 不以'\'结尾的目录名称(slash)
	//
	//      Return: CF set on error
	//              AX = error code (01h,02h,03h,05h) (see AH=59h)
	//              CF clear if successful
	//              CX = file attributes (see AX=4301h)
	//
	//      SeeAlso: AX=4301h, INT 2F/AX=110Fh
	//
	//      --------
	//      INT 21 - DOS 2+ - PUT FILE ATTRIBUTES (CHMOD)
	//          AX = 4301h
	//          CX = file attribute bits
	//              bit 0 = read only
	//                  1 = hidden file
	//                  2 = system file
	//                  3 = volume label
	//                  4 = subdirectory
	//                  5 = written since backup ("archive" bit)
	//                  8 = shareable (Novell NetWare)
	//          DS:DX -> ASCIZ file name
	//
	//      Return: CF set on error
	//              AX = error code (01h,02h,03h,05h) (see AH=59h)
	//          CF clear if successful
	//
	//      Note:   will not change volume label or directory attributes
	//
	//      SeeAlso: AX=4300h, INT 2F/AX=110Eh
	//
	//		Standard .h :
	//			windowsx.h --- _fmemset()
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		其他调用该函数的库及函数名:
	//			 
	//		函数调用到的本文件内的函数名: 
	//			
	//*********************************************************************************
	//
	#ifndef WIN32
	#pragma optimize("", off)
	DWORD FNGLOBAL DosGetFileAttributes( LPTSTR pszFilePath )
	{
		WORD        fwDosAttributes;

		_asm {
			push    ds
			mov     ax, 4300h
			lds     dx, pszFilePath
			int     21h
			jnc     Get_File_Attributes_Continue

			xor     cx, cx

		Get_File_Attributes_Continue:
	
			mov     fwDosAttributes, cx
			pop     ds
		}


		return ((DWORD)fwDosAttributes);
	} // DosGetFileAttributes()
	#pragma optimize("", on)
	#endif

	//*********************************************************************************
	//  函数名称 : DosGetDateTime()  
	//  功    能 :
	//		DOS 环境获取文件日期
	//  入口参数 : 
	//      HFILE hf:
	//  出口参数 : 
	//		无
	//  返 回 值 : 
	//		Return (DWORD):
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.08.27 ---- 
	//  Modify history (modifier and date) :
	//      
	//  使用说明 :
	//
	//	维护说明 :
	//		该函数用到了以下DOS中断.
	//      INT 21 - DOS 2+ - GET FILE 日期
	//          AX = 5700h
	//
	//		Standard .h :
	//			windowsx.h --- _fmemset()
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		其他调用该函数的库及函数名:
	//			 
	//		函数调用到的本文件内的函数名: 
	//			
	//*********************************************************************************
	//
	#ifndef WIN32
	#pragma optimize("", off)
	DWORD FNGLOBAL DosGetDateTime( HFILE hf )
	{
	    WORD        wDosDate;
	    WORD        wDosTime;

		_asm {
			mov     ax, 5700h
			mov     bx, hf
			int     21h
			jnc     Get_Date_Time_Continue

			xor     cx, cx
			xor     dx, dx

		Get_Date_Time_Continue:

			mov     wDosDate, dx
			mov     wDosTime, cx
		}

	    return ((DWORD)MAKELONG(wDosDate, wDosTime));
	} // DosGetDateTime()
	#pragma optimize("", on)
	#endif

	// 以上是对文件的访问操作		End }

	// 以下是对目录的访问操作		Start {

	//*********************************************************************************
	//  函数名称 : CreateDirOfFile()  
	//  功    能 :
	//			该函数用于创建全路径文件名(必须包含路径信息)的路径(结果没有'\')
	//  入口参数 :
	//		char * file		:	文件名称, 可能带有路径, 也可能没有带有路径
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		int		: 0
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//			若文件目录不存在，先生成, 
	//		若指定文件的路径信息为全路经, 则按照全路经生成路径, 否则按照相对路径生成路径
	//	维护说明 :
	//		CreateDirectory()需要用_mkdir()优化.
	//
	//		Standard .h :
	//			GetCurrentDirectory()--------winbase.h  
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int __stdcall CreateDirOfFile( char * file )
	{
		char			dirstr[BUFSIZ+1],tmp[BUFSIZ+1];
		DWORD			ret;
		DWORD			len = BUFSIZ+1;

		memset( dirstr, 0, BUFSIZ+1 );
		GetDirectoryOfFile( dirstr, file );

		memset( tmp, 0, BUFSIZ+1 );
		ret = GetCurrentDirectory( len, tmp );
	
		if ( SetCurrentDirectory(dirstr) == FALSE ) {
			// 目录不存在
			if ( CreateDirectory(dirstr,NULL) == 0 ) {	// 以下是一个递归创建目录的算法, 递归出口
				dirstr[strlen(dirstr)-1] = 0;
				CreateDirOfFile( dirstr );		// 若 CreateDirectory()失败, 则需要先建上级目录
				CreateDirOfFile( file );		// 建文件所在本级目录
			}
		}

		ret = SetCurrentDirectory(tmp);

		return 0;
	}

	//*********************************************************************************
	//  函数名称 : CreateDir()  
	//  功    能 :
	//			创建指定的路径
	//  入口参数 :
	//		char	* Dir		:	路径名(可能是相对路径)
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		bool	true : 表示创建成功, false : 表示创建失败
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.03
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		
	//	维护说明 :
	//		本质上是调用CreateDirOfFile()
	//		Standard .h :
	//            string.h--strlen()
	//			winbase.h--------GetCurrentDirectory()
	//		Develope Tools:
	//			  Visual C++ 6.0 /StuSP5
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	int __stdcall CreateDir( char *Dir )
	{
		char	tmpbuf[MAX_PATH+2];

		if ( !Dir || !*Dir ) {
			return -1;
		}

		memset( tmpbuf, 0, MAX_PATH+2 );
		strcpy( tmpbuf, Dir );

		if ( tmpbuf[strlen(tmpbuf)-1] != '\\') {	// 指定目录没有以斜杠结尾
			memcpy( &tmpbuf[strlen(tmpbuf)], "\\.", 2 );
		} else {
			tmpbuf[strlen(tmpbuf)] = '.';
		}

		return CreateDirOfFile(tmpbuf);
	}

	//*********************************************************************************
	//  函数名称 : IfDirectoryExist()  
	//  功    能 :
	//		判断指定目录是否存在
	//  入口参数 :
	//		char	* Dir		:	路径名(可能是相对路径)
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		bool	true : 表示创建成功, false : 表示创建失败
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.03
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//		
	//	维护说明 :
	//		
	//		Standard .h :
	//			
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	bool __stdcall IfDirectoryExist( char * Dir ) 
	{
		DWORD	ret;
		if ( !Dir || !*Dir )
			return false;

		ret = GetFileAttributes( Dir );
		if (  (ret & FILE_ATTRIBUTE_DIRECTORY) == (DWORD)0 )
			return false;

		return true;
	}

	//*********************************************************************************
	//  函数名称 : GetDirectoryOfFile()  
	//  功    能 :
	//			该函数用于获取全路径文件名(必须包含路径信息)的路径(结果有'\')
	//  入口参数 :
	//		char	* outstr	:	文件所在的全路径(结束有'\')
	//		char	* instr		:	全路径文件名
	//  出口参数 :
	//		char	* outstr	:	文件所在的全路径(结束有'\')
	//  返 回 值 :
	//		无
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//
	//  使用说明 :
	//			输入文件名称必须包含路径信息
	//			若指定文件名没有包含路径信息, 返回的路径信息缺省为当前目录
	//	维护说明 :
	//		
	//		Standard .h :
	//			
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	void __stdcall GetDirectoryOfFile( char * outstr, char * instr )
	{
		char	* str = outstr;
		char	* lstr = outstr;

		if ( strchr( instr, '\\' ) == NULL ) {
			memcpy( lstr,  ".\\", 2 );		// 若文件中不包含路径信息，缺省为当前目录
			return;
		}

		strcpy(str, instr);

		while( ( str = strchr( lstr+1, '\\' ) ) )
			lstr = str ++;

		* ++ lstr = '\0';

		return;
	}

	//*********************************************************************************
	//  函数名称 : GetCapacityOfDir()  
	//  功    能 :
	//			该函数用于获取指定路径下所有文件所占用的硬盘空间(单位为MB)
	//  入口参数 :
	//		char	* Dirstr	:	文件所在的全路径(结束没有'\')
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		unsigned long		0xFFFFFFFF 表示失败(-1)
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.03.11
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//		只支持4GB以下的磁盘操作, 超过4GB,返回0xFFFFFFFF(-1), 即只支持32位的文件系统
	//		不支持64位的文件系统
	//	维护说明 :
	//		
	//		Standard .h :
	//			
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	unsigned long __stdcall GetCapacityOfDir( char * Dirstr )
	{
		char				buf[MAX_PATH+1];
		HANDLE				hFound;
		WIN32_FIND_DATA		ffdFoundData;
		unsigned long		sum=0, num=0;

		// 先判断源目录是否存在
		if ( !IfDirectoryExist( Dirstr ) ) {	
			// 若不存在在错误返回
			return 0xFFFFFFFF;
		}

		memset( buf, 0, MAX_PATH+1 );
		strcpy( buf, Dirstr );

		if ( Dirstr[strlen(Dirstr)-1] == '\\' )
			memcpy( &buf[strlen(buf)], "*.*", 3 );
		else
			memcpy( &buf[strlen(buf)], "\\*.*", 4 );

		// 跳过前两个系统文件".", ".."
		hFound = FindFirstFile( buf, (LPWIN32_FIND_DATA)&ffdFoundData );	// 第一次找到的是"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n 异常错误\n");
				return 0xFFFFFFFF;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// 第二次找到的是".."
		while ( FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData) ) {
			memset( buf, 0, MAX_PATH+1 );

			if ( Dirstr[strlen(Dirstr)-1] == '\\' )
				sprintf( buf, "%s%s", Dirstr, ffdFoundData.cFileName );
			else 
				sprintf( buf, "%s\\%s", Dirstr, ffdFoundData.cFileName );
				

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // 是目录
				if ( (num = GetCapacityOfDir( buf) ) == 0xFFFFFFFF ) {
					FindClose( hFound );
					return 0xFFFFFFFF;
				}
			} else {
				if ( (num = GetSizeOfFile( buf) ) == 0xFFFFFFFF ) {
					FindClose( hFound );
					return 0xFFFFFFFF;
				}
			}
			sum += num;
		}

		FindClose( hFound );

		return sum;
	}

	//*********************************************************************************
	//  函数名称 : ifSubDir()  
	//  功    能 :
	//			判断指定目录2是否是指定目录1的子目录
	//  入口参数 :
	//		char	* Dir1	:	指定目录1
	//		char	* Dir2	:	指定目录2
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		bool	true 指定目录2是指定目录1的子目录; false 指定目录2不是指定目录1的子目录
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.06.03
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//			
	//	维护说明 :
	//		
	//		Standard .h :
	//			
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	bool __stdcall ifSubDir( char * Dir1, char * Dir2 )
	{
		char		buf1[MAX_PATH+1], buf2[MAX_PATH+2];
		// 只需根据全路经判断即可，不必判断目录是否存在

		memset( buf1, 0, MAX_PATH+1 );
		memset( buf2, 0, MAX_PATH+1 );

		if ( GetFullPath( Dir1, buf1, MAX_PATH ) == -1 )
			return false;

		if ( GetFullPath( Dir2, buf2, MAX_PATH ) == -1 )
			return false;

		if ( strlen(buf2) < strlen(buf1) )
			return false;

		// Dir1的全路径是Dir2的全路径的一部分, 表示Dir2是Dir1的子目录或就是Dir1
		if ( memcmp(buf1, buf2, strlen(buf1)) != 0 )	
			return false;

		return true;
	}

	//*********************************************************************************
	//  函数名称 : CopyDir()  
	//  功    能 :
	//			复制指定路径的文件(包括子目录及其文件)到指定路径
	//  入口参数 :
	//		char	* SourceDir	:	指定路径(相对路径或全路经)
	//		char	* DestDir	:	目标路径
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		long		
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.03.11
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//			
	//	维护说明 :
	//		
	//		Standard .h :
	//			
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	bool __stdcall CopyDir( char *SourceDir, char *DestDir )
	{
		char				buf1[MAX_PATH+1], buf2[MAX_PATH+1];
		HANDLE				hFound;
		WIN32_FIND_DATA		ffdFoundData;

		// 先判断源目录是否存在
		if ( !IfDirectoryExist( SourceDir ) ) {	
			// 若不存在在错误返回
			return false;
		}

		// 判断目标路径是否是源路经的子目录, 若是在出错返回
		if ( ifSubDir(SourceDir, DestDir) )
			return false;

		if ( !IfDirectoryExist( DestDir ) ) {
			if ( CreateDir( DestDir ) == -1 ) // 若出错, 可能会产生一些中间垃圾, 比如:目录
				return false;
		}

		memset( buf1, 0, MAX_PATH+1 );
		strcpy( buf1, SourceDir );

		if ( SourceDir[strlen(SourceDir)-1] == '\\' )
			memcpy( &buf1[strlen(buf1)], "*.*", 3 );
		else
			memcpy( &buf1[strlen(buf1)], "\\*.*", 4 );

		// 跳过前两个系统文件".", ".."
		hFound = FindFirstFile( buf1, (LPWIN32_FIND_DATA)&ffdFoundData );	// 第一次找到的是"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n 异常错误\n");
				return false;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// 第二次找到的是".."
		while ( FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData) ) {
			memset( buf1, 0, MAX_PATH+1 );
			memset( buf2, 0, MAX_PATH+1 );

			if ( SourceDir[strlen(SourceDir)-1] == '\\' )
				sprintf( buf1, "%s%s", SourceDir, ffdFoundData.cFileName );
			else 
				sprintf( buf1, "%s\\%s", SourceDir, ffdFoundData.cFileName );
				
			if ( DestDir[strlen(DestDir)-1] == '\\' )
				sprintf( buf2, "%s%s", DestDir, ffdFoundData.cFileName );
			else 
				sprintf( buf2, "%s\\%s", DestDir, ffdFoundData.cFileName );

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // 是目录
				if ( !CopyDir( buf1, buf2 ) ) {
					FindClose( hFound );
					return false;
				}
			} else {
				if ( CopyFile( buf1, buf2, FALSE) != TRUE ) {
					FindClose( hFound );
					return false;
				}
			}
		}

		FindClose( hFound );

		return true;
	}

	//*********************************************************************************
	//  函数名称 : RemoveDir()  
	//  功    能 :
	//			删除指定路径
	//  入口参数 :
	//		char	* Dir	:	指定路径(相对路径或全路经)
	//  出口参数 :
	//		无
	//  返 回 值 :
	//		long		
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.06.03
	//  Modify history (modifier and date) :
	//           
	//  使用说明 :
	//			若指定目录存在，则出错返回
	//	维护说明 :
	//		RemoveDirectory()需要用_rmdir()优化.
	//
	//		Standard .h :
	//			
	//		Develope Tools:
	//			Visual C++ 6.0 /StuSP5
	//		Environment:
	//			1. Windows 2000 Advanced Server/SP2
	//*********************************************************************************
	//
	bool __stdcall RemoveDir( char *Dirstr )
	{
		char				buf[MAX_PATH+1];
		HANDLE				hFound;
		WIN32_FIND_DATA		ffdFoundData;

		// 先判断源目录是否存在
		if ( !IfDirectoryExist( Dirstr ) ) {	
			// 若不存在在错误返回
			return false;
		}

		memset( buf, 0, MAX_PATH+1 );
		strcpy( buf, Dirstr );

		if ( Dirstr[strlen(Dirstr)-1] == '\\' )
			memcpy( &buf[strlen(buf)], "*.*", 3 );
		else
			memcpy( &buf[strlen(buf)], "\\*.*", 4 );

		// 跳过前两个系统文件".", ".."
		hFound = FindFirstFile( buf, (LPWIN32_FIND_DATA)&ffdFoundData );	// 第一次找到的是"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n 异常错误\n");
				return false;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// 第二次找到的是".."
		while ( FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData) ) {
			memset( buf, 0, MAX_PATH+1 );

			if ( Dirstr[strlen(Dirstr)-1] == '\\' )
				sprintf( buf, "%s%s", Dirstr, ffdFoundData.cFileName );
			else 
				sprintf( buf, "%s\\%s", Dirstr, ffdFoundData.cFileName );
				

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // 是目录
				if ( !RemoveDir( buf) ) {
					FindClose( hFound );
					return false;
				}
			} else {
				// 因为在NT/2000 下, _unlink()不能删除只读或系统文件, 所以需要先将文件属性改为存档
				if ( SetFileAttributes(buf, FILE_ATTRIBUTE_ARCHIVE) == FALSE ) {
					FindClose( hFound );
					return false;
				}
				if ( _unlink( buf) != 0 ) {
					FindClose( hFound );
					return false;
				}
			}
		}

		FindClose( hFound );

		if ( RemoveDirectory(Dirstr) != TRUE )
			return false;

		return true;
	}

	// 以上是对目录的访问操作		End  }


	// 参数 remstr: 是用来指定 注释符的
	bool __stdcall CreateCopyRightInfo( char *filename, char *period, 
				char *corporation, char *author, char *remstr )
	{
		FILE		*fp = NULL;			// 文件 句柄
		char		tmpfile[BUFSIZ+1];
		char		RemBuf[BUFSIZ+1];

		if ( filename == NULL || *filename == NULL )
			return false;

		// 如果有必要需要创建指定文件对应的目录
		CreateDirOfFile( filename );

		// 如果 该文件不存在, 系统自动创建
	
		memset( tmpfile, 0, BUFSIZ+1 );
		sprintf(tmpfile,"~CR.tmp");
	
	
		if ( (fp = fopen(tmpfile,"w+")) == NULL ) {  // 清空方式创建打开指定名称的文件
			return false;
		}

		memset( RemBuf, 0, BUFSIZ+1 );
		if ( remstr == NULL || *remstr == NULL )
			sprintf( RemBuf, "\t//" );
		else
			sprintf( RemBuf, "%s", remstr );

		fprintf( fp, "%s***************************************************************************\n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s                Copyright (C) %s %s\n", RemBuf, period, corporation );
		fprintf( fp, "%s                          All Rights Reserved \n", RemBuf );
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s***************************************************************************\n", RemBuf);
		fprintf( fp, "%s***************************************************************************\n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s  Portions Copyright (C) %s %s\n", RemBuf, period, corporation );
		fprintf( fp, "%s	      All Rights Reserved.\n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s The information in this file is provided for the exclusive use of \n", RemBuf);
		fprintf( fp, "%s the licensees of The Bright Oceans, Inc.  Such users have the \n", RemBuf);
		fprintf( fp, "%s right to use, modify, and incorporate this code into other products \n", RemBuf);
		fprintf( fp, "%s for purposes authorized by the license agreement provided they include \n", RemBuf);
		fprintf( fp, "%s this notice and the associated copyright notice with any such product. \n", RemBuf);
		fprintf( fp, "%s The information in this file is provided \"AS IS\" without warranty. \n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s            All Rights Reserved \n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE %s \n", RemBuf, author );
		fprintf( fp, "%s The copyright notice above does not evidence any \n", RemBuf ); 
		fprintf( fp, "%s actual or intended publication of such source code. \n", RemBuf );
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s \n", RemBuf );
		fprintf( fp, "%s***************************************************************************\n", RemBuf);
		fprintf( fp, "%s \n", RemBuf );
	
		fclose ( fp);

		// 将原来的文件内容追加到 已经有 版权信息的 临时文件中
		AppendFile( filename, tmpfile );

		// 再 删除 原来的文件, 用 临时文件替代 原来的文件, 最后删除临时文件
		_unlink( filename );
		AppendFile( tmpfile, filename );
		_unlink( tmpfile );

		return true;

	}


	//***************************************************************************/
	//  函数名称 : AppendFile()  
	//  功    能 :      
	//             追加文件 
	//  入口参数 :  
	//            s_filename : 源文件名称
	//            d_filename : 目标文件名称
	//  出口参数 :         
	//            无       
	//  返 回 值 :         
	//            int : 返回错误号 
	// Author   :                  
	//            Adams            
	//  Create Date :        
	//            2000.06.21 
	//  Modify history (modifier and date) :       
	//            2004.08.11 : 从Inifile.dll 转移到此
	//  使用说明 :   
	//            若目标文件不存在, 创建目标文件,并拷贝源文件到目标文件  
	//  Standard .h  :     
	//           io.h--open(),read(),write(),close()  
	//            fcntl.h--O_RDONLY,O_RDWR,O_APPEND,O_BINARY,O_CREAT 
	//  Develope Tools:     
	//			  Visual C++ 6.0 
	//  Environment:           
	//          1. Windows 2000 SimpleChinese/SP4
	//              Can be used in console、MFC Application for Windows 
	//**************************************************************************/
	int __stdcall AppendFile(char *s_filename,char *d_filename)
	{
		int		s_fp, d_fp;
		char	buf[BUFSIZ+1];
		int		num;
	
		if ( (s_fp = open(s_filename, O_RDONLY|O_BINARY, 0666)) < 0 ) {
			return -1;
		}
	
		if ( (d_fp = open(d_filename, O_RDWR|O_APPEND|O_BINARY|O_CREAT,0666)) < 0 ) {
			close(s_fp);
			return -1;
		}
	
		memset(buf, 0, BUFSIZ+1);
		while ( ( num = read(s_fp,buf,BUFSIZ) )> 0 ) {
			if ( write(d_fp,buf,num) != num ) {
				close(s_fp);
				close(d_fp);
				return -1;
			}
		}
	
		close(s_fp);
		close(d_fp);
	
		return 0;
	}
	


/*
 *
 *	Adat的账户管理
 *
 *  借鉴linux/kernel/acct.c
 *
 *  原作者Macro Van Wieringen的部分代码基于 Thomas K. Dyas 
 *			<tdyas@eden.rutgers.edu>
 *	的想法和代码。
 *
 *  原源文件实现了BSD风格进程账户. 当进程一旦存在,一个acct结构类型的纪录被写到
 *  由acct()系统调用指定的文件中. 它胜任于用户级程序用账户日志做一些有用的事情.
 *  内核只要提供原始(raw)的账户信息.
 *  
 *  预防两个漏洞. 
 *		1) 如果文件只读,不会将acct_file返回到free_filps.
 *		2) 如果由于缺乏空间,账户被挂起,它适当允许重新打开并完全丢失旧acct_file. 
 *				3/10/98, Al Viro.
 *
 *  关闭acct_file再重新打开. 清理sys_acct().
 *  XTerms(终端仿真程序)和EMACS(编辑程序宏指令)显得非常有害. 21/10/98, AV.
 *
 *  用sys_umount()修正有威胁的交互. 如果账户挂起,用umount()停止失败,就乱了.
 *  另一种情况: remount 只读,不停止账户.
 *        问题: 若是CAP_SYS_ADMIN又不是CAP_SYS_PACCT,又能干什么?目前代码如下处理:
 *				umount 返回EBUSY,除非root乱了.在这种情况,用do_remount_sb()会更乱.
 *											9/11/98, AV.
 *   Oh, fsck... Oopsable SMP race in do_process_acct() - we must hold
 * ->mmap_sem to walk the vma list of current->mm. Nasty, since it leaks
 * a struct file opened for write. Fixed. 2/6/2000, AV.
 */
	/*
bool CADat::ValidSpace(char *inifile)
// 由于是针对小数据用户，故目前对空间限制为
//		128MB=128*1048576 bytes =134217728 bytes.
// 
{
	char			tmpstr[BUFSIZ+1];
	char			drv[8];
	unsigned long	SectorsPerCluster;  // pointer to sectors per cluster
	unsigned long	BytesPerSector;  // pointer to bytes per sector
	unsigned long	NumberOfFreeClusters;
                             // pointer to number of free clusters
	unsigned long	TotalNumberOfClusters; 
                             // pointer to total number of clusters
	int				Free_Space;

	memset( tmpstr, 0, BUFSIZ+1 );
	ReadSectItem( inifile, S_DATMAIN, I_SPACE , tmpstr );
	if ( strlen(tmpstr) > L_DATSPACE ) {
		errno = OUT_RANGE_ERR;
		wlog(F_TMPLOG,"ValidSpace() I_SPACE OUT_RANGE_ERR");
		return false;
	}

	if(GetDiskFreeSpace("c://",&SectorsPerCluster,&BytesPerSector,&NumberOfFreeClusters,&TotalNumberOfClusters)==0)  
		return false;
	Free_Space=SectorsPerCluster*BytesPerSector*NumberOfFreeClusters;

	try{
		if ( atoi(tmpstr)>Free_Space) {
			errno = OUT_RANGE_ERR;
			wlog(F_TMPLOG,"ValidSpace() Not enough free disk!");
			return false;
		}
	}
	catch(...){
		wlog(F_TMPLOG,"ValidSpace() Exceptain error!");
		return false;
	}

	return true;
}
	*/