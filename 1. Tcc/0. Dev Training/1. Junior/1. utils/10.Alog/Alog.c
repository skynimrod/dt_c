   //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.��LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// Module File	: Alog.Cpp
	// 
	// Description	: LOG  Management API, the interface function about the visiting log file. 
	//			There is SDK Version, can be used in many tools.
	//
	// Author	: Michael.Adams
	//
	// Function	:
	//		1.  wlog()
	//		2.  lwlog()
	//		3.  wlogEx()		Write Log Function Extrend
	//		4.	wlogExDebug()	the log function of Debugging status
	//
	// Modified History	:
	//		2003.08.05 :
	//			Add the implement of the function wlogExDebug() . used programming in Debugging status
	// Notes	:
	//		
	//***************************************************************************
	//
	//
    //
	
	#include	<stdio.h>
	#include	<string.h>
	#include 	<time.h>
	#include	<fcntl.h>
	#include	<io.h>
	#include	<stdarg.h>

	#include	"Length.h"
	#include	"Astring.h"
	#include	"FileApi.h"

	#define		L_LINESIZE		80		// equal the sum of the length: time(19)+space(1)+information(60)

//	#define		F_INCBLANK		0		// a flag, different value express different meaning:
										//   0 : 
										// �����ļ�����ʱ��������
