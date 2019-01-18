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
    // ����Ľ������ļ��ڲ�ʹ��
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
    //      #     ����ķ���ȫ�����Ǵ������ݵ�, ��������, ��ʽ������
    //      #  2016.12.21:
    //      #      ɾ����IFCTRL1() �жϻ��е�ʵ�ַ�ʽ������֮ǰ�ķ���.
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

    // λ����Ϣӳ���
    typedef struct __posmap__ {
        long        curpos;
        
    }POSMAP;

    // �������ñ�  XREF ��Ϣ
    typedef struct __xref__ {
        int         retcode;
    }XREF;

    // trailer ��Ϣ�а���: ��object��ţ�object����,  startxref( xref λ����Ϣ)
    typedef struct __trailer__ {
        int         Root;       // ������ID
        int         Size;       // ��������. �ȶ�������Ŵ�1. ���Ϊ0�Ķ��� �������ݶ���.
        // Encrypt    // ����ļ���������
        int         Info;       // ��ѡ. ��¼�ĵ���Ϣ�Ķ���ID.   ������Ϣ�ȵ�
        char      * ID;         // ��ѡ�� Ϊ�˱�֤�ļ���Ψһ��, ǿ�ҽ����и�����. 
        long        Prev;       // ǰһ��Trailer�ĵ�ַ.
        long        xrefStm;    // ���õ�xref��ַ
        int         retcode;    // ������ӵ�����, �����жϸ�Trailer�Ƿ���Ч
    } TRAILER;

    //**************************************************************************
    //  �������� : parsePDF()
    //  ��    �� :
    //		���� pdf�ļ�, ��������Ľ�������desfile�ļ���.
    //  ��ڲ��� :
    //       desfile :      ����������ݴ�ŵ��ļ���
    //       srcfile :      ��Ҫ������pdf�ļ�
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         int         <0  ��ʾ����, -1 ��ʾ�ĸ�ʽ����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //			            
    //	Standard .h  :
    //         filemap.h -----------FILEMAP,  fm_seek()    
    //         str_tools.h ---------IsNumberStr() 
    //         �õ��ı��ļ��ڵ�API
    //             1. getStartxrefPos()    
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    int DLL_EXPORT parsePDF( char * desfile,  char * srcfile )
    {
        FILEMAP       * fm_p;
        long            start_pos;

        fm_p        = initFileMap( srcfile );
        print_fm( fm_p );

        start_pos   = getStartXrefPos( fm_p );          // ��ȡstartxref ������, Ҳ���ǵ�һ��xref ��λ��

        if ( start_pos < 0 )
            return -1; 
        
    }


    //һ��Ϊ�ڲ����õ�API
    //**************************************************************************
    //  �������� : getStartXrefPos()
    //  ��    �� :
    //		��ȡxref λ����Ϣ
    //  ��ڲ��� :
    //       FILEMAP    * fm_p
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         long         <0  ��ʾ����, -1 ��ʾ�ļ�ӳ��ָ�벻����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
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
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    long getStartXrefPos( FILEMAP * fm_p )
    {
        uchar     * buf = NULL;
        long        startxrefpos = -3;
         
        if ( !fm_p )       // ����ļ�ӳ��ָ��Ϊ���򷵻�-1
            return -1;
         
        // �ȶ�λ�������ļ�β48���ֽڴ�, һ��xref���ļ�β
        fm_seek( fm_p, fm_p->filelen - 48 );   

        while ( 1 ) {
            buf = fm_readLine( fm_p );
            if ( !buf )        
                return -2;             // ��ȡ����xref,�ļ���ʽ���ܲ���ȷ

            if ( strstr( buf, "startxref" ) ) {  // ���û�оͻ᷵�ؿ�ָ��, �����Ļ��ͻ᷵��λ��
                free( buf );       // �ͷ�buf�ڴ棬�ظ�����
                buf = fm_readLine( fm_p);   // "startxref" �����е���һ�о���ʵ�ʵ�xref���ڵ�λ����Ϣ

                if ( !buf )             // ���buf ΪNULL, ���ļ���ʽ����ȷ
                    return -2;
                
                if ( !IsNumberStr( buf ) ) {     // ������������ַ���, ����Ȼ���ļ���ʽ����ȷ, ע��, ���Ҫ�ͷ��ڴ�
                    free( buf );
                    return -2;
                }
                
                startxrefpos =  atol( buf );
                return startxrefpos;
                
             }
         }
    }


    //**************************************************************************
    //  �������� : getTrailer()
    //  ��    �� :
    //		��ȡ Trailer ��Ϣ
    //  ��ڲ��� :
    //       FILEMAP    * fm_p
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //         long         <0  ��ʾ����, -1 ��ʾ�ļ�ӳ��ָ�벻����. >=0 ��ʾ�ҵ�xrefλ��
    //  Author :
    //            Adams
    //  Create Date :
    //         2019.1.18
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
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
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    TRAILER * getTrailer( FILEMAP * fm_p )
    {
    }

