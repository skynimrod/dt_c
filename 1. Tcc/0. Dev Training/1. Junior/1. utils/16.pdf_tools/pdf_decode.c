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
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include "file_tools.h"
    #include "filemap.h"
    #include "memorymap.h"
    #include "str_tools.h"
    #include "zlib.h"
    #include "pdf_tools.h"

    // 下面函数是内容解析部分
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
    //#  以下部分为解析 解压后 页面内容部分
    /*
     * 15. decode()
     *      对给定的字节流进行解码
     * 思路:
     *      1. 将每一部分的处理都模块化
     *      2. 最高一级(I级)是 processBDC, processBT， 所有的处理都在这BDC/EMC, BT/ET 之间， 
     *         这2中之间也会互相嵌套
     *      3. II级 处理是: 
     *          processTD, processTd, processTm, processT*        文本位置
     *          processTJ, processTj, processQuote, processDblQuote         文本显示, 后两个是单引号, 双引号的处理
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  文本状态
     *          表格处理？？？
     *      2016.12.08:
     *          采用虚拟纸张的技术来进行内容编排, 即 处理后的数据存放在一个虚拟纸张的内存中，然后输出这个数据即可.
     *          virtual_Page={["Y坐标值": 文字内容],}
     *          然后输出的时候只要对virtual_Page 按照关键字排序后进行输出即可.
     *          坐标处理: 当前坐标如果与已有坐标中的某个坐标相差小于1, 则可视为同一行 。 因为最小的字体也会大于1个像素
     *          {Y:["文字内容","长度", 单位长度]},
     *               单位长度是文字的字体的长度, 用来判断下一文字是否需要与该文字之间有分隔符
     *               如果下一段文字的起始x坐标与该文字的x+len 小于单位长度, 则不需要分隔符.
     *      2016.12.21:
     *          之前的处理, 还是有很多不完善的地方, 主要是没有按照pdf 的环境的设计思路. 下面完善设计思路, 增加 图形状态stack 实现.
     *            用GraphStateStack 记录当前压栈的所有 图形状态, cur_graph_state 用来记录当前的图形状态. 考虑到只处理文本,
     *            图形状态只记录 tm. 
     * depends on:
     *        file_tools.initStreamBuf(), .readLineST( streambuf)
     *        self.processBT(), .processBDC()
     */

    /*
     * # 坐标信息, 表格信息(初始化页眉是没有的, -1用于页眉编号), 文本信息, 转换坐标
    // 坐标信息
    typedef struct __cur_xy__ {
        int     x;
        int     y;
    }CUR_XY;

    typedef struct __cellMap__ {
        // 单元格信息
    }CELLMAP;

    typedef struct __cellIndexMap__{
        // 单元格索引信息
    }CELLINDEXMAP;

    typedef struct __textMap__ {
        // 文本映射表
    } TEXTMAP;

    typedef struct __tm__ {
        // tm 信息
    }TM:

    typedef struct __tf__ {
        // tf 信息
    }TF;
    */
    void freeDecode( DECODE * decode_p )
    {
        CELL    * cp;
        TEXT    * tp;
        
        if ( !decode_p )
            return;

        // 1. 释放 cellMap 中申请的内存
        cp = decode_p->cp;
        
        while ( cp ) {
            if ( cp->txtTotal > 0 ) 
                free( cp->tps );
            cp = cp->next;           
        } 

        // 2. 释放 textMap中申请的内存
        tp = decode_p->tp;
        
        while ( tp ) {
            if ( tp->buf ) 
                free( tp->buf );
            tp = tp->next;           
        } 

        //
        // 3. 释放decode_p
        free( decode_p );

    }

    /*
     * 15. decode()
     *      对给定的字节流进行解码
     * 思路:
     *      1. 将每一部分的处理都模块化
     *      2. 最高一级(I级)是 processBDC, processBT， 所有的处理都在这BDC/EMC, BT/ET 之间， 
     *         这2中之间也会互相嵌套
     *      3. II级 处理是: 
     *          processTD, processTd, processTm, processT*        文本位置
     *          processTJ, processTj, processQuote, processDblQuote         文本显示, 后两个是单引号, 双引号的处理
     *          processTc, processTw, processTz, processTL, processTf,processTr, processTs  文本状态
     *          表格处理？？？
     *      2016.12.08:
     *          采用虚拟纸张的技术来进行内容编排, 即 处理后的数据存放在一个虚拟纸张的内存中，然后输出这个数据即可.
     *          virtual_Page={["Y坐标值": 文字内容],}
     *          然后输出的时候只要对virtual_Page 按照关键字排序后进行输出即可.
     *          坐标处理: 当前坐标如果与已有坐标中的某个坐标相差小于1, 则可视为同一行 。 因为最小的字体也会大于1个像素
     *          {Y:["文字内容","长度", 单位长度]},
     *               单位长度是文字的字体的长度, 用来判断下一文字是否需要与该文字之间有分隔符
     *               如果下一段文字的起始x坐标与该文字的x+len 小于单位长度, 则不需要分隔符.
     *      2016.12.21:
     *          之前的处理, 还是有很多不完善的地方, 主要是没有按照pdf 的环境的设计思路. 下面完善设计思路, 增加 图形状态stack 实现.
     *            用GraphStateStack 记录当前压栈的所有 图形状态, cur_graph_state 用来记录当前的图形状态. 考虑到只处理文本,
     *            图形状态只记录 tm. 
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

        retbuf = (char *)calloc( dlen, 1 ); // 确保申请的内存足够用
        
        decode_p = (DECODE *)calloc( sizeof( DECODE ), 1 );
        
        mm_p = initMemoryMap( buf, dlen );

        while( 1 ) {    // 注意: 下面的判断不是if else, 因为有可能一行包括多种操作符
            tmpbuf = mm_readLine( mm_p );
            if ( !tmpbuf )
                break;
            printf( "decode() : tmpbuf = %s\n", tmpbuf );
            
            if ( strstr( tmpbuf, "BT" ) ) {         // 如果是 包含"BT"/"ET"
                processBT( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            if ( strstr( tmpbuf, "BDC" ) ) { // 如果包含"BDC"/"EMC"
                processBDC( mm_p, &retbuf[strlen(retbuf)], tmpbuf, decode_p, pages_p );
            } 
            
            // re 的处理应该在BT/ET, BDC/EDC 之外, 也就是单元格.
            if ( strstr( tmpbuf, "re" )  ) {   //包含"re"
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
        
        // 下面来处理上面解析出来的内容, 包括表格的重新构建
        //
        //
        printf("----------------Hello decode()--------------------------\n");
        printCellMap( decode_p->cp );
        printTextMap( decode_p->tp );
        
        memset( retbuf, 0, dlen );
        retbuf = (char *)procText4Table( retbuf, dlen, decode_p->tp, decode_p->cp );

        printCellMap( decode_p->cp );
        printTextMap( decode_p->tp );

        printf("============解析后的该页内容为:\n%s\n", retbuf);

        freeDecode( decode_p );
        
        return retbuf;
    }


    /*
     * 16. processBT()
     *      处理 BT/ET 内的内容
     * 入口参数:
     *      mm-p           字节流映射文件 哈希表, 读取内容的时候需要.
     *      pages_p        pages_p里面包含字符映射表 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      无
     * 说明:
     *      处理过程基本同BDC 一致， 就是内部不包含BT的判断, 而是包含了BDC处理的嵌套
     *      处理后的内容存放在哪儿？？？？？ 文件？？
     * 2016.12.12:
     *      Tm 的内容是转换坐标系. [a b c d e f], 定义一个list Tm, 存放最新的转换坐标
     *      Td/TD 的坐标实际为
     *           x' = a * x + c * y + e
     *           y' = b * x + d * y + f
     * depends on:
     *       processBDC(), processRE(), //processQ()
     *       self.hasTxtPosition(), .processTxtPosition(),  .hasText(), .processText(),
     *       self.file_tools.readLineST()
     *      
    # cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    # textMap = {编号:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    # cellMap = {编号:{'x':,'y':,'w':,'h':}}
    # tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # 一直存放的是最新的tm数据
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
            
            //# 1. 过滤 页眉，页脚， 水印
            //#    判断是否包含  /Artifact && （/Top   /Bottom   /watermark）任一 
            //#     如果包含， 则过滤后面的BDC, EMC 之间的内容
            //# 2. 即便是页眉或页脚, 里面的坐标也要处理, 否则后面的坐标会出现错误.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # 页眉  , BDC 是起始标识
                while( tmpbuf ) {
                    printf("页眉部分. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# 页眉结束的标识
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
                // 页脚的处理
                while ( 1 ) {       // # 下面的循环判断比上面的循环判断可读性要高一点
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# 页脚结束的标识
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BDC" ) ) {       // # 一段文字  处理 BDC/EMC  文字部分
                processBDC( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # 如果 有 re 信息， 表示是表格, 获取cell 信息, 如果碰到Tm, 会置cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # 如有有q 信息, 那么表示保存当前的图形状态信息, 包括坐标信息, 然后到Q 再恢复
            /* 暂时不处理q/Q, 因为re 使用的是绝对坐标
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */

            //# 处理字体信息,这个字体就是当前解码需要的type0字体
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # 如果buf 有 文本位置信息, 则处理后获得当前的xy 坐标
                # 2016.12.13:
                #   不单独处理位置, 这儿只处理Tm 也就是转换坐标信息, 保证最新的转换坐标起作用. 文本的坐标在文本内部处理
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm 处理与tm 处理相同, 调用Tm的处理方法既可
            //#    self.processCm( tmpbuf, tm )
            
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );

            //# 如果buf 有文本信息 (这个时候应该没有BDC/BT, 否则下面的desbuf 应该替换为&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "ET" ) ) {  // # 正文结束的标识
                printf("碰到ET, BT/ET 处理结束. tmpbuf=%s\n", tmpbuf );
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }
    
    /*
     * 17. processBDC()
     *      处理 BDC/EMC 内的内容
     * 入口参数:
     *      streambuf           字节流映射文件 哈希表, 读取内容的时候需要.
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      无
     * 说明:
     *      1. 过滤页眉/页脚, 如果有的话
     *      2. 如果有Td/TD/Tm/T*, 就获取位置信息, 并与之前处理的位置信息进行比较, 来判断是否要换行
     *      3. 如果有Tj,TJ, ', " , 表示文本内容, 摘取文本内容, 如果需要解码则进行解码
     *      4. 如果有re, 一般会对应f*, 也就是画矩形操作  
     *          x, y, width, height    re   // x,y 是左下角的坐标
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
            
            //# 1. 过滤 页眉，页脚， 水印
            //#    判断是否包含  /Artifact && （/Top   /Bottom   /watermark）任一 
            //#     如果包含， 则过滤后面的BDC, EMC 之间的内容
            //# 2. 即便是页眉或页脚, 里面的坐标也要处理, 否则后面的坐标会出现错误.
            if ( strstr( tmpbuf, "/Top" ) && strstr( tmpbuf, "/Pagination" ) ) {
                // # 页眉  , BDC 是起始标识
                while( tmpbuf ) {
                    printf("页眉部分. tmpbuf=%s\n", tmpbuf );
                    if ( strstr( tmpbuf, "EMC" ) )  //# 页眉结束的标识
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
                // 页脚的处理
                while ( 1 ) {       // # 下面的循环判断比上面的循环判断可读性要高一点
                    free( tmpbuf );
                    tmpbuf = mm_readLine( mm_p );
                    if ( !tmpbuf )
                        break;
                    if ( strstr( tmpbuf, "EMC" ) )       //# 页脚结束的标识
                        break;
                    if ( hasTxtPosition( tmpbuf ) )
                        processTxtPosition( tmpbuf, cur_xy_p, tm_p );
                    if ( strstr( tmpbuf, "Tm" ) )
                        processTm( tmpbuf, cur_xy_p, tm_p );
                }
            }

            else if ( strstr( tmpbuf, "BT" ) ) {       // # 一段文字  处理 BT/ET  文字部分
                processBT( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            else if ( strstr( tmpbuf, "re" ) ) {    // # 58.08 323.96 124.82 19.02 re
                // # 如果 有 re 信息， 表示是表格, 获取cell 信息, 如果碰到Tm, 会置cur_cell=[:]
                processRE( mm_p, desbuf, tmpbuf, decode_p, pages_p );
            }

            // # 如有有q 信息, 那么表示保存当前的图形状态信息, 包括坐标信息, 然后到Q 再恢复
            /* 暂时不处理q/Q, 因为re 使用的是绝对坐标
                #if ( "q" in buf ) :    # q
                #    self.processQ( streambuf )
                #}
            */
            if ( strstr( tmpbuf, "q" ) || strstr( tmpbuf, "Q" ) ) 
                memset( tm_p, 0, sizeof(TM) );


            //# 处理字体信息,这个字体就是当前解码需要的type0字体
            if ( strstr( tmpbuf, "Tf" ) ) {
                processTf( tmpbuf, decode_p );
                printf("----------------------------------------------tf=%s----------------------\n", decode_p->tf );
            }

            /*
                # 如果buf 有 文本位置信息, 则处理后获得当前的xy 坐标
                # 2016.12.13:
                #   不单独处理位置, 这儿只处理Tm 也就是转换坐标信息, 保证最新的转换坐标起作用. 文本的坐标在文本内部处理
            */
        
            if ( hasTxtPosition( tmpbuf ) ) {
                processTxtPosition( tmpbuf, cur_xy_p, tm_p );
            }
            
            //if ( strstr( tmpbuf, "Tm" ) )
            //    processTm( tmpbuf, cur_xy_p, tm_p ); 
            

            //#if ( "cm" in tmpbuf )  # cm 处理与tm 处理相同, 调用Tm的处理方法既可
            //#    self.processCm( tmpbuf, tm )
            
            //# 如果buf 有文本信息 (这个时候应该没有BDC/BT, 否则下面的desbuf 应该替换为&desbuf[strlen(desbuf)])
            if ( hasText( tmpbuf ) ) { 
                processText(  desbuf, tmpbuf, decode_p, pages_p );
            }

            if ( strstr(tmpbuf, "EMC" ) ) {  // # 正文结束的标识
                free( tmpbuf );
                return desbuf;
            }
            free( tmpbuf );
        }
        return desbuf;
    }

    /*
     * 23. processRE()
     *      处理 re 内的内容
     * 入口参数:
     *      buf                 当前包含TD 的一行信息
     * 出口参数:
     *      cur_cell            哈希表, 记录单元格坐标 "x":x坐标值, "y":y坐标值, "w":width, "h":height
     * 返回值:
     *      无
     * 说明:
     *      如果有re, 则说明是表格, 记录Tj/TJ/'/" 之前的最新的re 的数据为最新的cell 信息。
     *  如果连续有多个re, 那么处理最新的re信息后, cur_cell 也存放的是最新的re信息
     *      如果碰到Tm, 会置cur_cell=[:], pre_cell=[:]
     *      x, y, width, height  re   ,  画一个矩形. x,y 是左下角坐标.
     *      RE 返回的是CELL 的词典, {'re1':[x,y,w,h],'re2',}
     * 2016.12.10:
     *      改变思路, 每次处理re的时候将获取的cell信息存放在一个cellMap中, 所有内容解析出来后,
     *      再结合 textMap和cellMap来处理换行以及跨表格文字的换行问题.
     *      1. 有些pdf文档中re 的x是负数, w 高达好几千, 例如:
     *              -1638.4 72 3795.5 39.1 re
     *      可能是用来控制的, 与内容无关, 可以过滤掉, 未来如果需要进行显示再处理
     *      2. 如果高度小于1, 可能是用来分隔页眉的, 例如:
     *          76.55 55.2 442.2 0.72 re
     *        这种高度时不可能有字体放入的.
     *      3. 获取所有的表格信息后, 还要进行一次过滤, 把包含表格的cell过滤掉, 目前发现只有一种情况会包含表格.
     *        整个页面边框有个表格, 这个表格会包含其它所有的合法表格(控制类re除外).
     *        目前简单处理, 第一个re 就是这种情况, 直接把第一个re过滤即可.
     *          0 0 595.3 841.9 re
     * 2016.12.21:
     *      re 也需要根据Tm 来进行坐标转换, 否则文本位置与表格位置就无法判断正确了.  不需要转换, 每次都是绝对坐标, 似乎与q/Q有关. 会清除之前的坐标转换信息???
     *      cellIndexMap[cellid] = {"maxlen":,"col":, "row":, "txtList":[]}
     * 2016.12.22:
     *      对一种极端情况进行容错. 当前cell完全在另一个有效cell之内, 则丢弃该cell
     * 2017.01.15:
     *      如果当前cell包含之前的某个有效cell, 则用当前cell 替代之前的有效cell
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
                j = i;      // 下一项的起始位置做准备
                
                switch ( n ) {
                    case 1:
                            cp->x = atof( buf );      // 单元格的x坐标 
                            n ++; 
                            break;
                    case 2:
                            cp->y = atof( buf );      // 单元格的y坐标  
                            n ++;
                            break;
                    case 3:
                            cp->w = atof( buf );      // 单元格的w坐标 
                            n ++;
                            break;
                    case 4: 
                            cp->h = atof( buf );      // 单元格的h坐标  
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        //# x 坐标小于0可能是控制用的, h,w数值小于1可能是用来做分割线的, (x,y)=(0,0)从原点来时的表格也是控制用(页眉都不可能在原点)
        //# 把页眉也保留, 用来判断页眉文本
        printf( "-------------------2----------processRE(), srcbuf=%s, x=%.2f, y=%.2f, w=%.2f, h=%.2f, cp=%p\n", srcbuf, cp->x, cp->y, cp->w, cp->h, cp );
        if ( cp->x < 0 || cp->y < 0 || cp->w < 1 || ( cp->x ==0 && cp->y == 0 ) )  //# (0,0)仍然过滤, 暂时没想到怎么处理
            return NULL;

        //# 2017.01.15:  过滤或替代无效的cell
        printf( "----------------------------------------3--------------------------processRE(), srcbuf=%s, cp=%p\n", srcbuf, cp );

        fillCellMap( decode_p, cp );
        
        printCellMap( decode_p->cp );
        
        return NULL;
    }

    /*
     * 23.1. filterCell()
     *       过滤无效的cell
     *  说明:
     *      有些cell 是无效的, 自身就在另一个有效cell之内。 丢弃即可
     *  2017.01.15:
     *      1. 当前cell 已经在之前的有效cell 内部, 则丢弃
     *      2. 如果当前cell 包含之前的 有效cell内部, 则用当前的cell 替代之前的有效cell   
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

    // 查找指定ID的单元格cell
    CELL * findCell( CELL * cellMap_p, int cellID )
    {
        CELL  * cp = cellMap_p;  // 从头开始遍历

        while ( cp ) {
            if ( cp->id == cellID )     // 找到了
                return cp;
            
            cp = cp->next;
        }

        return NULL;    // 遍历完还没有找到就返回NULL
    }


    // 保存cell信息时不区分是否是页眉, 保证数据完整性
    // 处理cell的时候, 判断出是页眉(h<1)页脚再处理或过滤即可
    // 这儿只过滤一种cell:  误差不超过1的, 视作一个cell
    int fillCellMap( DECODE *decode_p, CELL *cp )
    {
        printf("-------------------fileterCell()--cellMap_p=%p----cp=%p---\n", decode_p->cp, cp );
        printCellMap( decode_p->cp );

        assert( decode_p );
        printf("after assert(decode_p)------------------------------------------------------------------------------aaaaaaaaaaaaaaaaa\n");
        float       cx, cy, ch, cw; 
        float       px, py, ph, pw;     // 链表中的cell的x, y, w, h
        CELL    *   lcp;                // 链表的最后一个cell


        if ( !cp )              // 要添加的cell为空, 则不操作
            return 0;

        cx = cp->x;
        cy = cp->y;
        cw = cp->w;
        ch = cp->h;


        if ( !decode_p->cp ) {       // 如果cellMap_p为空, 则这是第一个cell, 直接添加
            printf("cellMap_p 为空,直接添加cp-----------------------------------------111  , \n");
            cp->id = 1;
            cp->prev = NULL;
            cp->next = NULL;
            decode_p->cp = cp;
            decode_p->lcp = cp;
            return 0;
        }
        // 只与最后一个cell比较, 因为每一个新加入的cell都自己的前一个比较(最后一个), 相当于全比较了
        // 因为当前坐标一直在变化, 如果与最后一个cell不一样, 也肯定不会与之前的cell一样了

        printf("----------------2--------------cellMap_p 不为空的处理, cellMap_p=%p, cellMap_p->last=%p\n", decode_p->cp, decode_p->lcp );

        lcp = decode_p->lcp;       

        px = lcp->x,   py = lcp->y, pw = lcp->w, ph = lcp->h;
            
        if ( !( abs( px - cx ) < 1 && abs( py - cy ) < 1 && abs( px+pw-cx-cw) < 1 && abs( py+ph-cy-ch) < 1 ) )  {
            // 该单元格 没有  被其他cell包含 或者 包含其他单元格, 并且误差小于1
            cp->id      = lcp->id + 1;
            lcp->next   = cp;
            cp->prev    = lcp;
            decode_p->lcp = cp;

            return 0;
        }

        free( cp );     // 该cp 是个无用的, 丢弃, 释放内存

        return 0;
    }


    char * processTf( char * buf, DECODE * decode_p )
    {
        char    * tmpbuf;

        tmpbuf = (char *)strsplit1( buf, ' ', 1 );      // /FT15 180 Tf  格式
        strcpy( decode_p->tf, tmpbuf +1 );      // +1 是为了跳过'/'   
        
        free( tmpbuf );                         // 因为strsplit1() 里面申请了内存
        
        return decode_p->tf;
    }

    /*
     *  18. hasText( buf )
     *          判断当前行是否有文本信息标识符  Tj / TJ/ ' / "
     *  入口参数
     *      buf     待处理的字符串
     *  出口参数
     *      无
     *  返回值
     *      False       没有包含文本信息标识符
     *      True        包含文本信息标识符
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
     *          处理文本信息, 判断是否需要换行, 是否需要解码
     *  入口参数:
     *      
     *  出口参数:
     *      
     *  返回值:
     *      
     *  说明:
     *      1. 判断cur_cell 与 pre_cell, 如果不为空, Y 相同, 则在同一行, 否则换行
     *         1.1 判断 cur_xy 是否在 cur_cell 中, 
     *      2. 只有处理文本以前, 才调用ifCRLF(),然后将cur_xy , cur_cell 保存到pre_xy, pre_cell,
     *         表示上一次处理文本的位置保存了。 才ifCRLF() 中处理位置相关的变量
     *  2016.12.10:
     *      改变实现思路. 新建一个textMap 字典, key:value=起始坐标:文本内容
     *      在re的处理中, 将结果存放在CellMap 字典中.
     *      等所有文本都解析出来后, 处理textMap 以及cellMap, 来进行文本合并以及换行的处理.
     * 2016.12.23:
     *      增加tf信息,实际上也就是字体名称.
     *      下一步用stateStack, cur_state的时候, 字体信息作为状态的一部分. 目前状态包括: 坐标, 字体.
     * depends on:
     *       self.ifCRLF(), .processTj(), .processTJ()
     *          
     */
    //# cur_xy = {"cur_opr":,"x":x,"y":y,  "ox":ox,"oy":oy, "tm":{tm}}
    //# textMap = {编号:{"xy":{"x":x,"y":y, "ox":ox,"oy":oy,"tm":{tm}},'txt':txt} }
    //# cellMap = {编号:{'x':,'y':,'w':,'h':}}
    //# tm = {'a':a, 'b':b, 'c':c, 'd':d, 'e':e, 'f':f}    # 一直存放的是最新的tm数据
    char * processText( char *desbuf, char *srcbuf, DECODE *decode_p, PAGES *pages_p ) 
    {
        // cur_xy, textMap, cellMap, cellIndexMap, tm, tf
        CMAP    * cmap_p;

        if ( !srcbuf || !decode_p || !pages_p )
            return NULL;

        // 找到字体对应的cmap
        for ( int i = 0; i < pages_p->cmaptotal; i ++ ) {
            if ( strcmp( pages_p->cmaps_p[i].fontname, decode_p->tf ) == 0 ) {  // 找到字体对应的CMAP
                cmap_p = &( pages_p->cmaps_p[i] );
                break;
            }
        }
        printf("找到CMAP, fontname=%s, 编码数量:%d\n", cmap_p->fontname, cmap_p->total );
        // 下面开始解码
        
        if ( strstr( srcbuf, "Tj" ) )               // Tj
            processTj( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "TJ" ) )          // TJ
            processTJ( desbuf, srcbuf, cmap_p );

        else if ( strstr( srcbuf, "\'" ) )  {         // '  单引号, 等同于T*  Tj 也就是移到下一行起始位置输出文本
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf );
        }
        else if ( strstr( srcbuf, "\"" ) )  {         // "   双引号,  使用了字间隔与字符间隔, 内容而言与单引号一样
            processTj( desbuf, srcbuf, cmap_p );
            sprintf( desbuf, "\r\n%s", desbuf); 
        }
        
        printf( "解码前:%s\n" , srcbuf );
        printf( "解码后:%s|\n", desbuf );
        //printCUR_XY( cur_xy_p );
        //
        // 将文本信息填充到 textMap 中, 便于未来格式处理, 同时判断该文本属于哪个cell, 记录在CELL信息中
        // 
        fillTextMap( desbuf, decode_p );
        
        return NULL;
    }   

    // 将解码后的文本保存在 textMap 以及 cellMap 中
    int fillTextMap( char * buf, DECODE * decode_p )
    {
        float       ox, oy;                   // 文本的x,y 坐标, 为了简化代码中的指针结构名称长度, 增加可读性
        float       cx, cy, cw,ch;          // cell的x,y,w,h 为了简化代码中的指针结构名称长度, 增加可读性
        TEXT    *   tp;                     // 存放新的文本映射单元， 并链接到文本映射表中
        CELL    *   cp;                     // 指向 decode_p->cellMap_p->last
        int         id;
        
        if ( !buf || !decode_p )
            return -1;
    
        tp = ( TEXT * )calloc( sizeof(TEXT), 1 );
        //tp = ( TEXT * )malloc( sizeof(TEXT) );
        //memset( tp, 0, sizeof(TEXT) );

        if ( !decode_p->tp ) {         // 如果 文本映射为空, 表示这是解析出来的第一个文本串
            tp->id      = 1;
            tp->prev    = NULL;                // 首个节点的prev (前一个节点) 为空NULL
            tp->next    = NULL;
            decode_p->tp = tp;
            decode_p->ltp = tp;
        } else {
            decode_p->ltp->next = tp;   // 最后一个节点的next 链接上tp
            tp->prev = decode_p->ltp;   // 新建text的前一个text就是之前的last text
            tp->id = tp->prev->id + 1;  // 新节点的id = 其上一个节点的id+1
            decode_p->ltp = tp;         // 更新last 指针到新建的 TEXT 节点
        }

        ox = tp->ox = decode_p->cur_xy.ox;            // 文本 绝对坐标x
        oy = tp->oy = decode_p->cur_xy.oy;            // 文本绝对坐标y

        tp->len = strlen( buf );
        tp->buf = (char *)calloc( sizeof(char), tp->len + 1);
        memcpy( tp->buf, buf, tp->len );
        printf("fillTextMap(), tp->buf=%s, tp->id=%d\n", tp->buf, tp->id );

        // 下面来比较文本与单元格的相对位置, 如果文本在单元格中, 则记录相关信息
        // 只判断最后一个单元格, 因为这个坐标与当前的文本坐标最接近, 如果这个不包含, 之前的不可能包含
        if ( !decode_p->cp ) {     // cell 映射表为空, 表示该文本不在cell中, 直接返回
            return 0;
        }
        cp = decode_p->lcp;     // 文本与最后一个cell进行比较判断 是否在 cell里面

        if ( !cp )
            return 0;

        cx = cp->x,  cy = cp->y, cw = cp->w, ch = cp->h;    // cell 的坐标信息
        printf("1===========================cp=%p, cx=%.2f, cy=%.2f, cw=%.2f, ch=%.2f, txtTotal=%d\n", cp, cx, cy, cw, ch, cp->txtTotal );
        
        if ( ( ox >= cx ) && ( ox <= cx+cw ) && ( oy > cy ) && ( oy <= cy+ch ) ) {   // 该文本在cell里面
            if ( tp->len > cp->maxlen )     // 更新cell的最大长度
                cp->maxlen = tp->len; 
            
            tp->cellID = cp->id;
            cp->txtTotal ++;                // 这儿只是计数 cell 中的文本数量, 最后处理完所有文本后, 
                                            // 根据该数字来申请cp->txtIDs_p空间, 再处理一次textMap_p
                                            // 将该cell中的文本的id 填充到cp->txtIDs_p 数组中
        }
        
        return 0;
    }

    /*
     * 20. processTj( desbuf, srcbuf, cmap )
     *      处理含有 Tj 标识 的文字部分
     * 入口参数:
     *      buf     待处理的包含Tj 的未解码文本
     *      cmap    用于本页所有文本解码使用的cmap 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      解码后的文本
     * 说明:
     *      这儿只解析文本内容， 返回解码后的文本
     * 例子:
     <547F>Tj 482.391 0 TD<097E>Tj 482.391 0 TD<0D2D>Tj 480 0 TD<41F1>Tj 482.391 0 TD
<4AD2>Tj 482.391 0 TD<4AF1>Tj 482.391 0 TD<3E7C>Tj 480 0 TD<044A>Tj 482.391 0 TD
<1937>Tj 482.391 0 TD<1BA0>Tj 482.391 0 TD<0836>Tj 482.391 0 TD<34D7>Tj 480 0 TD
<41F1>Tj 482.391 0 TD<0868>Tj 482.391 0 TD<18C5>Tj 482.391 0 TD<4374>Tj 480 0 TD
<0E2A>Tj
     */
    // srcbuf = "547F"  或多个编码  len  = 4 或4的倍数
    //
    int decodeR( char *desbuf, char * srcbuf, int len, CMAP * cmap_p )
    {
        wchar_t     wstr[len/4+1];      // srcbuf 是 "547f"格式, 4字节对应的1个 unicode是2字节 0x547F， 
        char        buf[4+1];
        int         min, max, mid;
        int         count;              // 记录unicode 转换字符串的字节数

        if ( !srcbuf || !cmap_p || len%4 != 0 )   // len除余4 不为0表示格式有问题. 
            return -1;

        min = 0;
        max = cmap_p->total-1;      // 下标是从0开始的

        int     wlen = len/4;      // 
        memset( wstr, 0, sizeof(wstr) );

        for ( int i = 0; i < wlen; i ++ ) {
            memset( buf, 0, 4+1);
            memcpy( buf, &srcbuf[i * 4], 4 );
            printf("buf=%s\n", buf);
            
            wstr[i] = Hex2Int2( buf );     // 这儿wstr[i] 是复用了,先存放编码, 比如<4C75> <9645> 中的0x4c75
            
            // 找编码对应的解码  0x0645, 用折半查找法, 因为 cmap 是从小到大排序的
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
        // wstr 中存放的是解码后的内容, 也就是最终的内容, 转换为单字节字符串
        count = unicode2str( desbuf, wlen * 2, wstr );
        printf( "\n---------------转换后的内容为:%s(len=%d)\n", desbuf, count );
        
       return count;     
    }

    char * processTj( char *desbuf, char *srcbuf, CMAP * cmap_p )
    {
        char    tmpbuf[L_TMPBUF];
        int     i, j, k, n;            // 用来标记下标，获取<> 或() 内的内容, n 用来记录目标缓冲区下标
        int     len;

        if ( !srcbuf )
            return NULL;

        len = strlen( srcbuf );
        printf( "processTj() Begin:%s, len=%d\n", srcbuf, len );

        j = k = n = 0;
        for ( i = 0; i < len; i ++ ) {
            if ( srcbuf[i] == '(' ) {          // ()不需要解码 
                j = i+1;          // j 用来记录截取内容的起始位置
                while( i < len && srcbuf[i] != ')' )   // 找到')'
                    i ++;
                if ( srcbuf[i] != ')' )        // 容错处理
                    return NULL;
                            
                memcpy( &desbuf[n], &srcbuf[j], i-j ); // 复制() 里面的内容到目标缓冲区
                n += i-j;
            } else if ( srcbuf[i] == '<' ) {   // <>表示需要解码   
                j = i+1;                      // j 用来记录截取内容的起始位置
                while( i < len && srcbuf[i] != '>' )   // 找到'>'
                    i ++;
                if ( srcbuf[i] != '>' )        // 容错处理
                    return NULL;
                memset( tmpbuf, 0, L_TMPBUF );            
                memcpy( tmpbuf, &srcbuf[j], i-j ); // 复制<> 里面的内容到目标缓冲区

                // 开始解码
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
     *      处理含有 TJ 标识 的文字部分
     * 入口参数:
     *      buf     待处理的包含Tj 的未解码文本
     *      cmap    用于本页所有文本解码使用的cmap 哈希表
     * 出口参数:
     *      无
     * 返回值:
     *      解码后的文本
     * 说明:
     *    1. TJ 类型, 表示有文字， 里面包含多个格式的文本
     * 例子:
     *    1.  [<41F7086E>-3<05192E37>-3<03C6>]TJ
     */
    char * processTJ( char * desbuf, char *srcbuf, CMAP * cmap_p )
    {
        return processTj( desbuf, srcbuf, cmap_p );
    }

    /*
     * 24. processQ( streambuf )
     *      处理q/Q 的信息, q 保存图形状态, Q 恢复图形状态
     * 入口参数
     * 出口参数
     * 返回值
     * 说明:
     *      碰到q 后, 直接过滤后面的信息, 直到碰到Q. 主要是图形的处理, 对于本系统而言, 
     *      只关注内容信息, 所以可以过滤掉这些信息
     *      
     *      不行， 不能简单的过滤q/Q中间的内容, q/Q 中间可能会嵌套q/Q, 而且里面也可能有文字内容
     *      用list 来实现堆栈，堆栈里存放那个 pre_cell, pre_xy
     *      经过仔细分析页面信息, 发现q/Q 本质上用处不大, 因为re 都是绝对坐标
     *      暂时不处理q/Q
     */
    void processQ( MEMORYMAP * mm_p )
    {
        char    * buf;

        buf = mm_readLine( mm_p );
        
        while ( !strstr( buf, "Q") ) {
            //# 如果没有碰到Q, 什么也不做, 继续读取下一行。 过滤掉q/Q之间的内容
            free( buf );
            buf = mm_readLine( mm_p );
        }
        free( buf );

        return;
    }

    /*
     * 25. hasTxtPosition()
     *      判断目标字符串是否包含位置信息
     * 入口参数
     *      buf         待处理的字符串
     * 出口参数
     *      无
     * 返回值
     *      false       不包含位置信息
     *      true        包含位置信息
     * 注意:
     *      有些内容也可能包含 Td, TD 等操作符， 应该区别， 即文本内容里面的不进行处理 !hasText( buf )
     * 2016.12.10:
     *      我晕, 上面的处理逻辑错误了..... 在没有编码的情况下, buf 中不可能含有Td等, 而是编码信息,修改逻辑, 删除!hasText() 相关判断
     *      缺少了T* 和 Tm的逻辑判断 - -！
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
     *      处理包含位置信息的字符串, 将位置信息记录在cur_xy 哈希表中, "x":x坐标值， "y":y坐标值
     * 入口参数
     *      buf         包含位置信息的字符串
     * 出口参数
     *      cur_xy      哈希表 "x":x坐标值， "y":y坐标值
     * 返回值
     *      无
     * 说明:
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
            processCm( buf, (CM *)tm_p );       // 暂时不处理
            return;
        } else if ( strstr( buf, "T*" ) ) {
            processTstar( buf, cur_xy_p );
            return;
        }
        
        printf("-------------------------s-------------\n");
    }

    /*
     * 27. processTd()
     *      处理 Td 内的内容
     * 入口参数:
     *      buf                 当前包含Td 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      Td 有多种可能性, 0 Tc 0 Tr 265 0 Td 或者 -135.68 -14.78 Td
     *      1. -135.68 -14.78 Td 在 51134 L115, 这个应该是相对坐标。。。
     *      2. 
     *      奇怪, Td 里面的数据到底是相对坐标？还是绝对坐标？？？
     *      3. 0 Tc 0 Tr 48.26 0 Td            50617  L41
     *      4. 0 Tr 144.54 0 Td                 50617 L44
     *      5. /TT0 9 Tf                        50617 L29
     *          529.68 51.72 Td
     *          (74)Tj
     *          1 0 0 1 538.68 51.72 Tm
     *          ( )Tj
     *      6. 不修改cur_cell, pre_cell 的内容.
     * 2016.12.12:
     *      修正实现方式, Td/TD中的坐标必须用Tm 中的矩阵进行转换, 例如:
     *        0.05 0 0 -0.05 80.52 146.45 Tm

     *        <0E2A>Tj 208.797 0 TD<4801>Tj 211.188 0 TD<31B0>Tj 208.797 0 TD<3020>Tj
     *      那么: 转换矩阵对应的[a b c d e f]=[0.05 0 0 -0.05 80.52 146.45]
     *      根据转换公式: (参见: PDF 1.7 Reference P208)
     *          x' = a*x + c*y + e
     *          y' = b*x + d*y + f
     *      则208.797 0 TD 实际的坐标为:
     *        x' = 0.05 * 208.797 + 0*0 + 80.52 = 184.9165 约184.917
     *        y' = 0 * 208.797 + (-0.05)*0 + 146.45 = 146.45
     *      现在判断文字前面的表格
     *         75.15 134.4 130 16.1 re
     *       75.15 < x' < 75.15+130=205.15
     *       134.4 < y' < 134.4+16.1 = 150.5
     * 2016.12.26:
     *      Td 的处理比较复杂.
     *      1. 如果Tm为空,  如果之前没有坐标, 则(ox,oy) = (x,y); 如果之前有坐标, 则(ox,oy) = (x,y)+(x',y')
     *      2. 如果Tm 不为空, (ox, oy) = (x,y) * tm, 然后 tm 置空
     *
     * depends on:
     *      self.getOriginXY()
     *   
     * 
     */
    // getXYofTd() 函数是本地函数, 简化代码的,仅在processTd() 中调用
    // 0 Tc 0 Tr 48.26 0 Td
    // 获取Td 之前的xy 坐标 
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
        
        while ( i <= len ) {  // 直到尾零
            if ( buf[i] == ' ' || buf[i] == 0 ) {  // 碰到空格或结尾
                memset( buf1, 0, 12 );
                strcpy( buf1, buf2 );       // 第一次循环到这儿, 其实是没有内容的,是空串拷贝, 后续就有内容了
                
                memset( buf1, 0, 12 );
                strcpy( buf2, buf3 );

                memset( buf3, 0, 12 );
                memcpy( buf3, &buf[j], i-j );
                if ( strstr(buf3, "Td" ) )
                    break;
                j = i+1;  // 空格后下一个字符
            }
            i ++;
        }

        cur_xy_p->x = atof( buf1 );
        cur_xy_p->y = atof( buf2 );

        return 0;
    }

    void processTd( char *buf, CUR_XY * cur_xy_p, TM * tm_p )
    {
        getXYofTd( buf, cur_xy_p );     // 获取 Td 之前的xy坐标
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "Td" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // 全为0表示没有Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]赋值
            memset( tm_p, 0, sizeof(TM) );      // 重置tm_p的内容为0
        }
    }

    /*
     * 28. processTD()
     *      处理 TD 内的内容, 获取(x,y) 坐标信息
     * 入口参数:
     *      buf                 当前包含TD 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *
     *  2016.12.10:
     *     原来的处理有问题, :<0016>Tj 91.188 0 TD<0015>Tj
     *     <547F>Tj 180 0 TD<097E>Tj 180 0 TD<4AD2>Tj 180 0 TD<4AF1>Tj 180 0 TD<0836>Tj 180 0 TD<34D7>TjET
     *     这种情况要嵌套处理， 直到一行处理结束(比如找到最后一个操作符 ET)
     *  2019.02.14:
     *     碰到Tj(如果里面有TD)或TD 要互相嵌套处理一整行,确保所有内容都处理完全.
     *     不需要处理TD后面的所有TD, 只需要处理第一个TD即可. 因为所有的TD的y 坐标相同, 之所有时多个TD, 是因为X坐标不同
     *
     * 注意: 
     *     这儿如果是表格内的内容, 可能需要处理最后一个TD来获取最新的位置信息. 待调试！！
     *
     */
    void processTD( char * buf, CUR_XY * cur_xy_p, TM * tm_p ) 
    {
        getXYofTd( buf, cur_xy_p );     // 获取 Td 之前的xy坐标
        
        memset( cur_xy_p->opr, 0, L_OPR);
        strcpy( cur_xy_p->opr, "TD" );
        
        if ( tm_p->a == 0 && tm_p->b == 0 && tm_p->c == 0 && tm_p->c == 0 && 
             tm_p->e == 0 && tm_p->f == 0  ) {        // 全为0表示没有Tm

            cur_xy_p->ox = cur_xy_p->x;
            cur_xy_p->oy = cur_xy_p->y;
        } else {
            getOriginXY( cur_xy_p, tm_p );      //  # cur_xy["ox"], cur_xy["oy"]赋值
            memset( tm_p, 0, sizeof(TM) );      // 重置tm_p的内容为0
        }

    }    

    /*
     * 29. getOriginXY()
     *      获取Td/TD 坐标的Matrics 坐标，因为Td/TD 大部分情况是相对坐标. 
     * 说明:
     *      1. 该方法只有processTd()和 processTD() 调用
     *      2. Tm, re 本身是绝对坐标, 不会调用该方法
     *      3. T* 也不用调用该方法, 因为它直接换行了, 不用进行判断了。
     * 
     *      
     */
    void getOriginXY( CUR_XY * cur_xy_p, TM * tm_p )
    {
        printf("getOriginXY():-------------======================================\n");
        //print(cur_xy)
        //print(tm)

        // 下面来获取ox, oy, 也就是参考坐标的原始坐标值(参照物), 如果x,y 本身就是绝对坐标,则ox,oy = 0,0
        if ( !tm_p ) {      //# 如果没有tm, 也就是没有转换坐标, 则x,y 就是绝对坐标
            if ( cur_xy_p->ox == 0 ){   //   # 如果该坐标是第一个坐标
                cur_xy_p->ox = cur_xy_p->x;
                cur_xy_p->oy = cur_xy_p->y;
            } else {
                cur_xy_p->ox += cur_xy_p->x;
                cur_xy_p->oy += cur_xy_p->y;
            }
        } else {    // 如果有tm, 要进行坐标转换
            //# x' = a*x + c*y+e,  y'=b*x+d*y+f
            cur_xy_p->ox = tm_p->a * cur_xy_p->x + tm_p->c * cur_xy_p->y + tm_p->e;
            cur_xy_p->oy = tm_p->b * cur_xy_p->x + tm_p->d * cur_xy_p->y + tm_p->f;
        }
    }            

    /*
     * 30. processTm()
     *      处理 Tm 内的内容 0.05 0 0 -0.05 233.759 92.57 Tm
     * 入口参数:
     *      buf                 当前包含 Tm 位置信息 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      特别需要注意的是, Tm 是绝对坐标, 处理Tm时, pre_cell, cur_cell 就可以置为[;], preT_xy 也置空[:]
     *      也就是说后面需要以 Tm 的坐标为基准进行计算与判断
     * 2016.12.21:
     *      增加了cur_xy参数. 因为一旦碰到Tm, 那么之前的坐标都没用了. 需要按照新的转换坐标来计算
     *      碰到Tm后, 初始化 (x,y)= (0,0)*Tm= (e,f)
     *      似乎所有的Tm 都在BT/ET 中间. 或许 ET之后, 该Tm 就失效了.
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
                j = i;      // 下一项的起始位置做准备
                

                switch ( n ) {
                    case 1:
                            tm_p->a = atof( buf );      //第1项是 tm_p->a 的值 
                            n ++; 
                            break;
                    case 2:
                            tm_p->b = atof( buf );      //第2项是 tm_p->b 的值 
                            n ++;
                            break;
                    case 3:
                            tm_p->c = atof( buf );      //第3项是 tm_p->c 的值 
                            n ++; 
                            break;
                    case 4: 
                            tm_p->d = atof( buf );      //第4项是 tm_p->d 的值 
                            n ++;
                            break;
                    case 5: 
                            tm_p->e = atof( buf );      //第5项是 tm_p->e 的值 
                            n ++;
                            break;
                    case 6: 
                            tm_p->f = atof( buf );      //第6项是 tm_p->f 的值 
                            n ++;
                            break;
                    default:
                            break;
                }
                
            }
        }

        
        cur_xy_p->x = cur_xy_p->ox = tm_p->e;       // 重置坐标值x
        cur_xy_p->y = cur_xy_p->oy = tm_p->f;       // 重置坐标值x

        memset( cur_xy_p->opr, 0, L_OPR );
        strcpy( cur_xy_p->opr, "Tm" );

        //printTM( tm_p );
        //printCUR_XY( cur_xy_p );

        printf("--------processTM() end !srcbuf=%s\n", srcbuf );
        return;
    }

    /*
    # 30.1 processCm()
    #     转换矩阵, 目前看作用和Tm类似, 回头再深研究
    */
    void processCm( char * buf, CM * cm_p )
    {        
        printf("=================-略过------------------------processTm(),%s\n", buf );
        // 暂时略过
        return;
    }
    
    /*
     * 31. processTstar()
     *      处理 T* 内的内容
     * 入口参数:
     *      streambuf           字节流映射文件 哈希表, 读取内容的时候需要.
     *      buf                 当前包含T* 的一行信息
     *      cmap                字符映射表 哈希表
     * 出口参数:
     *      cur_xy              哈希表, 记录坐标 cur_xy["x"], cur_xy["y"]
     * 返回值:
     *      无
     * 说明:
     *      T*   直接到下一行的起始位置
     *2016.12.13:
     *    这个处理需要完善？？？？？
     *    如果找到T*, 那么前面i-2, i-1就是x,y 
     */
    void processTstar( char * buf, CUR_XY * cur_xy_p )
    {
        printf("processTstar() buf=%s\n", buf);
        cur_xy_p->x = 0;         
        cur_xy_p->y += 1;           //# 设+1 表示与上次处理的位置相比, 下移了, 那么就是下一行
        strcpy( cur_xy_p->opr, "T*" );
        /*
        # 注意， 上面的操作是假设T* 不会是第一个位置操作符， 目前测试也
        # 确实没有第一个的情况, 但是如果真有T*是第一个位置操作符, 就会出异常
        # 因为这个时候pre_xy["x"]等访问是无效的。
        */
        return;
    }
    

