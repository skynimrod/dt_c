    //***************************************************************************
    //                                                                          
    //                 Copyright (C) 2004-2004 AXCEL TECH CO.，LTD                
    //                           All Rights Reserved                            
    //                                                                          
    //***************************************************************************
    //***************************************************************************
	//
	// 模块		: FileApi.H
	// 
	// 描述		: 标准公用代码API, 有关文件操作访问的应用接口函数, SDK版本, 可以用在许多工具中.
	//				包含对目录的访问操作
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

	#ifndef _FILEAPI_H__  // define FileApi.h Start {
	#define _FILEAPI_H__

	int	__stdcall	GetFullPath( char * in, char *out, int outlen );

	int __stdcall	GetTmpFile( char	*tmpfile );
	bool __stdcall	IfFileExist( char * szFileName );
	unsigned long __stdcall GetSizeOfFile( char * filename );
	int __stdcall	GetNoPathFileName( char * filename, char * buf );

	unsigned long __stdcall GetCapacityOfDir( char * Dirstr );
	void __stdcall	GetDirectoryOfFile( char * outstr, char * instr );
	int __stdcall	CreateDirOfFile( char * file );
	bool __stdcall	IfDirectoryExist( char * Dir );
	bool __stdcall	CopyDir( char *SourceDir, char *DestDir );
	bool __stdcall	ifSubDir( char * SourceDir, char * DestDir );
	bool __stdcall	RemoveDir( char *Dirstr );

	bool __stdcall CreateCopyRightInfo( char *filename, char *period, char *corporation, char *author, char *remstr );
	int __stdcall AppendFile( char *s_filename,char *d_filename );

	#endif // define FileApi.h End }
