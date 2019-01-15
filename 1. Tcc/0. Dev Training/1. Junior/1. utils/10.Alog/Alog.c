   //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.，LTD                
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
										// 计算文件行数时包括空行
//	#define		F_EXCBLANK		1		// 计算文件行数时不包括空行
//
	#define DLL_EXPORT __declspec(dllexport)

	//*********************************************************************************
	//  函数名称 : wlog()  
	//  功    能 :
	//			向指定日志文件写日志
	//  入口参数 :
	//			char	* logname		日志文件，全路径
	//			char	* logmsg			日志内容
	//			flag : 0x00
	//					 ||
	//					 |+----  0  表示有日志服务器, 1(缺省值) 表示没有日志服务器(暂不支持)
	//					 +-----  0 表示不受每行长度的限制, 不自动分行
	//							 1 (缺省值) 表示每行最大长度为L_LINESIZE, 自动分行，日志信息
	//								会分段写入日志中，除第一行有时间信息外，其它行没有时间信
	//								息， 担任空留L_DATETIME+1个空格，便于察看
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		值为0 表示成功, 值为false表示失败
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.06.19
	//  Modify history (modifier and date) :
	//			2002.07.04	-------  增加维护文档以及VB6.0调用接口说明
	//  使用说明 :
	//	
	//			logmsg 若为空, 则失败返回-1若logmsg长度超过L_LOGBUF,则不处理L_LOGBUF之外的信息。
	//			只支持logmsg 为TEXT的格式，不支持Binary格式
	//			对于给定的日志文件，系统自动为文件名称加入当前日期，用于表示日志生成的时间
	//			同时减少日志文件的大小
	//
	//		VB 6.0 调用格式:
	//			Private Declare Function wlog Lib "alog.dll" (ByVal logname As String, ByVal logmsg As String, ByVal flag As Long) As Long
	//				或
	//			Private Declare Function wlog Lib "alog.dll" (ByVal logname As String, ByVal logmsg As String, ByVal flag As Long) As Integer
	//
	//		需要注意的是, c/c++调用时, 第三个参数可以省略, 但是VB下调用时, 第三个参数不可以省略
	//		将库 Astring.dll, FileApi.dll (要求版本必须一致)拷到与Alog.dll同一目录下, 也可以拷到System32下.
	//
	//		VB 6.0 调用举例:
	//			...
	//			Dim buf As String
	//			Dim ret As Long
	//			...
	//			buf = "12345678901234567890123456789012345678901234567890123456789孩" _
	//				& "12345678901234567890123456789012345678901234567890123456789子" _
    //				& "12345678901234567890123456789012345678901234567890123456789你" _
    //				& "12345678901234567890123456789012345678901234567890123456789好" _
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
	//	维护说明 :
	//
	//		Standard .h :
	//			stdio.h----------sprintf()
	//			time.h-----------time_t, struct tm,time(), localtime()
	//			string.h---------memset(),strlen(), strchr(), 
	//			Length.h---------L_DATETIME,
	//			Astring.h--------GetFirstWord()
	//			FileApi.h--------CreateDirOfFile()
	//			fcntl.h----------O_WRONLY, O_APPEND, O_CREAT
	//			io.h-------------open()、write()、close()
	//		Develope Tools :
	//			Visual C++ 6.0 /StuSP5
	//		
	//		Environment:
	//           1. Windows 2000 Advanced Server/SP2
	//			
	//		其他调用该函数的库及函数名:
	//			
	//*********************************************************************************
	//
	DLL_EXPORT int wlog( char *logname, const char *logmsg, int flag )
	{
		time_t		now;
		struct tm	*ts;
		char		DateTime[L_DATETIME+1];

		char		logbuf[L_LINESIZE+1];	// 存放处理后的日志信息
		const char *pt = logmsg;
		int			num=0, ret;
		char		ch[3];
		char		tmpdir[BUFSIZ+1], tmpfile[BUFSIZ+1];
		int			fp;			// 日志文件句柄


		
		if ( !logname || !*logname || !logmsg || !*logmsg ) {
			return -1;
		}

		// 处理日志文件，若有必要需要创建目录

		CreateDirOfFile(logname);

		time( &now );
		ts = localtime( &now );

		memset( DateTime, 0, L_DATETIME+1 );

		sprintf( DateTime, "%4d.%02d.%02d %02d:%02d:%02d",
							ts->tm_year+1900,	ts->tm_mon+1,	ts->tm_mday,
							ts->tm_hour,		ts->tm_min,		ts->tm_sec );

		// 下面准备写入文件
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

		// 以下为处理日志信息，以便一次性写入日志文件， 减少文件操作
		sprintf( logbuf, "%s ", DateTime );			// 第一行都需要日期信息

		if ( strlen(logmsg) <= L_LINESIZE-L_DATETIME-1) {
			sprintf( logbuf+strlen(logbuf), "%s\n", logmsg );
			write ( fp, logbuf, strlen(logbuf) ); 
		} else {
			while ( pt[0] != 0 ) {		// 源串没有结束
				memset( ch, 0, 3 );
				ret = GetFirstWord( pt, ch );
				num += strlen( ch );
			 
				pt += strlen( ch );
				sprintf( logbuf + strlen(logbuf), "%s", ch );
				
				if ( num >= L_LINESIZE-L_DATETIME-1 ) {	// -1是为了去掉日期后面的空格计数
					sprintf( logbuf + strlen(logbuf), "\n" );
					num = 0;

					write ( fp, logbuf, strlen(logbuf) ); 

					// 准备下一行的内容
					memset( logbuf, 0, L_LINESIZE+1 );

					if ( pt[0] != 0 )			// 防止最后一个字符恰好为信息结束,不应该继续添加空格
						memset( logbuf, ' ', 20 );		// 为下一行提前添加20个空格
				}
			}

			if ( num != 0 )		// 处理剩余部分
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
	//  函数名称 : wlogEx()  
	//  功    能 :
	//			wlog() 函数的扩展
	//  入口参数 :
	//			char	* logname		日志文件，全路径
	//			char	* logmsg			日志内容
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		值为0 表示成功, 值为false表示失败
	//  Author   :
	//			Adams
	//  Create Date :
	//			2002.08.14
	//  Modify history (modifier and date) :
	//			
	//  使用说明 :
	//		单条日志的内容长度不得超过1024个字节, 否则会出现不可预知的错误
	//
	//		VB 6.0 下不需要调用 wlogEx(), 因为wlog()可以实现如下的功能:
	//			...
	//			dim ret as long
	//			
	//			ret = wlog("c:\tmp\test.log","id=" & id & ",name=" & "mike", 1)
	//			...
	//
	//	维护说明 :
	//		本函数内部调用的wlog(), 并且使用了wlog()的缺省用法
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
	//		其他调用该函数的库及函数名:
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
	//  宏定义 :wlogExDebug()  
	//  功    能 :
	//			wlog() 函数的扩展, 调试程序时使用. 最后发布时可以不编译(通过定义指定宏)
	//  入口参数 :
	//			char	* logname		日志文件，全路径
	//			char	* logmsg			日志内容
	//  出口参数 :
	//			无
	//  返 回 值 :
	//			int		值为0 表示成功, 值为false表示失败
	//  Author   :
	//			Michael.Adams
	//  Create Date :
	//			2003.08.04
	//  Modify history (modifier and date) :
	//			
	//  使用说明 :
	//		单条日志的内容长度不得超过1024个字节, 否则会出现不可预知的错误
	//			只有在应用中定义了该宏(_WLOGEX_DEBUG__), 才会记录日志. 调试结束后, 
	//		可以删除或注释该宏定义, 则目标系统中不会记录调试用的日志信息
	//		该功能的实现作为 头文件的宏定义存在, 
	//
	//	维护说明 :
	//		本函数内部调用的wlog(), 并且使用了wlog()的缺省用法
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
	//		其他调用该函数的库及函数名:
	//			
	//*********************************************************************************
	//
	/*
	int __stdcall wlogExDebug( char *logname, const char *format, ... )
	{

		#ifdef _WLOGEX_DEBUG__		// 只有在应用中定义了该宏, 才会记录日志.

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
//  函数名称 : GetFileLine(char *file, int flag)                           *
//  功    能 :                                                             *
//             获取源文件行数                                              *   
//  入口参数 :                                                             *
//            char *file  :  源文件名                                      * 
//            int  flag   :  标志  0: 表示空行也计算                       * 
//                                 1: 不计算空行                           * 
//  出口参数 :                                                             *
//            无                                                           * 
//  返 回 值 :                                                             * 
//            long :  成功返回冤文件的行数                                 * 
//				   :  失败返回-1                                           *
//  Author   :                                                             *
//            Adams                                                        * 
//  Create Date :                                                          *
//            2000.06.01                                                   *
//  Modify history (modifier and date) :                                   *
//                                                                         * 
//  使用说明 :                                                             *
//            调用前buf必须申请空间并初始化                                *
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
//  函数名称 : AddFileLine(char *file, char *msg)                          *
//  功    能 :                                                             *
//             自文件尾,增加文件的一行                                     *   
//  入口参数 :                                                             *
//            char *file  :  源文件名                                      * 
//            char *msg   :  要增加的信息                                  * 
//  出口参数 :                                                             *
//            无                                                           * 
//  返 回 值 :                                                             * 
//            int :  成功返回0                                             * 
//				   :  失败返回-1                                           *
//  Author   :                                                             *
//            Adams                                                        * 
//  Create Date :                                                          *
//            2000.06.01                                                   *
//  Modify history (modifier and date) :                                   *
//                                                                         * 
//  使用说明 :                                                             *
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
//	sprintf(logbuf, "刚开始执行函数:%s()时, 内存使用情况为:\n",funcname);

//	sprintf(logbuf+strlen(logbuf), "     使用百分比 : %d\n", memst.dwMemoryLoad);
//	sprintf(logbuf+strlen(logbuf), 系统物理内存共有 : %d\n", memst.dwTotalPhys);
//	sprintf(logbuf+strlen(logbuf), "     剩余内存为 : %d\n", memst.dwAvailPhys);

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
