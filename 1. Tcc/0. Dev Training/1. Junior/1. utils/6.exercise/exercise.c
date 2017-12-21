    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    exercise
    //
    // Description : about exercise generation operation.
    //       参见: 
    //       
    // Interface :
    //        1.   IfFileExist()
    //        2.   IfOpened()
    //        3.   ifDirExist()
    //        4.   
    //
    #include    <stdio.h>
    #include    <string.h>
    #include    <time.h>

    #include    "exercise.h"

    #define DLL_EXPORT __declspec(dllexport)


    //**************************************************************************
    //  函数名称 : init_SZ()
    //  功    能 :
    //		初始化生字表
    //  入口参数 :
    //      char **    sztotal : 二维字符串数组, 包括生字拼音
    //      int        start   : 起始课号, 有些生字表示从第二课开始的, 这儿是为了记录课文编号
    //      int        total   : 课文总数.
    //
    //  出口参数 :
    //            无
    //  返 回 值 :
    //            SZ  * : 二维结构数组指针, 生字，拼音，课号, 以及该课的生字总数.
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		相当于数据库表结构.         
    //	Standard .h  :
    //	        stdlib.h  ---- malloc(),
    //	        stdio.h   ---- printf();
    //          string.h  ---- strlen(), memset(),  memcpy(), strtok()
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    SZ * DLL_EXPORT init_SZ ( char **sztotal, int start, int total )       // 初始化生字表
    { 
	   SZ  * sztbl;

	   // 识字中的生字
	   int		hznum  = 0, len = 0;   // 用来临时记录字符串长度, len 用来计二维数组需要的内存空间
	   char		* result = NULL;    // 用来拆分拼音
	   int          NoNum = total;        //  sizeof(sztotal)/sizeof(char *) 就是有多少个字符串, 除以2,  就得到有多少编号
	   //int		rec = 0, lastrec = 0;   // 用来记录一条记录的长度

	   sztbl = (SZ *) malloc( NoNum * sizeof(SZ) );
	   //printf("处理前: NoNum=%d\n", NoNum);
	   
	   for ( int i = 0; i < NoNum; i ++ ) {     // 
		memset( &sztbl[i], 0, sizeof(SZ) );        // 初始化

		sztbl[i].No = i+start;                         // 识字中的编号

		
		hznum  = strlen( sztotal[ i*2 + 1 ] )/2;  // 每个汉字2个字节, 除以2后就是汉字的个数

		sztbl[i].num  = hznum;                         // 汉字数量
		//printf( "hznum=%d       ??\n", hznum );

		//汉字部分,  每个汉字申请一个指针
		sztbl[i].hz = (char **)malloc(  hznum * sizeof(char * ) );
		for ( int j = 0; j < hznum; j ++ ) {
			sztbl[i].hz[j] = (char *)malloc( 2 + 1);    // 1个汉字占用2个字节
			memset(sztbl[i].hz[j], 0, 2+1);
			memcpy(sztbl[i].hz[j], sztotal[i*2+1]+j*2, 2 );
			//printf( "i = %d,j =%d, 汉字:%s;\n", i,j,sztbl[i].hz[j] );
		}

		// 拼音部分
		sztbl[i].py = (char **)malloc( hznum * sizeof(char *) );
		
		result = strtok( sztotal[i*2], " " );

		for ( int j = 0; j < hznum && result != NULL; j ++ ) {
                        //printf( "拼音为:%s\n", result );
			len = strlen( result )  ;
			sztbl[i].py[j] = (char *)malloc( len + 1 );    // 1个汉字占用2个字节
			
			memset(sztbl[i].py[j], 0, len + 1);
			memcpy(sztbl[i].py[j], result, len );  // 获取拼音
			
			//printf( "i = %d,j =%d, 拼音:%s;\n", i,j,sztbl[i].py[j] );
			result = strtok( NULL, " " );
		}
		
	   }
	   
           printf("Init End: sz[0].num=%d, sz[0].No=%d\n", sztbl[0].num, sztbl[0].No ); 

	   return sztbl;
    }

    //**************************************************************************
    //  函数名称 : freeSZ()
    //  功    能 :
    //		释放 生字表指针 
    //  入口参数 :
    //      char **    sztotal : 二维字符串数组, 包括生字拼音
    //      int        start   : 起始课号, 有些生字表示从第二课开始的, 这儿是为了记录课文编号
    //      int        total   : 课文总数.
    //
    //  出口参数 :
    //            无
    //  返 回 值 :
    //            SZ  * : 二维结构数组指针, 生字，拼音，课号, 以及该课的生字总数.
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	使用说明 :
    //		相当于数据库表结构.         
    //	Standard .h  :
    //	        stdio.h   ---- printf();
    //	        stdlib.h  ---- free()
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console、MFC Application for Windows
    //***************************************************************************
    //
    void DLL_EXPORT freeSZ( SZ * sz, int NoNum ) 
    {
	    int  hznum = 0 ;
	    if ( sz != NULL ) {
       	          //printf("--NoNum=%d-------\n", NoNum );
                  for ( int i = 0; i < NoNum; i ++ ) {
                        hznum = sz[i].num;
			//printf( "            hznum =%d,  sz[i].num=%d, sz[i].No=%d\n", hznum,  sz[i].num, sz[i].No ); 
			for ( int j = 0; j < hznum; j ++ ) {
				//printf( "    汉字:%s, 拼音:%s\n", sz[i].hz[j], sz[i].py[j] );
				if ( sz[i].hz[j] != NULL )
					free(sz[i].hz[j] );
				
				if ( sz[i].py[j] != NULL )
					free(sz[i].py[j] );
			}
			if ( sz[i].hz != NULL ) free( sz[i].hz );
			if ( sz[i].py != NULL ) free( sz[i].py );
			
		}
		free( sz );
	    }
    }

        // 判断一个数字序列中是否包含某个数字
    int ifContain( int len, int *queue, int n ) 
    {
	    int    i =0, ret = -1;
	    for ( i=0; i < len; i ++ ) {
		if ( queue[i] == n ) {
		    ret = i;
		    break;
	        }
	    }
	    return ret;
    }

    // 获取生字拼音连线题, 生字范围是可变的, num:生字表个数(对应后面的生字表限制条件个数), 后面的变参: arg0: 生字表1, ..., arg(num+0):第一个生字表的限制, ....
    //     生字表的限制指的是不能超过所学范围
    //     lines:  是连线个数, 也就是出题的生字个数
    //     如果限制条件为<=0, 表示无限制.
    //     最后的num个参数是 各个生字表中的课数. 便于处理结构指针.
    // 例如: 
    #define MAXARGS  127
    // start 参数用来计算毫秒, 是为了初始化随机数
    char * getMatchProblem( int  lines, int num, time_t start, ... )
    {

	    va_list  ap;
	    SZ       * sz[(MAXARGS -2)/3];
            int      limit[(MAXARGS-2)/3];  
	    int      KeTotal[(MAXARGS-2)/3];
	    int      ptr_no = 0;    // 用来计数变参个数, 不能超过31个
	    time_t   end;
	    
	    //printf( "开始出题! 连线数=%d, 生字表个数=%d\n", lines, num );
            if ( num > (MAXARGS-3)/3 ) {  // (127-3)/3 取整为41, 因为后面的参数都是3个一组的, 1个生字表，对应一个生字表限制
		    return NULL;
	    }

	    va_start( ap, start );  
	    while ( ptr_no < num ) 
		    sz[ptr_no++] = va_arg( ap, SZ *);
	    
	    while ( ptr_no < num * 2 ) { 
		    limit[(ptr_no-num)] = va_arg( ap,int);
		    ptr_no ++;
	    }

	    while ( ptr_no < num * 3 ) { 
		    KeTotal[(ptr_no-num*2)] = va_arg( ap,int);
		    ptr_no ++;
	    }

	    va_end( ap );


	    // 下面产生随机数来取生字
	    // 1. 产生随机数来获取生字出处,  0: 识字, 1: 语文园地, 2: 课文
	    // 2. 产生随机数来获取生字位置   第几课<=limit, 第几个生字
	    int    KeNo, ZiNo;    // 课编号, 字编号
	    int    lim, sum;      // sum 是课内汉字数
	    char   hz[lines][3], py[lines][8];     // 单个汉字是2个字节, 再加1个尾零,算3字节.  拼音最长的是7字节(chuáng), 加尾零1字节, 是8字节.
	    char  recs [512];      // 记录"{出处:课编号:字编号},"
	    char  rec[20];
	    int    j;
	    struct __rec{
		    int         lim;        // 出处
		    int		KeNo;  // 课文编号
		    int		no;    // 生字编号
	    }rec1;
	    
	    struct __rec recs1[lines];

	    memset( recs, 0, 512);

	    int     l = 0, p1=0, p2 = 0;  // 2 个计数器, 用来记录拼接的位置
	    char space[] = "          ";
	    
            Sleep(1);        // 确保连续调用的时候初始化随机数不会相同.
	    end = clock();
	    srand( (unsigned int)(end-start) );
	    char   buf[20];
	    for ( int i =0; i < lines; i ++ ) {
		    while ( 1 ) {
			lim = rand() % num;         // 出处
		        //printf(" %d\n", lim );
		     
			if ( lim != 1 )     // 不是语文园地的话, 不存在跳跃课号 
   		            KeNo = rand() % limit[lim];     // 限制内的某课
			else {
			    for( int x=0; x < KeTotal[lim]; x++ ) {   // 语文园地有可能是跳跃课号,比如: 1,4,8, 所以要判断随机数是否是有效的, 如果无效就继续产生随机数.
		                KeNo =  rand() % limit[lim];  
				if ( KeNo == KeTotal[x] ) 
				    break;

			    }
			}

    		        sum = sz[lim][KeNo].num;        // 获取该课的生字数量
  		        j = rand() % sum;

    		        memset( rec, 0, 20);
			//sprintf( rec, "{%d:%d:%d}", lim, KeNo, j );    // 一旦用了sprintf() , 就修改了py1[i]的内容。。。。
			rec1.lim = lim;   rec1.KeNo = KeNo; rec1.no = j;

                        memset( buf, 0, 20);
			itoa( lim, buf, 10 );
			memcpy( rec + strlen(rec), buf, strlen(buf));

                        memset( buf, 0, 20);
			itoa( KeNo, buf, 10 );
			memcpy( rec + strlen(rec), buf, strlen(buf));

                        memset( buf, 0, 20);
			itoa( j, buf, 10 );
			memcpy( rec + strlen(rec), buf, strlen(buf));

			memset( hz[i], 0, 3 );
			memset( py[i], 0, 8 );
			//printf( "rec = %s\n", rec);
			if ( strstr( recs, rec ) == NULL ) {  // 新选的字之前没有选中
			    memcpy( hz[i],  sz[lim][KeNo].hz[j], strlen( sz[lim][KeNo].hz[j]) );
			    memcpy( py[i],  sz[lim][KeNo].py[j], strlen( sz[lim][KeNo].py[j]) );

			    memcpy( recs+strlen(recs), rec, strlen(rec) );
			    break;
			}
			
		    }

	    }

	    for ( int j = 0; j < lines; j ++ ) {
		    printf("%-10s", hz[j]);
	    }
	    printf("\n\n\n");

	    int   rec3[lines];
	    for (int i=0;i < lines; i ++) rec3[i] = -1;
	    int    k=-1;
	    int   flag = -1;    // 缺省认为生字没有重复
	    for ( int j = 0; j < lines; j ++ ) {
   		    //Asrand();         // 封装后的初始化随机数, 以秒为随机数, 防止出现重复
         	    end = clock();
	            srand( (unsigned int)(end-start) );
		    while ( 1) {
		        k = rand() % lines;
			flag = ifContain( lines, rec3, k );

			if (flag == -1  ) {  // 如果flag 是 -1 , 则说明该生字没有重复, 否则重复了
			    for ( int i =0; i < lines; i ++ ) {
				if ( rec3[i] == -1 ) {
			    	    rec3[i] = k;
		            	    printf("%-11s", py[k]);
				    break;
				}
		    	     }
		             break;
			}
		    }
	    }
	    printf("\n");

	    return NULL;
    }

