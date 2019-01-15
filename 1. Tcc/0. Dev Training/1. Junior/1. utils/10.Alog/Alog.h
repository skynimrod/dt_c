    //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2001-2002 AQUGEN TECH CO.£¬LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// Module File	: Alog.H
	// 
	// Description	: LOG  Management API, the interface function about the visiting log file. 
	//			There is SDK Version, can be used in many tools.
	//
	// Author	: Michael.Adams
	//
	// Function	:
	//			
	//
	// Modified History	:
	//		2003.08.05 :
	//			Add the Defination of the function wlogExDebug() . used programming in Debugging status
	// Notes	:
	//		
	//***************************************************************************
	//
	//
	#ifndef _ALOG_H__		 // define Alog.h Start {
	#define _ALOG_H__



	int wlog( char *logname, const char *logmsg, int flag=0x01 );
	int wlogEx( char *logname, const char *format, ... );

	/*
	int wlogExDebug( char *logname, const char *format, ... )
	{

		#ifdef _WLOGEX_DEBUG__

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
	int __stdcall lwlog(char *logname,char *msg);

	long GetFileLine(char *file,int flag);
	int AddFileLine(char *file, char * msg);

	int BeginFuncMem(char *file, char *funcname);
	int EndFuncMem(char *file, char *funcname);

	int BeforeFuncMem(char *file, char *funcname);
	int AfterFuncMem(char *file, char *funcname);

*/
	#endif		// define Alog.h End }
