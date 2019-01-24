    //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.��LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// ģ��		: FileApi.Cpp
	// 
	// ����		: ��׼���ô���API, �й��ļ��������ʵ�Ӧ�ýӿں���, SDK�汾, �����������๤����.
	//
	// ���ܺ���	:
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
	// ��ʷ		:
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
	//  �������� : GetFullPath()  
	//  ��    �� :
	//			��ȡָ���ļ�(��Ŀ¼)��ȫ·������
	//  ��ڲ��� :
	//		char	*in				�ļ���·�����ƣ����ܲ���·������
	//  ���ڲ��� :
	//		char	*out			�ļ���·�����ƣ���ȫ·������
	//  �� �� ֵ :
	//		int		0 ��ʾ �ɹ�; -1 ��ʾ ʧ��
	//  Author   :
	//		Adams                 
	//  Create Date :
	//		2002.06.03            
	//  Modify history (modifier and date) :
	//		
	//  ʹ��˵�� :
	//			����ǰ��out ���������㹻�Ŀռ�(MAX_PAHT+2), ����memset()��ʼ��Ϊ0, ���뽫
	//		����������Ĵ�С��Ϊ����outsize����.
	//		������Ŀ¼�Ƿ����
	//	ά��˵�� :
	//		��������Ӳ�̻�, �ó�����д
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
                    (LPTSTR *)&lpsz);		// ���һ����������ȡin�����һ����, ������������
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

	// �����Ƕ��ļ��ķ��ʲ���		Start {

	//*********************************************************************************
	//  �������� : GetTmpFile()  
	//  ��    �� :
	//			��ȡ��ʱ�ļ�����
	//  ��ڲ��� :
	//		int		i				�ļ����
	//		char	* tmpfile
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		
	//  Author   :
	//            Adams                 
	//  Create Date :
	//            2001.12.11            
	//  Modify history (modifier and date) :
	//                                      
	//  ʹ��˵�� :
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
	//  �������� : IfFileExist()  
	//  ��    �� :
	//		�ж�ָ���ļ��Ƿ����
	//  ��ڲ��� :
	//		char	* szFileName		:	ָ���ļ���(���������·��)
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		bool	true : ��ʾָ���ļ�����, false : ��ʾָ���ļ�������
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.04
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		
	//	ά��˵�� :
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

		if ( OpenFile( szFileName, &of, OF_EXIST ) == -1 ) {	// �򿪺��Զ��رգ���������ƶ��ļ��Ƿ����
			return false;
		}

		return true;
	}

	//*********************************************************************************
	//  �������� : GetSizeOfFile()  
	//  ��    �� :
	//			��ȡָ���ļ��Ĵ�С(�ֽ���)
	//  ��ڲ��� :
	//		char	* szFileName		:	ָ���ļ���(���������·��)
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		unsigned long	0xFFFFFFFF : ��ʾʧ��(-1)
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.04
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		
	//	ά��˵�� :
	//		��ϵͳ����GetFileSize()��һ��Ӧ��, ��Ҫ��Ϊ�˼򻯵���
	//		�����ƶ����ļ���С����2��32�η�Byte֮�ڣ���С��2��12�η�MB=4GB
	//		ټ��������4GB���ļ�����Ҫ�����ú�������GetFileSize()�ĵڶ���������������
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


		// CreateFile�������ļ�
		hFile = CreateFile(
						filename, 
						GENERIC_READ,	// 
						FILE_SHARE_READ,// ��Ϊ0,ֻ�иþ���رպ�,�����Ը��ļ��Ĳ����ſ�����ȷִ��
						NULL,			// �ӽ��̲��ܼ̳иþ��
						OPEN_EXISTING,	// ��һ���ڵ��ļ�,��������,��������
						FILE_ATTRIBUTE_NORMAL,	//
						(HANDLE)NULL);
		if ( hFile == INVALID_HANDLE_VALUE ) {
			return 0xFFFFFFFF;
		}

		// ֻ�д����ļ��ſ����жϴ�С
		if ( GetFileType( hFile ) != FILE_TYPE_DISK ) {
			CloseHandle( hFile );
			return 0xFFFFFFFF;
		}

		if ( (num = GetFileSize( hFile, NULL ) ) == 0xFFFFFFFF ) {
			// �ڶ��������������ļ���С�ĸ�λ˫��(��ʱ������)
			CloseHandle( hFile );
			return 0xFFFFFFFF;
		}
	
		CloseHandle( hFile );

		return num;
	}

	//*********************************************************************************
	//  �������� : GetNoPathFileName()  
	//  ��    �� :
	//		��ȡָ���ļ��Ĳ���·�����ļ�����
	//  ��ڲ��� :
	//		char	* filename		:	ָ���ļ���(���ܴ�·��)
	//		char	* buf			:	��Ų���·���ļ����Ļ�����
	//  ���ڲ��� :
	//		char	* buf			:	��Ų���·���ļ���
	//  �� �� ֵ :
	//		int		-1: ��ʾ��ȡʧ��; 0 ��ʾ��ȡ�ɹ�
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.07.03
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		���filenameΪNULL�򳤶�Ϊ0, �򷵻�-1.
	//		buf ���������㹻�Ŀռ�(L_FILENAME=256), ������memset()��ʼ��Ϊ0
	//
	//		VB 6.0 ���ø�ʽ:
	//			Private Declare Function GetNoPathFileName Lib "FileApi.dll" (ByVal filename As String, ByVal str As String) As Long
	//
	//		��Ҫע�����, VB�е������, (������ʾ������,��ʵ�Ѿ���������)���¸�ֵ�ſ��Կ���������Ľ��
	//
	//		����:
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
	//	ά��˵�� :
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

		if ( i == strlen(filename)-1 )		// filename ֻ��·����(Ŀ¼)
			return -1;

		if ( i == 0 )
			memcpy( buf, filename, strlen(filename) );
		else
			memcpy( buf, &filename[i+1], strlen(filename)-i-1);

		return 0;
	}

	//*********************************************************************************
	//  �������� : DosGetFileAttributes()  
	//  ��    �� :
	//		DOS ������ȡ�ļ�����
	//  ��ڲ��� : 
	//      LPTSTR pszFilePath:
	//  ���ڲ��� : 
	//		��
	//  �� �� ֵ : 
	//		Return (DWORD):
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.08.27 ---- 
	//  Modify history (modifier and date) :
	//      
	//  ʹ��˵�� :
	//
	//	ά��˵�� :
	//		�ú����õ�������DOS�ж�.
	//      INT 21 - DOS 2+ - GET FILE ATTRIBUTES
	//          AX = 4300h
	//          DS:DX -> ASCIZ �ļ����� �� ����'\'��β��Ŀ¼����(slash)
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
	//		�������øú����Ŀ⼰������:
	//			 
	//		�������õ��ı��ļ��ڵĺ�����: 
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
	//  �������� : DosGetDateTime()  
	//  ��    �� :
	//		DOS ������ȡ�ļ�����
	//  ��ڲ��� : 
	//      HFILE hf:
	//  ���ڲ��� : 
	//		��
	//  �� �� ֵ : 
	//		Return (DWORD):
	//  Author   : 
	//		Adams                 
	//  Create Date :
	//		2002.08.27 ---- 
	//  Modify history (modifier and date) :
	//      
	//  ʹ��˵�� :
	//
	//	ά��˵�� :
	//		�ú����õ�������DOS�ж�.
	//      INT 21 - DOS 2+ - GET FILE ����
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
	//		�������øú����Ŀ⼰������:
	//			 
	//		�������õ��ı��ļ��ڵĺ�����: 
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

	// �����Ƕ��ļ��ķ��ʲ���		End }

	// �����Ƕ�Ŀ¼�ķ��ʲ���		Start {

	//*********************************************************************************
	//  �������� : CreateDirOfFile()  
	//  ��    �� :
	//			�ú������ڴ���ȫ·���ļ���(�������·����Ϣ)��·��(���û��'\')
	//  ��ڲ��� :
	//		char * file		:	�ļ�����, ���ܴ���·��, Ҳ����û�д���·��
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		int		: 0
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//			���ļ�Ŀ¼�����ڣ�������, 
	//		��ָ���ļ���·����ϢΪȫ·��, ����ȫ·������·��, ���������·������·��
	//	ά��˵�� :
	//		CreateDirectory()��Ҫ��_mkdir()�Ż�.
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
			// Ŀ¼������
			if ( CreateDirectory(dirstr,NULL) == 0 ) {	// ������һ���ݹ鴴��Ŀ¼���㷨, �ݹ����
				dirstr[strlen(dirstr)-1] = 0;
				CreateDirOfFile( dirstr );		// �� CreateDirectory()ʧ��, ����Ҫ�Ƚ��ϼ�Ŀ¼
				CreateDirOfFile( file );		// ���ļ����ڱ���Ŀ¼
			}
		}

		ret = SetCurrentDirectory(tmp);

		return 0;
	}

	//*********************************************************************************
	//  �������� : CreateDir()  
	//  ��    �� :
	//			����ָ����·��
	//  ��ڲ��� :
	//		char	* Dir		:	·����(���������·��)
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		bool	true : ��ʾ�����ɹ�, false : ��ʾ����ʧ��
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.03
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		
	//	ά��˵�� :
	//		�������ǵ���CreateDirOfFile()
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

		if ( tmpbuf[strlen(tmpbuf)-1] != '\\') {	// ָ��Ŀ¼û����б�ܽ�β
			memcpy( &tmpbuf[strlen(tmpbuf)], "\\.", 2 );
		} else {
			tmpbuf[strlen(tmpbuf)] = '.';
		}

		return CreateDirOfFile(tmpbuf);
	}

	//*********************************************************************************
	//  �������� : IfDirectoryExist()  
	//  ��    �� :
	//		�ж�ָ��Ŀ¼�Ƿ����
	//  ��ڲ��� :
	//		char	* Dir		:	·����(���������·��)
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		bool	true : ��ʾ�����ɹ�, false : ��ʾ����ʧ��
	//  Author   :
	//		Adams
	//  Create Date :
	//		2002.06.03
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//		
	//	ά��˵�� :
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
	//  �������� : GetDirectoryOfFile()  
	//  ��    �� :
	//			�ú������ڻ�ȡȫ·���ļ���(�������·����Ϣ)��·��(�����'\')
	//  ��ڲ��� :
	//		char	* outstr	:	�ļ����ڵ�ȫ·��(������'\')
	//		char	* instr		:	ȫ·���ļ���
	//  ���ڲ��� :
	//		char	* outstr	:	�ļ����ڵ�ȫ·��(������'\')
	//  �� �� ֵ :
	//		��
	//  Author   :
	//            Adams
	//  Create Date :
	//            2001.12.11
	//  Modify history (modifier and date) :
	//
	//  ʹ��˵�� :
	//			�����ļ����Ʊ������·����Ϣ
	//			��ָ���ļ���û�а���·����Ϣ, ���ص�·����ϢȱʡΪ��ǰĿ¼
	//	ά��˵�� :
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
			memcpy( lstr,  ".\\", 2 );		// ���ļ��в�����·����Ϣ��ȱʡΪ��ǰĿ¼
			return;
		}

		strcpy(str, instr);

		while( ( str = strchr( lstr+1, '\\' ) ) )
			lstr = str ++;

		* ++ lstr = '\0';

		return;
	}

	//*********************************************************************************
	//  �������� : GetCapacityOfDir()  
	//  ��    �� :
	//			�ú������ڻ�ȡָ��·���������ļ���ռ�õ�Ӳ�̿ռ�(��λΪMB)
	//  ��ڲ��� :
	//		char	* Dirstr	:	�ļ����ڵ�ȫ·��(����û��'\')
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		unsigned long		0xFFFFFFFF ��ʾʧ��(-1)
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.03.11
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//		ֻ֧��4GB���µĴ��̲���, ����4GB,����0xFFFFFFFF(-1), ��ֻ֧��32λ���ļ�ϵͳ
	//		��֧��64λ���ļ�ϵͳ
	//	ά��˵�� :
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

		// ���ж�ԴĿ¼�Ƿ����
		if ( !IfDirectoryExist( Dirstr ) ) {	
			// ���������ڴ��󷵻�
			return 0xFFFFFFFF;
		}

		memset( buf, 0, MAX_PATH+1 );
		strcpy( buf, Dirstr );

		if ( Dirstr[strlen(Dirstr)-1] == '\\' )
			memcpy( &buf[strlen(buf)], "*.*", 3 );
		else
			memcpy( &buf[strlen(buf)], "\\*.*", 4 );

		// ����ǰ����ϵͳ�ļ�".", ".."
		hFound = FindFirstFile( buf, (LPWIN32_FIND_DATA)&ffdFoundData );	// ��һ���ҵ�����"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n �쳣����\n");
				return 0xFFFFFFFF;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// �ڶ����ҵ�����".."
		while ( FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData) ) {
			memset( buf, 0, MAX_PATH+1 );

			if ( Dirstr[strlen(Dirstr)-1] == '\\' )
				sprintf( buf, "%s%s", Dirstr, ffdFoundData.cFileName );
			else 
				sprintf( buf, "%s\\%s", Dirstr, ffdFoundData.cFileName );
				

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // ��Ŀ¼
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
	//  �������� : ifSubDir()  
	//  ��    �� :
	//			�ж�ָ��Ŀ¼2�Ƿ���ָ��Ŀ¼1����Ŀ¼
	//  ��ڲ��� :
	//		char	* Dir1	:	ָ��Ŀ¼1
	//		char	* Dir2	:	ָ��Ŀ¼2
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		bool	true ָ��Ŀ¼2��ָ��Ŀ¼1����Ŀ¼; false ָ��Ŀ¼2����ָ��Ŀ¼1����Ŀ¼
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.06.03
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//			
	//	ά��˵�� :
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
		// ֻ�����ȫ·���жϼ��ɣ������ж�Ŀ¼�Ƿ����

		memset( buf1, 0, MAX_PATH+1 );
		memset( buf2, 0, MAX_PATH+1 );

		if ( GetFullPath( Dir1, buf1, MAX_PATH ) == -1 )
			return false;

		if ( GetFullPath( Dir2, buf2, MAX_PATH ) == -1 )
			return false;

		if ( strlen(buf2) < strlen(buf1) )
			return false;

		// Dir1��ȫ·����Dir2��ȫ·����һ����, ��ʾDir2��Dir1����Ŀ¼�����Dir1
		if ( memcmp(buf1, buf2, strlen(buf1)) != 0 )	
			return false;

		return true;
	}

	//*********************************************************************************
	//  �������� : CopyDir()  
	//  ��    �� :
	//			����ָ��·�����ļ�(������Ŀ¼�����ļ�)��ָ��·��
	//  ��ڲ��� :
	//		char	* SourceDir	:	ָ��·��(���·����ȫ·��)
	//		char	* DestDir	:	Ŀ��·��
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		long		
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.03.11
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//			
	//	ά��˵�� :
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

		// ���ж�ԴĿ¼�Ƿ����
		if ( !IfDirectoryExist( SourceDir ) ) {	
			// ���������ڴ��󷵻�
			return false;
		}

		// �ж�Ŀ��·���Ƿ���Դ·������Ŀ¼, �����ڳ�������
		if ( ifSubDir(SourceDir, DestDir) )
			return false;

		if ( !IfDirectoryExist( DestDir ) ) {
			if ( CreateDir( DestDir ) == -1 ) // ������, ���ܻ����һЩ�м�����, ����:Ŀ¼
				return false;
		}

		memset( buf1, 0, MAX_PATH+1 );
		strcpy( buf1, SourceDir );

		if ( SourceDir[strlen(SourceDir)-1] == '\\' )
			memcpy( &buf1[strlen(buf1)], "*.*", 3 );
		else
			memcpy( &buf1[strlen(buf1)], "\\*.*", 4 );

		// ����ǰ����ϵͳ�ļ�".", ".."
		hFound = FindFirstFile( buf1, (LPWIN32_FIND_DATA)&ffdFoundData );	// ��һ���ҵ�����"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n �쳣����\n");
				return false;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// �ڶ����ҵ�����".."
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

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // ��Ŀ¼
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
	//  �������� : RemoveDir()  
	//  ��    �� :
	//			ɾ��ָ��·��
	//  ��ڲ��� :
	//		char	* Dir	:	ָ��·��(���·����ȫ·��)
	//  ���ڲ��� :
	//		��
	//  �� �� ֵ :
	//		long		
	//  Author   :
	//            Adams
	//  Create Date :
	//            2002.06.03
	//  Modify history (modifier and date) :
	//           
	//  ʹ��˵�� :
	//			��ָ��Ŀ¼���ڣ����������
	//	ά��˵�� :
	//		RemoveDirectory()��Ҫ��_rmdir()�Ż�.
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

		// ���ж�ԴĿ¼�Ƿ����
		if ( !IfDirectoryExist( Dirstr ) ) {	
			// ���������ڴ��󷵻�
			return false;
		}

		memset( buf, 0, MAX_PATH+1 );
		strcpy( buf, Dirstr );

		if ( Dirstr[strlen(Dirstr)-1] == '\\' )
			memcpy( &buf[strlen(buf)], "*.*", 3 );
		else
			memcpy( &buf[strlen(buf)], "\\*.*", 4 );

		// ����ǰ����ϵͳ�ļ�".", ".."
		hFound = FindFirstFile( buf, (LPWIN32_FIND_DATA)&ffdFoundData );	// ��һ���ҵ�����"."
		if ( (HANDLE)(-1) == hFound ) {
			if ( GetLastError() != ERROR_FILE_NOT_FOUND ) {
				printf("\n �쳣����\n");
				return false;
			}
		}

		FindNextFile( hFound, (LPWIN32_FIND_DATA)&ffdFoundData );			// �ڶ����ҵ�����".."
		while ( FindNextFile(hFound, (LPWIN32_FIND_DATA)&ffdFoundData) ) {
			memset( buf, 0, MAX_PATH+1 );

			if ( Dirstr[strlen(Dirstr)-1] == '\\' )
				sprintf( buf, "%s%s", Dirstr, ffdFoundData.cFileName );
			else 
				sprintf( buf, "%s\\%s", Dirstr, ffdFoundData.cFileName );
				

			if ( FILE_ATTRIBUTE_DIRECTORY & ffdFoundData.dwFileAttributes ) { // ��Ŀ¼
				if ( !RemoveDir( buf) ) {
					FindClose( hFound );
					return false;
				}
			} else {
				// ��Ϊ��NT/2000 ��, _unlink()����ɾ��ֻ����ϵͳ�ļ�, ������Ҫ�Ƚ��ļ����Ը�Ϊ�浵
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

	// �����Ƕ�Ŀ¼�ķ��ʲ���		End  }


	// ���� remstr: ������ָ�� ע�ͷ���
	bool __stdcall CreateCopyRightInfo( char *filename, char *period, 
				char *corporation, char *author, char *remstr )
	{
		FILE		*fp = NULL;			// �ļ� ���
		char		tmpfile[BUFSIZ+1];
		char		RemBuf[BUFSIZ+1];

		if ( filename == NULL || *filename == NULL )
			return false;

		// ����б�Ҫ��Ҫ����ָ���ļ���Ӧ��Ŀ¼
		CreateDirOfFile( filename );

		// ��� ���ļ�������, ϵͳ�Զ�����
	
		memset( tmpfile, 0, BUFSIZ+1 );
		sprintf(tmpfile,"~CR.tmp");
	
	
		if ( (fp = fopen(tmpfile,"w+")) == NULL ) {  // ��շ�ʽ������ָ�����Ƶ��ļ�
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

		// ��ԭ�����ļ�����׷�ӵ� �Ѿ��� ��Ȩ��Ϣ�� ��ʱ�ļ���
		AppendFile( filename, tmpfile );

		// �� ɾ�� ԭ�����ļ�, �� ��ʱ�ļ���� ԭ�����ļ�, ���ɾ����ʱ�ļ�
		_unlink( filename );
		AppendFile( tmpfile, filename );
		_unlink( tmpfile );

		return true;

	}


	//***************************************************************************/
	//  �������� : AppendFile()  
	//  ��    �� :      
	//             ׷���ļ� 
	//  ��ڲ��� :  
	//            s_filename : Դ�ļ�����
	//            d_filename : Ŀ���ļ�����
	//  ���ڲ��� :         
	//            ��       
	//  �� �� ֵ :         
	//            int : ���ش���� 
	// Author   :                  
	//            Adams            
	//  Create Date :        
	//            2000.06.21 
	//  Modify history (modifier and date) :       
	//            2004.08.11 : ��Inifile.dll ת�Ƶ���
	//  ʹ��˵�� :   
	//            ��Ŀ���ļ�������, ����Ŀ���ļ�,������Դ�ļ���Ŀ���ļ�  
	//  Standard .h  :     
	//           io.h--open(),read(),write(),close()  
	//            fcntl.h--O_RDONLY,O_RDWR,O_APPEND,O_BINARY,O_CREAT 
	//  Develope Tools:     
	//			  Visual C++ 6.0 
	//  Environment:           
	//          1. Windows 2000 SimpleChinese/SP4
	//              Can be used in console��MFC Application for Windows 
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
 *	Adat���˻�����
 *
 *  ���linux/kernel/acct.c
 *
 *  ԭ����Macro Van Wieringen�Ĳ��ִ������ Thomas K. Dyas 
 *			<tdyas@eden.rutgers.edu>
 *	���뷨�ʹ��롣
 *
 *  ԭԴ�ļ�ʵ����BSD�������˻�. ������һ������,һ��acct�ṹ���͵ļ�¼��д��
 *  ��acct()ϵͳ����ָ�����ļ���. ��ʤ�����û����������˻���־��һЩ���õ�����.
 *  �ں�ֻҪ�ṩԭʼ(raw)���˻���Ϣ.
 *  
 *  Ԥ������©��. 
 *		1) ����ļ�ֻ��,���Ὣacct_file���ص�free_filps.
 *		2) �������ȱ���ռ�,�˻�������,���ʵ��������´򿪲���ȫ��ʧ��acct_file. 
 *				3/10/98, Al Viro.
 *
 *  �ر�acct_file�����´�. ����sys_acct().
 *  XTerms(�ն˷������)��EMACS(�༭�����ָ��)�Ե÷ǳ��к�. 21/10/98, AV.
 *
 *  ��sys_umount()��������в�Ľ���. ����˻�����,��umount()ֹͣʧ��,������.
 *  ��һ�����: remount ֻ��,��ֹͣ�˻�.
 *        ����: ����CAP_SYS_ADMIN�ֲ���CAP_SYS_PACCT,���ܸ�ʲô?Ŀǰ�������´���:
 *				umount ����EBUSY,����root����.���������,��do_remount_sb()�����.
 *											9/11/98, AV.
 *   Oh, fsck... Oopsable SMP race in do_process_acct() - we must hold
 * ->mmap_sem to walk the vma list of current->mm. Nasty, since it leaks
 * a struct file opened for write. Fixed. 2/6/2000, AV.
 */
	/*
bool CADat::ValidSpace(char *inifile)
// ���������С�����û�����Ŀǰ�Կռ�����Ϊ
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