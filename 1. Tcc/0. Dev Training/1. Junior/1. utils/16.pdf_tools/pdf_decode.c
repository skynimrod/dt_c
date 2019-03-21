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
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "zlib.h"
    #include "pdf_tools.h"

    // ���溯�������ݽ�������
    void processTd( char * buf, CUR_XY * cur_xy_p, TM * tm_p );
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ); 
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p );
    void processTm( char *buf, CUR_XY * cur_xy_p, TM * tm_p );
    void processCm( char * buf, CM * cm_p );
    void processTstar( char * buf, CUR_XY * cur_xy_p );

    bool hasTxtPosition( char * buf );
    void processTxtPosition( char *buf, CUR_XY * cur_xy_p, TM * tm_p );

    char * processBT( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processBDC( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processRE( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p );
    char * processTf( char * tmpbuf, DECODE * decode_p );
    
    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p );
    char * processTJ( char *desbuf, char *srcbuf, CMAP * cmap_p );

    bool   hasText( char * buf );
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p );
    
    char * decode( PAGES * pages_p, char * buf, uLong dlen, char * filename ) ;

    

    #define CHECK_ERR(err, msg) { \
        if (err != Z_OK) { \
            fprintf(stderr, "%s error: %d\n", msg, err); \
            exit(1); \
        } \
    }
    #define L_TMPBUF        256
        
    //###########################################################################
    //#  ���²���Ϊ���� ��ѹ�� ҳ�����ݲ���
    /*
     * 15. decode()
     *      �Ը������ֽ������н���
     * ˼·:
     *      1. ��ÿһ���ֵĴ���ģ�黯
     *      2. ���һ��(I��)�� processBDC, processBT�� ���еĴ�������BDC/EMC, BT/ET ֮�䣬 
     *         ��2��֮��Ҳ�ụ��Ƕ��
     *      3. II�� ������: 
     *          processTD, processTd, processTm, processT*        �ı�λ��
     *          processTJ, processTj, processQuote, processDblQuote         �ı���ʾ, �������ǵ�����, ˫���ŵĴ���
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  �ı�״̬
     *          ���������
     *      2016.12.08:
     *          ��������ֽ�ŵļ������������ݱ���, �� ���������ݴ����һ������ֽ�ŵ��ڴ��У�Ȼ�����������ݼ���.
     *          virtual_Page={["Y����ֵ": ��������],}
     *          Ȼ�������ʱ��ֻҪ��virtual_Page ���չؼ������������������.
     *          ���괦��: ��ǰ������������������е�ĳ���������С��1, �����Ϊͬһ�� �� ��Ϊ��С������Ҳ�����1������
     *          {Y:["��������","����", ��λ����]},
     *               ��λ���������ֵ�����ĳ���, �����ж���һ�����Ƿ���Ҫ�������֮���зָ���
     *               �����һ�����ֵ���ʼx����������ֵ�x+len С�ڵ�λ����, ����Ҫ�ָ���.
     *      2016.12.21:
     *          ֮ǰ�Ĵ���, �����кܶ಻���Ƶĵط�, ��Ҫ��û�а���pdf �Ļ��������˼·. �����������˼·, ���� ͼ��״̬stack ʵ��.
     *            ��GraphStateStack ��¼��ǰѹջ������ ͼ��״̬, cur_graph_state ������¼��ǰ��ͼ��״̬. ���ǵ�ֻ�����ı�,
     *            ͼ��״ֻ̬��¼ tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */

    /*
     * # ������Ϣ, �����Ϣ(��ʼ��ҳü��û�е�, -1����ҳü���), �ı���Ϣ, ת������
    // ������Ϣ
    typedef struct __cur_xy__ {
        int     x;
        int     y;
    }CUR_XY;

    typedef struct __cellMap__ {
        // ��Ԫ����Ϣ
    }CELLMAP;

    typedef struct __cellIndexMap__{
        // ��Ԫ��������Ϣ
    }CELLINDEXMAP;

    typedef struct __textMap__ {
        // �ı�ӳ���
    } TEXTMAP;

    typedef struct __tm__ {
        // tm ��Ϣ
    }TM:

    typedef struct __tf__ {
        // tf ��Ϣ
    }TF;
    */
    void freeDecode( DECODE * decode_p )
    {
        CELL    * cp;
        TEXT    * tp;
        
        if ( !decode_p )
            return;

        // 1. �ͷ� cellMap ��������ڴ�
        cp = decode_p->cp;
        
        while ( cp ) {
            if ( cp->txtTotal > 0 ) 
                free( cp->tps );
            cp = cp->next;           
        } 

        // 2. �ͷ� textMap��������ڴ�
        tp = decode_p->tp;
        
        while ( tp ) {
            if ( tp->buf ) 
                free( tp->buf );
            tp = tp->next;           
        } 

        //
        // 3. �ͷ�decode_p
        free( decode_p );

    }

    /*
     * 15. decode()
     *      �Ը������ֽ������н���
     * ˼·:
     *      1. ��ÿһ���ֵĴ���ģ�黯
     *      2. ���һ��(I��)�� processBDC, processBT�� ���еĴ�������BDC/EMC, BT/ET ֮�䣬 
     *         ��2��֮��Ҳ�ụ��Ƕ��
     *      3. II�� ������: 
     *          processTD, processTd, processTm, processT*        �ı�λ��
     *          processTJ, processTj, processQuote, processDblQuote         �ı���ʾ, �������ǵ�����, ˫���ŵĴ���
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  �ı�״̬
     *          ���������
     *      2016.12.08:
     *          ��������ֽ�ŵļ������������ݱ���, �� ���������ݴ����һ������ֽ�ŵ��ڴ��У�Ȼ�����������ݼ���.
     *          virtual_Page={["Y����ֵ": ��������],}
     *          Ȼ�������ʱ��ֻҪ��virtual_Page ���չؼ������������������.
     *          ���괦��: ��ǰ������������������е�ĳ���������С��1, �����Ϊͬһ�� �� ��Ϊ��С������Ҳ�����1������
     *          {Y:["��������","����", ��λ����]},
     *               ��λ���������ֵ�����ĳ���, �����ж���һ�����Ƿ���Ҫ�������֮���зָ���
     *               �����һ�����ֵ���ʼx����������ֵ�x+len С�ڵ�λ����, ����Ҫ�ָ���.
     *      2016.12.21:
     *          ֮ǰ�Ĵ���, �����кܶ಻���Ƶĵط�, ��Ҫ��û�а���pdf �Ļ��������˼·. �����������˼·, ���� ͼ��״̬stack ʵ��.
     *            ��GraphStateStack ��¼��ǰѹջ������ ͼ��״̬, cur_graph_state ������¼��ǰ��ͼ��״̬. ���ǵ�ֻ�����ı�,
     *            ͼ��״ֻ̬��¼ tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */
    char * decode( PAGES * pages_p, char * buf, uLong dlen, char * filename ) 
    {
        char        *   retbuf;
        char        *   tmpbuf;       
        MEMORYMAP   *   mm_p;
        DECODE      *   decode_p;

        retbuf = (char *)calloc( dlen, 1 ); // ȷ��������ڴ��㹻��
        
        decode_p = (DECODE *)calloc( sizeof( DECODE ), 1 );
        
        mm_p = initMemoryMap( buf, dlen );

        while( 1 ) {    // ע��: ������жϲ���if else, ��Ϊ�п���һ�а������ֲ�����
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf( "decode() : tmpbuf = %s\n", tmpbuf );
            
            if ( strstr( tmpbuf, "BT" ) ) {         // ����� ����"BT"/"ET"
                processBT( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            if ( strstr( tmpbuf, "BDC" ) ) { // �������"BDC"/"EMC"
                processBDC( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            
            // re �Ĵ���Ӧ����BT/ET, BDC/EDC ֮��, Ҳ���ǵ�Ԫ��.
            if ( strstr( tmpbuf, "re" )  ) {   //����"re"
                processRE( mm_p,  &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            }
            
            if ( strstr( tmpbuf, "Tf") ) {
                processTf( tmpbuf, decode_p );
            }
            
            if ( strstr(tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) {
                memset( decode_p->tm, 0, sizeof(TM) );
            }
            
            free( tmpbuf );
        }

        freeMemoryMap( mm_p );
        
        // ���������������������������, �����������¹���
        //
        //
        printf("----------------Hello decode()--------------------------\n");
        printCellMap( decode_p->cp );
        printTextMap( decode_p->tp );
        
        memset( retbuf, 0, dlen );
        retbuf = (char *)procText4Table( retbuf, dlen, decode_p->tp, decode_p->cp );

        printCellMap( decode_p->cp );
        printTextMap( decode_p->tp );

        printf("============������ĸ�ҳ����Ϊ:\n%s\n", retbuf);

        freeDecode( decode_p );
        
        return retbuf;
    }


    /*
     * 16. processBT()
     *      ���� BT/ET �ڵ�����
     * ��ڲ���:
     *      mm-p           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      pages_p        pages_p��������ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      ��
     * ˵��:
     *      ������̻���ͬBDC һ�£� �����ڲ�������BT���ж�, ���ǰ�����BDC�����Ƕ��
     *      ���������ݴ�����Ķ����������� �ļ�����
     * 2016.12.12:
     *      Tm ��������ת������ϵ. [a b c d e f], ����һ��list Tm, ������µ�ת������
     *      Td/TD ������ʵ��Ϊ
     *           x' = a * x + c * y + e
     *           y' = b * x + d * y + f
     * depends on:
     *       processBDC(), processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     *      
    # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {���:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {���:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # һֱ��ŵ������µ�tm����
     * 
     */
    char * processBT( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p )
    {
        char    * tmpbuf;
        CUR_XY  * cur_xy_p  = & decode_p->cur_xy;
        TM      * tm_p      = & decode_p->tm;

        printf("processBT() buf = %s\n", srcbuf );
        while ( 1 ) {
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf("processBT() tmpbuf = %s\n", tmpbuf );
            
            //# 1. ���� ҳü��ҳ�ţ� ˮӡ
            //#    �ж��Ƿ����  /Artifact && ��/Top   /Bottom   /watermark����һ 
            //#     ��������� ����˺����BDC, EMC ֮�������
            //# 2. ������ҳü��ҳ��, ���������ҲҪ����, ���������������ִ���.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # ҳü  , BDC ����ʼ��ʶ
                while( tmpbuf ) {
                    printf("ҳü����. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# ҳü�����ı�ʶ
                        break;
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );

                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    else if ( strstr( tmpbuf, "Tm" )  )
                        processTm( tmpbuf, cur_xy_p, tm_p );                        
                }
            }   

            else if ( strstr( tmpbuf, "/Bottom" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // ҳ�ŵĴ���
                while ( 1 ) {       // # �����ѭ���жϱ������ѭ���жϿɶ���Ҫ��һ��
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# ҳ�Ž����ı�ʶ
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BDC" ) ) {       // # һ������  ���� BDC/EMC  ���ֲ���
                processBDC( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # ��� �� re ��Ϣ�� ��ʾ�Ǳ��, ��ȡcell ��Ϣ, �������Tm, ����cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # ������q ��Ϣ, ��ô��ʾ���浱ǰ��ͼ��״̬��Ϣ, ����������Ϣ, Ȼ��Q �ٻָ�
            /* ��ʱ������q/Q, ��Ϊre ʹ�õ��Ǿ�������
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */

            //# ����������Ϣ,���������ǵ�ǰ������Ҫ��type0����
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # ���buf �� �ı�λ����Ϣ, ������õ�ǰ��xy ����
                # 2016.12.13:
                #   ����������λ��, ���ֻ����Tm Ҳ����ת��������Ϣ, ��֤���µ�ת������������. �ı����������ı��ڲ�����
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm ������tm ������ͬ, ����Tm�Ĵ������ȿ�
            //#    self.processCm( tmpbuf, tm )
            
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );

            //# ���buf ���ı���Ϣ (���ʱ��Ӧ��û��BDC/BT, ���������desbuf Ӧ���滻Ϊ&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "ET" ) ) {  // # ���Ľ����ı�ʶ
                printf("����ET, BT/ET �������. tmpbuf=%s\n", tmpbuf );
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }
    
    /*
     * 17. processBDC()
     *      ���� BDC/EMC �ڵ�����
     * ��ڲ���:
     *      streambuf           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      ��
     * ˵��:
     *      1. ����ҳü/ҳ��, ����еĻ�
     *      2. �����Td/TD/Tm/T*, �ͻ�ȡλ����Ϣ, ����֮ǰ�����λ����Ϣ���бȽ�, ���ж��Ƿ�Ҫ����
     *      3. �����Tj,TJ, ', " , ��ʾ�ı�����, ժȡ�ı�����, �����Ҫ��������н���
     *      4. �����re, һ����Ӧf*, Ҳ���ǻ����β���  
     *          x, y, width, height    re   // x,y �����½ǵ�����
     * depends on:
     *       self.processBT(), .processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     * 
     */
    char * processBDC( MEMORYMAP *mm_p, char * desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p  )
    {
        char    * tmpbuf;
        CUR_XY  * cur_xy_p  = & decode_p->cur_xy;
        TM      * tm_p      = & decode_p->tm;

        printf("processBDC() buf = %s\n", srcbuf );
        while ( 1 ) {
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf("processBDC() tmpbuf = %s\n", tmpbuf );
            
            //# 1. ���� ҳü��ҳ�ţ� ˮӡ
            //#    �ж��Ƿ����  /Artifact && ��/Top   /Bottom   /watermark����һ 
            //#     ��������� ����˺����BDC, EMC ֮�������
            //# 2. ������ҳü��ҳ��, ���������ҲҪ����, ���������������ִ���.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # ҳü  , BDC ����ʼ��ʶ
                while( tmpbuf ) {
                    printf("ҳü����. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# ҳü�����ı�ʶ
                        break;
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );

                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    else if ( strstr( tmpbuf, "Tm" )  )
                        processTm( tmpbuf, cur_xy_p, tm_p );                        
                }
            }   

            else if ( strstr( tmpbuf, "/Bottom" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // ҳ�ŵĴ���
                while ( 1 ) {       // # �����ѭ���жϱ������ѭ���жϿɶ���Ҫ��һ��
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# ҳ�Ž����ı�ʶ
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BT" ) ) {       // # һ������  ���� BT/ET  ���ֲ���
                processBT( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # ��� �� re ��Ϣ�� ��ʾ�Ǳ��, ��ȡcell ��Ϣ, �������Tm, ����cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # ������q ��Ϣ, ��ô��ʾ���浱ǰ��ͼ��״̬��Ϣ, ����������Ϣ, Ȼ��Q �ٻָ�
            /* ��ʱ������q/Q, ��Ϊre ʹ�õ��Ǿ�������
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );


            //# ����������Ϣ,���������ǵ�ǰ������Ҫ��type0����
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # ���buf �� �ı�λ����Ϣ, ������õ�ǰ��xy ����
                # 2016.12.13:
                #   ����������λ��, ���ֻ����Tm Ҳ����ת��������Ϣ, ��֤���µ�ת������������. �ı����������ı��ڲ�����
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm ������tm ������ͬ, ����Tm�Ĵ������ȿ�
            //#    self.processCm( tmpbuf, tm )
            
            //# ���buf ���ı���Ϣ (���ʱ��Ӧ��û��BDC/BT, ���������desbuf Ӧ���滻Ϊ&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "EMC" ) ) {  // # ���Ľ����ı�ʶ
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }

    /*
     * 23. processRE()
     *      ���� re �ڵ�����
     * ��ڲ���:
     *      buf                 ��ǰ����TD ��һ����Ϣ
     * ���ڲ���:
     *      cur_cell            ��ϣ��, ��¼��Ԫ������ "x":x����ֵ, "y":y����ֵ, "w":width, "h":height
     * ����ֵ:
     *      ��
     * ˵��:
     *      �����re, ��˵���Ǳ��, ��¼Tj/TJ/'/" ֮ǰ�����µ�re ������Ϊ���µ�cell ��Ϣ��
     *  ��������ж��re, ��ô�������µ�re��Ϣ��, cur_cell Ҳ��ŵ������µ�re��Ϣ
     *      �������Tm, ����cur_cell=[:], pre_cell=[:]
     *      x, y, width, height  re   ,  ��һ������. x,y �����½�����.
     *      RE ���ص���CELL �Ĵʵ�, {'re1':[x,y,w,h],'re2',}
     * 2016.12.10:
     *      �ı�˼·, ÿ�δ���re��ʱ�򽫻�ȡ��cell��Ϣ�����һ��cellMap��, �������ݽ���������,
     *      �ٽ�� textMap��cellMap���������Լ��������ֵĻ�������.
     *      1. ��Щpdf�ĵ���re ��x�Ǹ���, w �ߴ�ü�ǧ, ����:
     *              -1638.4 72 3795.5 39.1 re
     *      �������������Ƶ�, �������޹�, ���Թ��˵�, δ�������Ҫ������ʾ�ٴ���
     *      2. ����߶�С��1, �����������ָ�ҳü��, ����:
     *          76.55 55.2 442.2 0.72 re
     *        ���ָ߶�ʱ����������������.
     *      3. ��ȡ���еı����Ϣ��, ��Ҫ����һ�ι���, �Ѱ�������cell���˵�, Ŀǰ����ֻ��һ�������������.
     *        ����ҳ��߿��и����, �����������������еĺϷ����(������re����).
     *        Ŀǰ�򵥴���, ��һ��re �����������, ֱ�Ӱѵ�һ��re���˼���.
     *          0 0 595.3 841.9 re
     * 2016.12.21:
     *      re Ҳ��Ҫ����Tm ����������ת��, �����ı�λ������λ�þ��޷��ж���ȷ��.  ����Ҫת��, ÿ�ζ��Ǿ�������, �ƺ���q/Q�й�. �����֮ǰ������ת����Ϣ???
     *      cellIndexMap[cellid] = {"maxlen":,"col":, "row":, "txtList":[]}
     * 2016.12.22:
     *      ��һ�ּ�����������ݴ�. ��ǰcell��ȫ����һ����Чcell֮��, ������cell
     * 2017.01.15:
     *      �����ǰcell����֮ǰ��ĳ����Чcell, ���õ�ǰcell ���֮ǰ����Чcell
     */
    char * processRE( MEMORYMAP *mm_p, char *desbuf, char * srcbuf, DECODE * decode_p, PAGES *pages_p  )
    {
        CELL            *   cp;
        char            *   tmpbuf;
        int                 len;
        int                 i, j, k, n;
        char        buf[128];
        
        printf( "-------------------------------------------------------------------------------processRE(), srcbuf=%s\n", srcbuf );
        if ( !srcbuf ) {
            return NULL;
        }

        cp = (CELL *)calloc( sizeof(CELL), 1 );
        //cp = (CELL *)malloc( sizeof(CELL) );
        //memset( cp, 0, sizeof(CELL) );

        len = strlen( srcbuf );
        j = 0;
        n = 1;
        for ( i = 0; i < len; i ++ ){
            if ( srcbuf[i] == ' ' ) {
                memset( buf, 0, 128 );
                memcpy( buf, srcbuf+j, i-j );
                printf( "buf=%s, i=%d, j=%d\n", buf, i, j );
                while ( buf[i] == ' ' )
                    i ++;
                j = i;      // ��һ�����ʼλ����׼��
                
                switch ( n ) {
                    case 1:
                            cp->x = atof( buf );      // ��Ԫ���x���� 
                            n ++; 
                            break;
                    case 2:
                            cp->y = atof( buf );      // ��Ԫ���y����  
                            n ++;
                            break;
                    case 3:
                            cp->w = atof( buf );      // ��Ԫ���w���� 
                            n ++;
                            break;
                    case 4: 
                            cp->h = atof( buf );      // ��Ԫ���h����  
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        //# x ����С��0�����ǿ����õ�, h,w��ֵС��1�������������ָ��ߵ�, (x,y)=(0,0)��ԭ����ʱ�ı��Ҳ�ǿ�����(ҳü����������ԭ��)
        //# ��ҳüҲ����, �����ж�ҳü�ı�
        printf( "-------------------2----------processRE(), srcbuf=%s, x=%.2f, y=%.2f, w=%.2f, h=%.2f, cp=%p\n", srcbuf, cp->x, cp->y, cp->w, cp->h, cp );
        if ( cp->x < 0 || cp->y < 0 || cp->w < 1 || ( cp->x ==0 && cp->y == 0 ) )  //# (0,0)��Ȼ����, ��ʱû�뵽��ô����
            return NULL;

        //# 2017.01.15:  ���˻������Ч��cell
        printf( "----------------------------------------3--------------------------processRE(), srcbuf=%s, cp=%p\n", srcbuf, cp );

        fillCellMap( decode_p, cp );
        
        printCellMap( decode_p->cp );
        
        return NULL;
    }

    /*
     * 23.1. filterCell()
     *       ������Ч��cell
     *  ˵��:
     *      ��Щcell ����Ч��, ���������һ����Чcell֮�ڡ� ��������
     *  2017.01.15:
     *      1. ��ǰcell �Ѿ���֮ǰ����Чcell �ڲ�, ����
     *      2. �����ǰcell ����֮ǰ�� ��Чcell�ڲ�, ���õ�ǰ��cell ���֮ǰ����Чcell   
     */

    int intsertCell( CELL * cellMap_p, CELL cur_cell ) 
    {
        return 0;
    }

    int insteadCell( CELL * cellMap_p, CELL cur_cell, int cellID)
    {
        CELL    *   cp;
        cp = cellMap_p;
        while( cp ) {
            if ( cp->id == cellID ) {
                
                return 0;
            }
        }
        return -1;
    }

    // ����ָ��ID�ĵ�Ԫ��cell
    CELL * findCell( CELL * cellMap_p, int cellID )
    {
        CELL  * cp = cellMap_p;  // ��ͷ��ʼ����

        while ( cp ) {
            if ( cp->id == cellID )     // �ҵ���
                return cp;
            
            cp = cp->next;
        }

        return NULL;    // �����껹û���ҵ��ͷ���NULL
    }


    // ����cell��Ϣʱ�������Ƿ���ҳü, ��֤����������
    // ����cell��ʱ��, �жϳ���ҳü(h<1)ҳ���ٴ������˼���
    // ���ֻ����һ��cell:  ������1��, ����һ��cell
    int fillCellMap( DECODE *decode_p, CELL *cp )
    {
        printf("-------------------fileterCell()--cellMap_p=%p----cp=%p---\n", decode_p->cp, cp );
        printCellMap( decode_p->cp );

        assert( decode_p );
        printf("after assert(decode_p)------------------------------------------------------------------------------aaaaaaaaaaaaaaaaa\n");
        float       cx, cy, ch, cw; 
        float       px, py, ph, pw;     // �����е�cell��x, y, w, h
        CELL    *   lcp;                // ��������һ��cell


        if ( !cp )              // Ҫ��ӵ�cellΪ��, �򲻲���
            return 0;

        cx = cp->x;
        cy = cp->y;
        cw = cp->w;
        ch = cp->h;


        if ( !decode_p->cp ) {       // ���cellMap_pΪ��, �����ǵ�һ��cell, ֱ�����
            printf("cellMap_p Ϊ��,ֱ�����cp-----------------------------------------111  , \n");
            cp->id = 1;
            cp->prev = NULL;
            cp->next = NULL;
            decode_p->cp = cp;
            decode_p->lcp = cp;
            return 0;
        }
        // ֻ�����һ��cell�Ƚ�, ��Ϊÿһ���¼����cell���Լ���ǰһ���Ƚ�(���һ��), �൱��ȫ�Ƚ���
        // ��Ϊ��ǰ����һֱ�ڱ仯, ��������һ��cell��һ��, Ҳ�϶�������֮ǰ��cellһ����

        printf("----------------2--------------cellMap_p ��Ϊ�յĴ���, cellMap_p=%p, cellMap_p->last=%p\n", decode_p->cp, decode_p->lcp );

        lcp = decode_p->lcp;       

        px = lcp->x,   py = lcp->y, pw = lcp->w, ph = lcp->h;
            
        if ( !( abs( px - cx ) < 1 && abs( py - cy ) < 1 && abs( px+pw-cx-cw) < 1 && abs( py+ph-cy-ch) < 1 ) )  {
            // �õ�Ԫ�� û��  ������cell���� ���� ����������Ԫ��, �������С��1
            cp->id      = lcp->id + 1;
            lcp->next   = cp;
            cp->prev    = lcp;
            decode_p->lcp = cp;

            return 0;
        }

        free( cp );     // ��cp �Ǹ����õ�, ����, �ͷ��ڴ�

        return 0;
    }


    char * processTf( char * buf, DECODE * decode_p )
    {
        char    * tmpbuf;

        tmpbuf = (char *)strsplit1( buf, ' ', 1 );      // /FT15 180 Tf  ��ʽ
        strcpy( decode_p->tf, tmpbuf +1 );      // +1 ��Ϊ������'/'   
        
        free( tmpbuf );                         // ��Ϊstrsplit1() �����������ڴ�
        
        return decode_p->tf;
    }

    /*
     *  18. hasText( buf )
     *          �жϵ�ǰ���Ƿ����ı���Ϣ��ʶ��  Tj / TJ/ ' / "
     *  ��ڲ���
     *      buf     ��������ַ���
     *  ���ڲ���
     *      ��
     *  ����ֵ
     *      False       û�а����ı���Ϣ��ʶ��
     *      True        �����ı���Ϣ��ʶ��
     *          
     */
    bool hasText( char *buf )
    {
        if ( strstr( buf, "Tj" ) || strstr( buf, "TJ" ) || strstr( buf, "\'" ) || strstr ( buf, "\"") ) 
            return true;
        else
            return false;
    }

    /*
     *  19. processText( buf, curxy, pre_xy, cur_cell, pre_cell )
     *          �����ı���Ϣ, �ж��Ƿ���Ҫ����, �Ƿ���Ҫ����
     *  ��ڲ���:
     *      
     *  ���ڲ���:
     *      
     *  ����ֵ:
     *      
     *  ˵��:
     *      1. �ж�cur_cell �� pre_cell, �����Ϊ��, Y ��ͬ, ����ͬһ��, ������
     *         1.1 �ж� cur_xy �Ƿ��� cur_cell ��, 
     *      2. ֻ�д����ı���ǰ, �ŵ���ifCRLF(),Ȼ��cur_xy , cur_cell ���浽pre_xy, pre_cell,
     *         ��ʾ��һ�δ����ı���λ�ñ����ˡ� ��ifCRLF() �д���λ����صı���
     *  2016.12.10:
     *      �ı�ʵ��˼·. �½�һ��textMap �ֵ�, key:value=��ʼ����:�ı�����
     *      ��re�Ĵ�����, ����������CellMap �ֵ���.
     *      �������ı�������������, ����textMap �Լ�cellMap, �������ı��ϲ��Լ����еĴ���.
     * 2016.12.23:
     *      ����tf��Ϣ,ʵ����Ҳ������������.
     *      ��һ����stateStack, cur_state��ʱ��, ������Ϣ��Ϊ״̬��һ����. Ŀǰ״̬����: ����, ����.
     * depends on:
     *       self.ifCRLF(), .processTj(), .processTJ()
     *          
     */
    //# cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    //# textMap = {���:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    //# cellMap = {���:{'x':,'y':,'w':,'h':}}
    //# tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # һֱ��ŵ������µ�tm����
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p ) 
    {
        // cur_xy, textMap, cellMap, cellIndexMap, tm, tf
        CMAP    * cmap_p;

        if ( !srcbuf || !decode_p || !pages_p )
            return NULL;

        // �ҵ������Ӧ��cmap
        for ( int i = 0; i < pages_p->cmaptotal; i ++ ) {
            if ( strcmp( pages_p->cmaps_p[i].fontname, decode_p->tf ) == 0 ) {  // �ҵ������Ӧ��CMAP
                cmap_p = &( pages_p->cmaps_p[i] );
                break;
            }
        }
        printf("�ҵ�CMAP, fontname=%s, ��������:%d\n", cmap_p->fontname, cmap_p->total );
        // ���濪ʼ����
        
        if ( strstr( srcbuf, "Tj" ) )               // Tj
            processTj( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "TJ" ) )          // TJ
            processTJ( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "\'" ) )  {         // '  ������, ��ͬ��T*  Tj Ҳ�����Ƶ���һ����ʼλ������ı�
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf );
        }
        else if ( strstr( srcbuf, "\"" ) )  {         // "   ˫����,  ʹ�����ּ�����ַ����, ���ݶ����뵥����һ��
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf); 
        }
        
        printf( "����ǰ:%s\n" , srcbuf );
        printf( "�����:%s|\n", desbuf );
        //printCUR_XY( cur_xy_p );
        //
        // ���ı���Ϣ��䵽 textMap ��, ����δ����ʽ����, ͬʱ�жϸ��ı������ĸ�cell, ��¼��CELL��Ϣ��
        // 
        fillTextMap( desbuf, decode_p );
        
        return NULL;
    }   

    // ���������ı������� textMap �Լ� cellMap ��
    int fillTextMap( char * buf, DECODE * decode_p )
    {
        float       ox, oy;                   // �ı���x,y ����, Ϊ�˼򻯴����е�ָ��ṹ���Ƴ���, ���ӿɶ���
        float       cx, cy, cw,ch;          // cell��x,y,w,h Ϊ�˼򻯴����е�ָ��ṹ���Ƴ���, ���ӿɶ���
        TEXT    *   tp;                     // ����µ��ı�ӳ�䵥Ԫ�� �����ӵ��ı�ӳ�����
        CELL    *   cp;                     // ָ�� decode_p->cellMap_p->last
        int         id;
        
        if ( !buf || !decode_p )
            return -1;
    
        tp = ( TEXT * )calloc( sizeof(TEXT), 1 );
        //tp = ( TEXT * )malloc( sizeof(TEXT) );
        //memset( tp, 0, sizeof(TEXT) );

        if ( !decode_p->tp ) {         // ��� �ı�ӳ��Ϊ��, ��ʾ���ǽ��������ĵ�һ���ı���
            tp->id      = 1;
            tp->prev    = NULL;                // �׸��ڵ��prev (ǰһ���ڵ�) Ϊ��NULL
            tp->next    = NULL;
            decode_p->tp = tp;
            decode_p->ltp = tp;
        } else {
            decode_p->ltp->next = tp;   // ���һ���ڵ��next ������tp
            tp->prev = decode_p->ltp;   // �½�text��ǰһ��text����֮ǰ��last text
            tp->id = tp->prev->id + 1;  // �½ڵ��id = ����һ���ڵ��id+1
            decode_p->ltp = tp;         // ����last ָ�뵽�½��� TEXT �ڵ�
        }

        ox = tp->ox = decode_p->cur_xy.ox;            // �ı� ��������x
        oy = tp->oy = decode_p->cur_xy.oy;            // �ı���������y

        tp->len = strlen( buf );
        tp->buf = (char *)calloc( sizeof(char), tp->len + 1);
        memcpy( tp->buf, buf, tp->len );
        printf("fillTextMap(), tp->buf=%s, tp->id=%d\n", tp->buf, tp->id );

        // �������Ƚ��ı��뵥Ԫ������λ��, ����ı��ڵ�Ԫ����, ���¼�����Ϣ
        // ֻ�ж����һ����Ԫ��, ��Ϊ��������뵱ǰ���ı�������ӽ�, ������������, ֮ǰ�Ĳ����ܰ���
        if ( !decode_p->cp ) {     // cell ӳ���Ϊ��, ��ʾ���ı�����cell��, ֱ�ӷ���
            return 0;
        }
        cp = decode_p->lcp;     // �ı������һ��cell���бȽ��ж� �Ƿ��� cell����

        if ( !cp )
            return 0;

        cx = cp->x,  cy = cp->y, cw = cp->w, ch = cp->h;    // cell ��������Ϣ
        printf("1===========================cp=%p, cx=%.2f, cy=%.2f, cw=%.2f, ch=%.2f, txtTotal=%d\n", cp, cx, cy, cw, ch, cp->txtTotal );
        
        if ( ( ox >= cx ) && ( ox <= cx+cw ) && ( oy > cy ) && ( oy <= cy+ch ) ) {   // ���ı���cell����
            if ( tp->len > cp->maxlen )     // ����cell����󳤶�
                cp->maxlen = tp->len; 
            
            tp->cellID = cp->id;
            cp->txtTotal ++;                // ���ֻ�Ǽ��� cell �е��ı�����, ������������ı���, 
                                            // ���ݸ�����������cp->txtIDs_p�ռ�, �ٴ���һ��textMap_p
                                            // ����cell�е��ı���id ��䵽cp->txtIDs_p ������
        }
        
        return 0;
    }

    /*
     * 20. processTj( desbuf, srcbuf, cmap )
     *      ������ Tj ��ʶ �����ֲ���
     * ��ڲ���:
     *      buf     ������İ���Tj ��δ�����ı�
     *      cmap    ���ڱ�ҳ�����ı�����ʹ�õ�cmap ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      �������ı�
     * ˵��:
     *      ���ֻ�����ı����ݣ� ���ؽ������ı�
     * ����:
     <547F>Tj 482.391 0 TD<097E>Tj 482.391 0 TD<0D2D>Tj 480 0 TD<41F1>Tj 482.391 0 TD
<4AD2>Tj 482.391 0 TD<4AF1>Tj 482.391 0 TD<3E7C>Tj 480 0 TD<044A>Tj 482.391 0 TD
<1937>Tj 482.391 0 TD<1BA0>Tj 482.391 0 TD<0836>Tj 482.391 0 TD<34D7>Tj 480 0 TD
<41F1>Tj 482.391 0 TD<0868>Tj 482.391 0 TD<18C5>Tj 482.391 0 TD<4374>Tj 480 0 TD
<0E2A>Tj
     */
    // srcbuf = "547F"  ��������  len  = 4 ��4�ı���
    //
    int decodeR( char *desbuf, char * srcbuf, int len, CMAP * cmap_p )
    {
        wchar_t     wstr[len/4+1];      // srcbuf �� "547f"��ʽ, 4�ֽڶ�Ӧ��1�� unicode��2�ֽ� 0x547F�� 
        char        buf[4+1];
        int         min, max, mid;
        int         count;              // ��¼unicode ת���ַ������ֽ���

        if ( !srcbuf || !cmap_p || len%4 != 0 )   // len����4 ��Ϊ0��ʾ��ʽ������. 
            return -1;

        min = 0;
        max = cmap_p->total-1;      // �±��Ǵ�0��ʼ��

        int     wlen = len/4;      // 
        memset( wstr, 0, sizeof(wstr) );

        for ( int i = 0; i < wlen; i ++ ) {
            memset( buf, 0, 4+1);
            memcpy( buf, &srcbuf[i * 4], 4 );
            printf("buf=%s\n", buf);
            
            wstr[i] = Hex2Int2( buf );     // ���wstr[i] �Ǹ�����,�ȴ�ű���, ����<4C75> <9645> �е�0x4c75
            
            // �ұ����Ӧ�Ľ���  0x0645, ���۰���ҷ�, ��Ϊ cmap �Ǵ�С���������
            while( min + 1 != max ) {
                mid = (min + max )/2;
                if ( cmap_p->code_p[mid].code == wstr[i] ) {
                    wstr[i] = cmap_p->code_p[mid].decode;
                    break;
                }
                if ( wstr[i] > cmap_p->code_p[mid].code )
                    min = mid;
                else
                    max = mid; 
            }
            
        }
        // wstr �д�ŵ��ǽ���������, Ҳ�������յ�����, ת��Ϊ���ֽ��ַ���
        count = unicode2str( desbuf, wlen * 2, wstr );
        printf( "\n---------------ת���������Ϊ:%s(len=%d)\n", desbuf, count );
        
       return count;     
    }

    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p )
    {
        char    tmpbuf[L_TMPBUF];
        int     i, j, k, n;            // ��������±꣬��ȡ<> ��() �ڵ�����, n ������¼Ŀ�껺�����±�
        int     len;

        if ( !srcbuf )
            return NULL;

        len = strlen( srcbuf );
        printf( "processTj() Begin:%s, len=%d\n", srcbuf, len );

        j = k = n = 0;
        for ( i = 0; i < len; i ++ ) {
            if ( srcbuf[i] == '(' ) {          // ()����Ҫ���� 
                j = i+1;          // j ������¼��ȡ���ݵ���ʼλ��
                while( i < len && srcbuf[i] != ')' )   // �ҵ�')'
                    i ++;
                if ( srcbuf[i] != ')' )        // �ݴ���
                    return NULL;
                            
                memcpy( &desbuf[n], &srcbuf[j], i-j ); // ����() ��������ݵ�Ŀ�껺����
                n += i-j;
            } else if ( srcbuf[i] == '<' ) {   // <>��ʾ��Ҫ����   
                j = i+1;                      // j ������¼��ȡ���ݵ���ʼλ��
                while( i < len && srcbuf[i] != '>' )   // �ҵ�'>'
                    i ++;
                if ( srcbuf[i] != '>' )        // �ݴ���
                    return NULL;
                memset( tmpbuf, 0, L_TMPBUF );            
                memcpy( tmpbuf, &srcbuf[j], i-j ); // ����<> ��������ݵ�Ŀ�껺����

                // ��ʼ����
                printf("-------------tmpbuf=%s, %d-%d=%d, n=%d\n", tmpbuf, i, j, i-j, n );
                k = decodeR( &desbuf[n], tmpbuf, i-j, cmap_p );
                if ( k > 0 )
                    n += k;
                
            }       
        }
        return desbuf;
    }

    /*
     * 21. processTJ( buf, cmap )
     *      ������ TJ ��ʶ �����ֲ���
     * ��ڲ���:
     *      buf     ������İ���Tj ��δ�����ı�
     *      cmap    ���ڱ�ҳ�����ı�����ʹ�õ�cmap ��ϣ��
     * ���ڲ���:
     *      ��
     * ����ֵ:
     *      �������ı�
     * ˵��:
     *    1. TJ ����, ��ʾ�����֣� ������������ʽ���ı�
     * ����:
     *    1.  [<41F7086E>-3<05192E37>-3<03C6>]TJ
     */
    char * processTJ( char * desbuf, char *srcbuf, CMAP * cmap_p )
    {
        return processTj( desbuf, srcbuf, cmap_p );
    }

    /*
     * 24. processQ( streambuf )
     *      ����q/Q ����Ϣ, q ����ͼ��״̬, Q �ָ�ͼ��״̬
     * ��ڲ���
     * ���ڲ���
     * ����ֵ
     * ˵��:
     *      ����q ��, ֱ�ӹ��˺������Ϣ, ֱ������Q. ��Ҫ��ͼ�εĴ���, ���ڱ�ϵͳ����, 
     *      ֻ��ע������Ϣ, ���Կ��Թ��˵���Щ��Ϣ
     *      
     *      ���У� ���ܼ򵥵Ĺ���q/Q�м������, q/Q �м���ܻ�Ƕ��q/Q, ��������Ҳ��������������
     *      ��list ��ʵ�ֶ�ջ����ջ�����Ǹ� pre_cell, pre_xy
     *      ������ϸ����ҳ����Ϣ, ����q/Q �������ô�����, ��Ϊre ���Ǿ�������
     *      ��ʱ������q/Q
     */
    void processQ( MEMORYMAP * mm_p )
    {
        char    * buf;

        buf = mm_readLine( mm_p );
        
        while ( !strstr( buf, "Q") ) {
            //# ���û������Q, ʲôҲ����, ������ȡ��һ�С� ���˵�q/Q֮�������
            free( buf );
            buf = mm_readLine( mm_p );
        }
        free( buf );

        return;
    }

    /*
     * 25. hasTxtPosition()
     *      �ж�Ŀ���ַ����Ƿ����λ����Ϣ
     * ��ڲ���
     *      buf         ��������ַ���
     * ���ڲ���
     *      ��
     * ����ֵ
     *      false       ������λ����Ϣ
     *      true        ����λ����Ϣ
     * ע��:
     *      ��Щ����Ҳ���ܰ��� Td, TD �Ȳ������� Ӧ������ ���ı���������Ĳ����д��� !hasText( buf )
     * 2016.12.10:
     *      ����, ����Ĵ����߼�������..... ��û�б���������, buf �в����ܺ���Td��, ���Ǳ�����Ϣ,�޸��߼�, ɾ��!hasText() ����ж�
     *      ȱ����T* �� Tm���߼��ж� - -��
     */
    bool hasTxtPosition( char * buf )
    {    
        if ( strstr( buf, "Td" ) || strstr( buf, "TD" ) || strstr( buf, "\'" ) || strstr( buf, "\"" ) || strstr( buf, "Tm" ) || strstr( buf, "T*" ) ) {
            return true;
        }
        else {
            return false;
        }
    }

    /*
     * 26. precessTxtPosition( buf, cur_xy )
     *      �������λ����Ϣ���ַ���, ��λ����Ϣ��¼��cur_xy ��ϣ����, "x":x����ֵ�� "y":y����ֵ
     * ��ڲ���
     *      buf         ����λ����Ϣ���ַ���
     * ���ڲ���
     *      cur_xy      ��ϣ�� "x":x����ֵ�� "y":y����ֵ
     * ����ֵ
     *      ��
     * ˵��:
     */
    void processTxtPosition( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    { 
        if ( strstr( buf, "Td" ) ) {
            processTd( buf, cur_xy_p, tm_p );
            return;
        }
        else if ( strstr( buf, "TD" ) ) {
            processTD( buf, cur_xy_p, tm_p );
            return;
        } else if ( strstr( buf, "Tm" ) ) {
            processTm( buf, cur_xy_p, tm_p );
            return;
        } else if ( strstr( buf, "cm" ) ) {
            processCm( buf, (CM *)tm_p );       // ��ʱ������
            return;
        } else if ( strstr( buf, "T*" ) ) {
            processTstar( buf, cur_xy_p );
            return;
        }
        
        printf("-------------------------s-------------\n");
    }

    /*
     * 27. processTd()
     *      ���� Td �ڵ�����
     * ��ڲ���:
     *      buf                 ��ǰ����Td ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      Td �ж��ֿ�����, 0 Tc 0 Tr 265 0 Td ���� -135.68 -14.78 Td
     *      1. -135.68 -14.78 Td �� 51134 L115, ���Ӧ����������ꡣ����
     *      2. 
     *      ���, Td ��������ݵ�����������ꣿ���Ǿ������ꣿ����
     *      3. 0 Tc 0 Tr 48.26 0 Td            50617  L41
     *      4. 0 Tr 144.54 0 Td                 50617 L44
     *      5. /TT0 9 Tf                        50617 L29
     *          529.68 51.72 Td
     *          (74)Tj
     *          1 0 0 1 538.68 51.72 Tm
     *          ( )Tj
     *      6. ���޸�cur_cell, pre_cell ������.
     * 2016.12.12:
     *      ����ʵ�ַ�ʽ, Td/TD�е����������Tm �еľ������ת��, ����:
     *        0.05 0 0 -0.05 80.52 146.45 Tm

     *        <0E2A>Tj 208.797 0 TD<4801>Tj 211.188 0 TD<31B0>Tj 208.797 0 TD<3020>Tj
     *      ��ô: ת�������Ӧ��[a b c d e f]=[0.05 0 0 -0.05 80.52 146.45]
     *      ����ת����ʽ: (�μ�: PDF 1.7 Reference P208)
     *          x' = a*x + c*y + e
     *          y' = b*x + d*y + f
     *      ��208.797 0 TD ʵ�ʵ�����Ϊ:
     *        x' = 0.05 * 208.797 + 0*0 + 80.52 = 184.9165 Լ184.917
     *        y' = 0 * 208.797 + (-0.05)*0 + 146.45 = 146.45
     *      �����ж�����ǰ��ı��
     *         75.15 134.4 130 16.1 re
     *       75.15 < x' < 75.15+130=205.15
     *       134.4 < y' < 134.4+16.1 = 150.5
     * 2016.12.26:
     *      Td �Ĵ���Ƚϸ���.
     *      1. ���TmΪ��,  ���֮ǰû������, ��(ox,oy) = (x,y); ���֮ǰ������, ��(ox,oy) = (x,y)+(x',y')
     *      2. ���Tm ��Ϊ��, (ox, oy) = (x,y) * tm, Ȼ�� tm �ÿ�
     *
     * depends on:
     *      self.getOriginXY()
     *   
     * 
     */
    // getXYofTd() �����Ǳ��غ���, �򻯴����,����processTd() �е���
    // 0 Tc 0 Tr 48.26 0 Td
    // ��ȡTd ֮ǰ��xy ���� 
    int getXYofTd( char * buf, CUR_XY * cur_xy_p )
    {
        int     ret;
        char    buf1[12], buf2[12], buf3[12];

        int     i = 0,j = 0;
        int     len;
       
        if ( !buf )
            return -1; 
        
        len = strlen( buf );

        memset( buf1, 0, 12 );
        memset( buf2, 0, 12 );
        memset( buf3, 0, 12 );
        
        while ( i <= len ) {  // ֱ��β��
            if ( buf[i] == ' ' || buf[i] == 0 ) {  // �����ո���β
                memset( buf1, 0, 12 );
                strcpy( buf1, buf2 );       // ��һ��ѭ�������, ��ʵ��û�����ݵ�,�ǿմ�����, ��������������
                
                memset( buf1, 0, 12 );
                strcpy( buf2, buf3 );

                memset( buf3, 0, 12 );
                memcpy( buf3, &buf[j], i-j );
                if ( strstr(buf3, "Td" ) )
                    break;
                j = i+1;  // �ո����һ���ַ�
            }
            i ++;
        }

        cur_xy_p->x = atof( buf1 );
        cur_xy_p->y = atof( buf2 );

        return 0;
    }

    void processTd( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        getXYofTd( buf, cur_xy_p );     // ��ȡ Td ֮ǰ��xy����
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "Td" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // ȫΪ0��ʾû��Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]��ֵ
            memset( tm_p, 0, sizeof(TM) );      // ����tm_p������Ϊ0
        }
    }

    /*
     * 28. processTD()
     *      ���� TD �ڵ�����, ��ȡ(x,y) ������Ϣ
     * ��ڲ���:
     *      buf                 ��ǰ����TD ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *
     *  2016.12.10:
     *     ԭ���Ĵ���������, :<0016>Tj 91.188 0 TD<0015>Tj
     *     <547F>Tj 180 0 TD<097E>Tj 180 0 TD<4AD2>Tj 180 0 TD<4AF1>Tj 180 0 TD<0836>Tj 180 0 TD<34D7>TjET
     *     �������ҪǶ�״��� ֱ��һ�д������(�����ҵ����һ�������� ET)
     *  2019.02.14:
     *     ����Tj(���������TD)��TD Ҫ����Ƕ�״���һ����,ȷ���������ݶ�������ȫ.
     *     ����Ҫ����TD���������TD, ֻ��Ҫ�����һ��TD����. ��Ϊ���е�TD��y ������ͬ, ֮����ʱ���TD, ����ΪX���겻ͬ
     *
     * ע��: 
     *     �������Ǳ���ڵ�����, ������Ҫ�������һ��TD����ȡ���µ�λ����Ϣ. �����ԣ���
     *
     */
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ) 
    {
        getXYofTd( buf, cur_xy_p );     // ��ȡ Td ֮ǰ��xy����
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "TD" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // ȫΪ0��ʾû��Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]��ֵ
            memset( tm_p, 0, sizeof(TM) );      // ����tm_p������Ϊ0
        }

    }    

    /*
     * 29. getOriginXY()
     *      ��ȡTd/TD �����Matrics ���꣬��ΪTd/TD �󲿷�������������. 
     * ˵��:
     *      1. �÷���ֻ��processTd()�� processTD() ����
     *      2. Tm, re �����Ǿ�������, ������ø÷���
     *      3. T* Ҳ���õ��ø÷���, ��Ϊ��ֱ�ӻ�����, ���ý����ж��ˡ�
     * 
     *      
     */
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p )
    {
        printf("getOriginXY():-------------======================================\n");
        //print(cur_xy)
        //print(tm)

        // ��������ȡox, oy, Ҳ���ǲο������ԭʼ����ֵ(������), ���x,y ������Ǿ�������,��ox,oy = 0,0
        if ( !tm_p ) {      //# ���û��tm, Ҳ����û��ת������, ��x,y ���Ǿ�������
            if ( cur_xy_p->ox == 0 ){   //   # ����������ǵ�һ������
                cur_xy_p->ox = cur_xy_p->x;
                cur_xy_p->oy = cur_xy_p->y;
            } else {
                cur_xy_p->ox += cur_xy_p->x;
                cur_xy_p->oy += cur_xy_p->y;
            }
        } else {    // �����tm, Ҫ��������ת��
            //# x' = a*x + c*y+e,  y'=b*x+d*y+f
            cur_xy_p->ox = tm_p->a * cur_xy_p->x + tm_p->c * cur_xy_p->y + tm_p->e;
            cur_xy_p->oy = tm_p->b * cur_xy_p->x + tm_p->d * cur_xy_p->y + tm_p->f;
        }
    }            

    /*
     * 30. processTm()
     *      ���� Tm �ڵ����� 0.05 0 0 -0.05 233.759 92.57 Tm
     * ��ڲ���:
     *      buf                 ��ǰ���� Tm λ����Ϣ ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      �ر���Ҫע�����, Tm �Ǿ�������, ����Tmʱ, pre_cell, cur_cell �Ϳ�����Ϊ[;], preT_xy Ҳ�ÿ�[:]
     *      Ҳ����˵������Ҫ�� Tm ������Ϊ��׼���м������ж�
     * 2016.12.21:
     *      ������cur_xy����. ��Ϊһ������Tm, ��ô֮ǰ�����궼û����. ��Ҫ�����µ�ת������������
     *      ����Tm��, ��ʼ�� (x,y)= (0,0)*Tm= (e,f)
     *      �ƺ����е�Tm ����BT/ET �м�. ���� ET֮��, ��Tm ��ʧЧ��.
     */
    void processTm( char *srcbuf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        char    buf[12];
        int     i, j, k, n;
        int     len;

        printf("==========================-----------------processTm(),srcbuf=%s\n",srcbuf);

        len = strlen( srcbuf );
        j = 0;
        n = 1;
        for ( i = 0; i < len; i ++ ){
            if ( srcbuf[i] == ' ' ) {
                memset( buf, 0, 12 );
                memcpy( buf, srcbuf+j, i-j );
                printf( "--------processTm()------------buf=%s, i=%d, j=%d\n", buf, i, j );
                while ( srcbuf[i] == ' ' )
                    i ++;
                j = i;      // ��һ�����ʼλ����׼��
                

                switch ( n ) {
                    case 1:
                            tm_p->a = atof( buf );      //��1���� tm_p->a ��ֵ 
                            n ++; 
                            break;
                    case 2:
                            tm_p->b = atof( buf );      //��2���� tm_p->b ��ֵ 
                            n ++;
                            break;
                    case 3:
                            tm_p->c = atof( buf );      //��3���� tm_p->c ��ֵ 
                            n ++; 
                            break;
                    case 4: 
                            tm_p->d = atof( buf );      //��4���� tm_p->d ��ֵ 
                            n ++;
                            break;
                    case 5: 
                            tm_p->e = atof( buf );      //��5���� tm_p->e ��ֵ 
                            n ++;
                            break;
                    case 6: 
                            tm_p->f = atof( buf );      //��6���� tm_p->f ��ֵ 
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        
        cur_xy_p->x = cur_xy_p->ox = tm_p->e;       // ��������ֵx
        cur_xy_p->y = cur_xy_p->oy = tm_p->f;       // ��������ֵx

        memset( cur_xy_p->opr, 0, L_OPR );
        strcpy( cur_xy_p->opr, "Tm" );

        //printTM( tm_p );
        //printCUR_XY( cur_xy_p );

        printf("--------processTM() end !srcbuf=%s\n", srcbuf );
        return;
    }

    /*
    # 30.1 processCm()
    #     ת������, Ŀǰ�����ú�Tm����, ��ͷ�����о�
    */
    void processCm( char * buf, CM * cm_p )
    {        
        printf("=================-�Թ�------------------------processTm(),%s\n", buf );
        // ��ʱ�Թ�
        return;
    }
    
    /*
     * 31. processTstar()
     *      ���� T* �ڵ�����
     * ��ڲ���:
     *      streambuf           �ֽ���ӳ���ļ� ��ϣ��, ��ȡ���ݵ�ʱ����Ҫ.
     *      buf                 ��ǰ����T* ��һ����Ϣ
     *      cmap                �ַ�ӳ��� ��ϣ��
     * ���ڲ���:
     *      cur_xy              ��ϣ��, ��¼���� cur_xy["x"], cur_xy["y"]
     * ����ֵ:
     *      ��
     * ˵��:
     *      T*   ֱ�ӵ���һ�е���ʼλ��
     *2016.12.13:
     *    ���������Ҫ���ƣ���������
     *    ����ҵ�T*, ��ôǰ��i-2, i-1����x,y 
     */
    void processTstar( char * buf, CUR_XY * cur_xy_p )
    {
        printf("processTstar() buf=%s\n", buf);
        cur_xy_p->x = 0;         
        cur_xy_p->y += 1;           //# ��+1 ��ʾ���ϴδ����λ�����, ������, ��ô������һ��
        strcpy( cur_xy_p->opr, "T*" );
        /*
        # ע�⣬ ����Ĳ����Ǽ���T* �����ǵ�һ��λ�ò������� Ŀǰ����Ҳ
        # ȷʵû�е�һ�������, �����������T*�ǵ�һ��λ�ò�����, �ͻ���쳣
        # ��Ϊ���ʱ��pre_xy["x"]�ȷ�������Ч�ġ�
        */
        return;
    }
    

