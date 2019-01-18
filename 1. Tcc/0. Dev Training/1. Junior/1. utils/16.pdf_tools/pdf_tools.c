    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    pdf_tools
    //
    // Description : about pdf parse operation.
    //
    // Interface :
    //      1.  parsePDF()  -- getStartxrefPos(), getxref(), getPages(), getCmapList, getCMAPs()
    //
    // 下面的仅供本文件内部使用
    //      2.  getStartxrefPos()
    //      3.  getxref()  ---- getTrailer()
    //      4.  getTrailer()
    //      5.  getPages()  --- getObjContent(), getPageleaf()
    //      6.  getObjContent()
    //      7.  getPageleaf()
    //      8.  getCmapList() ---- getObjContent(),isType0()
    //      9.  isType0()
    //      10. getCMAPs() -----getObjContent(), getItemOfObj(),procType0Stream()
    //      11. getItemOfObj() -- getObjContent
    //      12. procType0Stream() ---  getItemOfObj(), decompress()
    //      13. decompress() --- import zlib(zlib.decomporess())
    //      14. processPage() ---- getItemOfObj()
    //      ##########################################
    //      #     下面的方法全部都是处理内容的, 包括解码, 格式调整等
    //      #  2016.12.21:
    //      #      删除了IFCTRL1() 判断换行的实现方式不再用之前的方法.
    //      # 15. decode()---  file_tools.initStreamBuf(), .readLineST( streambuf)
    //      #                  self.processBT(), .processBDC()
    //      # 16. processBT() -- self.processBDC(), .processRE(), /*processQ()*/
    //      #                    self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
    //      #                    file_tools.readLineST()
    //      # 17. processBDC() -- self.processBT(), .processRE(), /*processQ()*/
    //      #                    self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
    //      #                    file_tools.readLineST()
    //      # 18. hasText()
    //      # 19. processText()---  self.ifCRLF(), .processTj(), .processTJ()
    //      # 20. processTj() --- none
    //      # 21. processTJ() ---
    //      # 23. processRE()
    //      # 24. processQ()
    //      # 25. hasTxtPosition()
    //      # 26. precessTxtPosition()
    //      # 27. processTd()
    //      # 28. processTD()
    //      # 29. getOriginXY()
    //      # 30. processTm()
    //      # 31. processTstar()
    //      # 99. rlen()
    //      
    //  
    #include "file_tools.h"
    #include "filemap.h"
    #include "str_tools.h"
    #include "pdf_tools.h"

    #define DLL_EXPORT __declspec(dllexport)

    // 位置信息映射表
    typedef struct __posmap__ {
        long        curpos;
        
    }POSMAP;

    // 交叉引用表  XREF 信息
    typedef struct __xref__ {
        int         retcode;
    }XREF;

    // trailer 信息中包含: 根object编号，object数量,  startxref( xref 位置信息)
    typedef struct __trailer__ {
        int         Root;       // 根对象ID
        int         Size;       // 对象数量. 比对象最大编号大1. 编号为0的对象 不是数据对象.
        // Encrypt    // 如果文件加密则有
        int         Info;       // 可选. 记录文档信息的对象ID.   作者信息等等
        char      * ID;         // 可选。 为了保证文件的唯一性, 强烈建议有该内容. 
        long        Prev;       // 前一个Trailer的地址.
        long        xrefStm;    // 可用的xref地址
        int         retcode;    // 额外添加的内容, 用来判断该Trailer是否有效
    } TRAILER;

    //**************************************************************************
    //  函数名称 : parsePDF()
    //  功    能 :
    //		解析 pdf文件, 将解析后的结果存放在desfile文件中.
    //  入口参数 :
    //       desfile :      解析后的内容存放的文件名
    //       srcfile :      需要解析的pdf文件
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         int         <0  表示出错, -1 表示文格式不对. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			            
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr() 
    //         用到的本文件内的API
    //             1. getStartxrefPos()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    int DLL_EXPORT parsePDF( char * desfile,  char * srcfile )
    {
        FILEMAP       * fm_p;
        long            start_pos;

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        start_pos   = getStartXrefPos( fm_p );          // 获取startxref 的内容, 也就是第一个xref 的位置

        if ( start_pos < 0 )
            return -1; 
        
    }


    //一下为内部调用的API
    //**************************************************************************
    //  函数名称 : getStartXrefPos()
    //  功    能 :
    //		获取xref 位置信息
    //  入口参数 :
    //       FILEMAP    * fm_p
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         long         <0  表示出错, -1 表示文件映射指针不可用. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			            
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    long getStartXrefPos( FILEMAP * fm_p )
    {
        uchar     * buf = NULL;
        long        startxrefpos = -3;
         
        if ( !fm_p )       // 如果文件映射指针为空则返回-1
            return -1;
         
        // 先定位到距离文件尾48个字节处, 一般xref在文件尾
        fm_seek( fm_p, fm_p->filelen - 48 );   

        while ( 1 ) {
            buf = fm_readLine( fm_p );
            if ( !buf )        
                return -2;             // 读取不到xref,文件格式可能不正确

            if ( strstr( buf, "startxref" ) ) {  // 如果没有就会返回空指针, 包含的话就会返回位置
                free( buf );       // 释放buf内存，重复利用
                buf = fm_readLine( fm_p);   // "startxref" 所在行的下一行就是实际的xref所在的位置信息

                if ( !buf )             // 如果buf 为NULL, 则文件格式不正确
                    return -2;
                
                if ( !IsNumberStr( buf ) ) {     // 如果不是数字字符串, 则仍然是文件格式不正确, 注意, 这儿要释放内存
                    free( buf );
                    return -2;
                }
                
                startxrefpos =  atol( buf );
                return startxrefpos;
                
             }
         }
    }


    //**************************************************************************
    //  函数名称 : getTrailer()
    //  功    能 :
    //		获取 Trailer 信息
    //  入口参数 :
    //       FILEMAP    * fm_p
    //  出口参数 :
    //            无
    //  返 回 值 :
    //         long         <0  表示出错, -1 表示文件映射指针不可用. >=0 表示找到xref位置
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //			            
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    TRAILER * getTrailer( FILEMAP * fm_p )
    {
    }