//	#define		F_EXCBLANK		1		// �����ļ�����ʱ����������
//
	#define DLL_EXPORT __declspec(dllexport)

	//*********************************************************************************
	//  �������� : wlog()  
	//  ��    �� :
	//			��ָ����־�ļ�д��־
	//  ��ڲ��� :
	//			char	* logname		��־�ļ���ȫ·��
	//			char	* logmsg			��־����
	//			flag : 0x00
	//					 ||
	//					 |+----  0  ��ʾ����־������, 1(ȱʡֵ) ��ʾû����־������(�ݲ�֧��)
	//					 +-----  0 ��ʾ����ÿ�г��ȵ�����, ���Զ�����
	//							 1 (ȱʡֵ) ��ʾÿ����󳤶�ΪL_LINESIZE, �Զ����У���־��Ϣ
	//								��ֶ�д����־�У�����һ����ʱ����Ϣ�⣬������û��ʱ����
	//								Ϣ�� ���ο���L_DATETIME+1���ո񣬱��ڲ쿴
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		ֵΪ0 ��ʾ�ɹ�, ֵΪfalse��ʾʧ��
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.06.19
	//  Modify history (modifier and date) :
	//			2002.07.04	-------  ����ά���ĵ��Լ�VB6.0���ýӿ�˵��
	//  ʹ��˵�� :
	//	
	//			logmsg ��Ϊ��, ��ʧ�ܷ���-1��logmsg���ȳ���L_LOGBUF,�򲻴���L_LOGBUF֮�����Ϣ��
	//			ֻ֧��logmsg ΪTEXT�ĸ�ʽ����֧��Binary��ʽ
	//			���ڸ�������־�ļ���ϵͳ�Զ�Ϊ�ļ����Ƽ��뵱ǰ���ڣ����ڱ�ʾ��־���ɵ�ʱ��
	//			ͬʱ������־�ļ��Ĵ�С
	//
	//		VB 6.0 ���ø�ʽ:
	//			Private Declare Function wlog Lib "alog.dll" (ByVal logname As String, ByVal logmsg As String, ByVal flag As Long) As Long
	//				��
	//			Private Declare Function wlog Lib "alog.dll" (ByVal logname As String, ByVal logmsg As String, ByVal flag As Long) As Integer
	//
	//		��Ҫע�����, c/c++����ʱ, ��������������ʡ��, ����VB�µ���ʱ, ����������������ʡ��
	//		���� Astring.dll, FileApi.dll (Ҫ��汾����һ��)������Alog.dllͬһĿ¼��, Ҳ���Կ���System32��.
	//
	//		VB 6.0 ���þ���:
	//			...
	//			Dim buf As String
	//			Dim ret As Long
	//			...
	//			buf = "12345678901234567890123456789012345678901234567890123456789��" _
	//				& "12345678901234567890123456789012345678901234567890123456789��" _
    //				& "12345678901234567890123456789012345678901234567890123456789��" _
    //				& "12345678901234567890123456789012345678901234567890123456789��" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890" _
	//				& "123456789012345678901234567890123456789012345678901234567890"
	//			...
	//			ret = wlog("..\\abcde.txt", buf, 1)
	//			...
	//
	//	ά��˵�� :
	//
	//		Standard .h :
	//			stdio.h----------sprintf()
	//			time.h-----------time_t, struct tm,time(), localtime()
	//			string.h---------memset(),strlen(), strchr(), 
	//			Length.h---------L_DATETIME,
	//			Astring.h--------GetFirstWord()
	//			FileApi.h--------CreateDirOfFile()
	//			fcntl.h----------O_WRONLY, O_APPEND, O_CREAT
	//			io.h-------------open()��write()��close()
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		�������øú����Ŀ⼰������:
	//			
	//*********************************************************************************
	//
	DLL_EXPORT int wlog( char *logname, const char *logmsg, int flag )
	{
		time_t		now;
		struct tm	*ts;
		char		DateTime[L_DATETIME+1];

		char		logbuf[L_LINESIZE+1];	// ��Ŵ��������־��Ϣ
		const char *pt = logmsg;
		int			num=0, ret;
		char		ch[3];
		char		tmpdir[BUFSIZ+1], tmpfile[BUFSIZ+1];
		int			fp;			// ��־�ļ����


		
		if ( !logname || !*logname || !logmsg || !*logmsg ) {
			return -1;
		}

		// ������־�ļ������б�Ҫ��Ҫ����Ŀ¼

		CreateDirOfFile(logname);

		time( &now );
		ts = localtime( &now );

		memset( DateTime, 0, L_DATETIME+1 );

		sprintf( DateTime, "%4d.%02d.%02d %02d:%02d:%02d",
							ts->tm_year+1900,	ts->tm_mon+1,	ts->tm_mday,
							ts->tm_hour,		ts->tm_min,		ts->tm_sec );

		// ����׼��д���ļ�
		memset( tmpdir, 0, BUFSIZ+1 );
		memset( tmpfile, 0, BUFSIZ+1 );

		GetDirectoryOfFile( tmpdir, logname );
		if ( strchr( logname, '\\' ) == NULL )
			sprintf(tmpfile,"%4d%02d%02d%s", ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, logname );
		else
			sprintf(tmpfile,"%s%4d%02d%02d%s", tmpdir, ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, logname+strlen(tmpdir) );

		if ((fp = open ( tmpfile, O_WRONLY | O_APPEND)) < 0 )
			if ((fp = open( tmpfile, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0 )
				return (-1);
			else
				chmod( logname, 0666 );


		// ======================

		memset( logbuf, 0, L_LINESIZE+1 );

		// ����Ϊ������־��Ϣ���Ա�һ����д����־�ļ��� �����ļ�����
		sprintf( logbuf, "%s ", DateTime );			// ��һ�ж���Ҫ������Ϣ

		if ( strlen(logmsg) <= L_LINESIZE-L_DATETIME-1) {
			sprintf( logbuf+strlen(logbuf), "%s\n", logmsg );
			write ( fp, logbuf, strlen(logbuf) ); 
		} else {
			while ( pt[0] != 0 ) {		// Դ��û�н���
				memset( ch, 0, 3 );
				ret = GetFirstWord( pt, ch );
				num += strlen( ch );
			 
				pt += strlen( ch );
				sprintf( logbuf + strlen(logbuf), "%s", ch );
				
				if ( num >= L_LINESIZE-L_DATETIME-1 ) {	// -1��Ϊ��ȥ�����ں���Ŀո����
					sprintf( logbuf + strlen(logbuf), "\n" );
					num = 0;

					write ( fp, logbuf, strlen(logbuf) ); 

					// ׼����һ�е�����
					memset( logbuf, 0, L_LINESIZE+1 );

					if ( pt[0] != 0 )			// ��ֹ���һ���ַ�ǡ��Ϊ��Ϣ����,��Ӧ�ü������ӿո�
						memset( logbuf, ' ', 20 );		// Ϊ��һ����ǰ����20���ո�
				}
			}

			if ( num != 0 )		// ����ʣ�ಿ��
				sprintf( logbuf + strlen(logbuf), "\n" );
				write ( fp, logbuf, strlen(logbuf) ); 

		}

		// ===================

	//	lockf (fd, F_LOCK, 0L);
		close ( fp);

		return 0;
	}

/*
int lwlog(char *logname,char *msg)
{
	int		fd,bufp,msgp,copylength,uncopylength,retval,chinese,i;
	char	DateTime[L_LogDateTime+1];
	char	msgbuf[((L_LogMsg+L_LogDateTime)/(L_LineSize-L_LogSpace-1)+1)*(L_LineSize+1)];
	time_t	now;
	struct tm *ts;

	time(&now);
	ts = localtime(&now);
	sprintf(DateTime,"%4d.%02d.%02d %02d:%02d:%02d ",ts->tm_year+1900,
		ts->tm_mon+1,ts->tm_mday,ts->tm_hour,ts->tm_min,ts->tm_sec);

	uncopylength = strlen(msg);
	if (uncopylength > L_LogMsg)
		uncopylength = L_LogMsg;
	retval = uncopylength;
	copylength = L_LineSize - L_LogDateTime;

	for (chinese = bufp = msgp = 0; copylength < uncopylength;
				chinese = 0,uncopylength -= copylength,
				copylength = L_LineSize - L_LogSpace){
		for (i = copylength; (i>0) && (msg[msgp+i-1] & 0x80); i--)
			chinese = (chinese == 0) ? 1:0;
		if (chinese)
			copylength--;
		memcpy (&msgbuf[bufp], &msg[msgp], copylength);
		bufp += copylength;
		msgp += copylength;
	}
	memcpy (&msgbuf[bufp], &msg[msgp], uncopylength);
	msgbuf[bufp+uncopylength] = '\n'; 
	msgbuf[bufp+uncopylength+1] = '\0';

	if ((fd = open (logname, O_WRONLY | O_APPEND)) < 0)
		if ((fd=open(logname, O_WRONLY | O_CREAT | O_APPEND, 0666)) < 0)
			return (-1);
		else
			chmod(logname,0666);
//	lockf (fd, F_LOCK, 0L);
	write (fd, DateTime, strlen(DateTime)); 
	write (fd, msgbuf, strlen(msgbuf));
	close (fd);
	return (retval);
}

*/

	//*********************************************************************************
	//  �������� : wlogEx()  
	//  ��    �� :
	//			wlog() ��������չ
	//  ��ڲ��� :
	//			char	* logname		��־�ļ���ȫ·��
	//			char	* logmsg			��־����
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		ֵΪ0 ��ʾ�ɹ�, ֵΪfalse��ʾʧ��
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.08.14
	//  Modify history (modifier and date) :
	//			
	//  ʹ��˵�� :
	//		������־�����ݳ��Ȳ��ó���1024���ֽ�, �������ֲ���Ԥ֪�Ĵ���
	//
	//		VB 6.0 �²���Ҫ���� wlogEx(), ��Ϊwlog()����ʵ�����µĹ���:
	//			...
	//			dim ret as long
	//			
	//			ret = wlog("c:\tmp\test.log","id=" & id & ",name=" & "mike", 1)
	//			...
	//
	//	ά��˵�� :
	//		�������ڲ����õ�wlog(), ����ʹ����wlog()��ȱʡ�÷�
	//
	//		Standard .h :
	//			stdarg.h    -------- va_list, va_start(), va_arg(), va_end(), vsprintf()
	//			stdio.h		-------- vsprintf()
	//			string.h    -------- memset()
	//		
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		�������øú����Ŀ⼰������:
	//			
	//*********************************************************************************
	//
	DLL_EXPORT int wlogEx( char *logname, const char *format, ... )
	{
		va_list		args;
		char		logbuf[1024];

		va_start( args, format );


		memset( logbuf, 0, 1024 );
		vsprintf( logbuf, format, args );

		wlog( logname, logbuf, 0x01 );

		va_end( args );

		return 0;
	}

	//*********************************************************************************
	//  �궨�� :wlogExDebug()  
	//  ��    �� :
	//			wlog() ��������չ, ���Գ���ʱʹ��. ��󷢲�ʱ���Բ�����(ͨ������ָ����)
	//  ��ڲ��� :
	//			char	* logname		��־�ļ���ȫ·��
	//			char	* logmsg			��־����
	//  ���ڲ��� :
	//			��
	//  �� �� ֵ :
	//			int		ֵΪ0 ��ʾ�ɹ�, ֵΪfalse��ʾʧ��
	//  Author   :
	//			Michael.Adams
	//  Create Date :
	//			2003.08.04
	//  Modify history (modifier and date) :
	//			
	//  ʹ��˵�� :
	//		������־�����ݳ��Ȳ��ó���1024���ֽ�, �������ֲ���Ԥ֪�Ĵ���
	//			ֻ����Ӧ���ж����˸ú�(_WLOGEX_DEBUG__), �Ż��¼��־. ���Խ�����, 
	//		����ɾ����ע�͸ú궨��, ��Ŀ��ϵͳ�в����¼�����õ���־��Ϣ
	//		�ù��ܵ�ʵ����Ϊ ͷ�ļ��ĺ궨�����, 
	//
	//	ά��˵�� :
	//		�������ڲ����õ�wlog(), ����ʹ����wlog()��ȱʡ�÷�
	//
	//		Standard .h :
	//			stdarg.h    -------- va_list, va_start(), va_arg(), va_end(), vsprintf()
	//			stdio.h		-------- vsprintf()
	//			string.h    -------- memset()
	//		
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		�������øú����Ŀ⼰������:
	//			
	//*********************************************************************************
	//
	/*
	int __stdcall wlogExDebug( char *logname, const char *format, ... )
	{

		#ifdef _WLOGEX_DEBUG__		// ֻ����Ӧ���ж����˸ú�, �Ż��¼��־.

		va_list		args;
		char		logbuf[1024];

		va_start( args, format );


		memset( logbuf, 0, 1024 );
		vsprintf( logbuf, format, args );

		wlog( logname, logbuf, 0x01 );

		va_end( args );

		#endif

		return 0;
	}
	*/
/*

// ***************************************************************************
//  �������� : GetFileLine(char *file, int flag)                           *
//  ��    �� :                                                             *
//             ��ȡԴ�ļ�����                                              *   
//  ��ڲ��� :                                                             *
//            char *file  :  Դ�ļ���                                      * 
//            int  flag   :  ��־  0: ��ʾ����Ҳ����                       * 
//                                 1: ���������                           * 
//  ���ڲ��� :                                                             *
//            ��                                                           * 
//  �� �� ֵ :                                                             * 
//            long :  �ɹ�����ԩ�ļ�������                                 * 
//				   :  ʧ�ܷ���-1                                           *
//  Author   :                                                             *
//            Adams                                                        * 
//  Create Date :                                                          *
//            2000.06.01                                                   *
//  Modify history (modifier and date) :                                   *
//                                                                         * 
//  ʹ��˵�� :                                                             *
//            ����ǰbuf��������ռ䲢��ʼ��                                *
//  Standard .h  :                                                         * 
//  Develope Tools:                                                        * 
//			  Visual C++ 6.0                                               * 
//  Environment:                                                           * 
//           1. Windows NT WorkStation SimpleChinese 4.0                   * 
// *************************************************************************
long GetFileLine(char *file,int	flag)
{
	long	count;
	FILE	*fp;
	char	buf[BUFSIZ+1];

	if ( (fp = fopen(file,"r")) == NULL ) {
		return -1;
	}

//	lockf(fp,F_LOCK,0L);

	fseek( fp, 0L, SEEK_SET );
	
	count = 0;
	while( !feof(fp) ) {
		memset(buf, 0, BUFSIZ+1 );
		fgets(buf,BUFSIZ,fp);

		if( (flag==F_EXCBLANK) && (buf[0] == '\n') ) continue;

		count ++;
	}

//	lockf(fp,F_ULOCK,0L);

	fclose(fp);

	return count;
}

//**************************************************************************
//  �������� : AddFileLine(char *file, char *msg)                          *
//  ��    �� :                                                             *
//             ���ļ�β,�����ļ���һ��                                     *   
//  ��ڲ��� :                                                             *
//            char *file  :  Դ�ļ���                                      * 
//            char *msg   :  Ҫ���ӵ���Ϣ                                  * 
//  ���ڲ��� :                                                             *
//            ��                                                           * 
//  �� �� ֵ :                                                             * 
//            int :  �ɹ�����0                                             * 
//				   :  ʧ�ܷ���-1                                           *
//  Author   :                                                             *
//            Adams                                                        * 
//  Create Date :                                                          *
//            2000.06.01                                                   *
//  Modify history (modifier and date) :                                   *
//                                                                         * 
//  ʹ��˵�� :                                                             *
//                                                                         *
//  Standard .h  :                                                         * 
//  Develope Tools:                                                        * 
//			  Visual C++ 6.0                                               * 
//  Environment:                                                           * 
//           1. Windows NT WorkStation SimpleChinese 4.0                   * 
//**************************************************************************
int AddFileLine(char *file, char * msg)
{
	FILE	*fp;

	if ( (fp = fopen(file,"a+")) == NULL ) {
		return -1;
	}

	fseek( fp, 0L, SEEK_END );
	
	fprintf(fp,msg);

	fclose(fp);

	return 0;
}

int BeginFuncMem(char *file, char *funcname)
{
	char			logbuf[BUFSIZ+1];

//	MEMORYSTATUS	memst;

//	GlobalMemoryStatus(&memst);

//	memset(logbuf, 0, BUFSIZ+1);
//	sprintf(logbuf, "�տ�ʼִ�к���:%s()ʱ, �ڴ�ʹ�����Ϊ:\n",funcname);

//	sprintf(logbuf+strlen(logbuf), "     ʹ�ðٷֱ� : %d\n", memst.dwMemoryLoad);
//	sprintf(logbuf+strlen(logbuf), ϵͳ�����ڴ湲�� : %d\n", memst.dwTotalPhys);
//	sprintf(logbuf+strlen(logbuf), "     ʣ���ڴ�Ϊ : %d\n", memst.dwAvailPhys);

//	wlog(file,logbuf);


	return 0;
}

int EndFuncMem(char *file, char *funcname)
{
	return 0;
}

int BeforeFuncMem(char *file, char *funcname)
{
	return 0;
}

int AfterFuncMem(char *file, char *funcname)
{
	return 0;
}

*/