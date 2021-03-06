#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>       // 产生随机数的时候用时间做种子
#include    <sys\stat.h>

#include "file_tools.h"
#include "exercise.h"
#include "exercise_math.h"
#include "time_tools.h"


           // 拼音表   # āáǎà ōóǒò  ēéěè īíǐì ūúǔù üǖǘǚǜ
	    
	   /*  共 100个字  
	     * 识字
	     *  2     yī     èr      sān     shàng            一   二   三  上
	     *  3     kǒu    mù      ěr      shǒu             口   目   耳  手
	     *  4     rì     tián    hé      huǒ              日   田   禾  火
	     *  5     chóng  yún     shān                      虫   云   山 
	     *  语文园地
	     *  1     bā     shí                                八  十
	     *  课文
	     *  1     le      zǐ      rén     dà               了   子   人  大
	     *  2     yuè    ér      tóu     lǐ               月   儿   头  里
	     *  3     kě     dōng    xī                        可   东   西
	     *  4     tiān   sì      shì                       天   四   是
	     *  语文园地四
	     *  4     nǚ     kāi                                女   开
	     *  识字
	     *  6     shuǐ   qù      lái     bù               水   去   来  不  
	     *  7     xiǎo   shǎo    niú     guǒ   niǎo      小   少   牛  果   鸟
	     *  8     zǎo    shū     dāo     chǐ   běn       早   书   刀  尺   本
	     *  9     mù     lín     tǔ      lì    xīn       木   林   土  力   心
	     *  10    zhōng  wǔ      lì      zhèng            中   五   立  正
	     *  课文
	     *  5     zài    huò     wǒ      hǎo              在   后   我  好
	     *  6     cháng  bǐ      bā      bǎ               长   比   巴  把
	     *  7     xià    gè      yǔ      men               下   个   雨  们
	     *  8     wèn    yǒu     bàn     cóng   nǐ       问   有   半  从  你
	     *  9     cái    míng    tóng    xué              才   明   同  学
	     *  10    zì     jǐ      yi                         自   己   衣
	     *  11    bái    de       yòu     hé               白   的   又  和
	     *  12    zhú    yá      mǎ      yòng   jǐ       竹   牙   马  用  几
	     *  13    zhī    shí     duō     chū    jiàn     只   石   多  出  见
	     *  14    duì    mā      quán    huí              对   妈   全  回
	     *  语文园地八
	     *  8     gōng   chǎng                              工   厂
	     *
	     */
	    // 拼音汉字全放在一起，为了计算空间
	    
	   char sztotal1[]="yīèrsānshàng一二三上kǒumùěrshǒu口目耳手rìtiánhéhuǒ日田禾火chóngyúnshān虫云山"\
	                  "shuǐqùláibù水去来不xiǎoshǎoniúguǒniǎo小少牛果鸟zǎoshūdāochǐběn早书刀尺本"\
			  "mùlíntǔlìxīn木林土力心zhōngwǔlìzhèng中五立正";
	   char *sztotal[]={"yī èr sān shàng","一二三上","kǒu mù ěr shǒu","口目耳手","rì tián hé huǒ","日田禾火","chóng yún shān","虫云山",
	                  "shuǐ qù lái bù","水去来不","xiǎo shǎo niú guǒ niǎo","小少牛果鸟","zǎo shū dāo chǐ běn","早书刀尺本",
			  "mù lín tǔ lì xīn","木林土力心","zhōng wǔ lì zhèng","中五立正"};
	   char *ywyd[] = { "bā shí", "八十", "nǚ kāi", "女开", "gōng chǎng", "工厂" };
	   
	   char *kewen[] = { "le zǐ rén dà", "了子人大", "yuè ér tóu lǐ", "月儿头里", "kě dōng xī", "可东西", "tiān sì shì", "天四是",
	                     "zài huò wǒ hǎo", "在后我好", "cháng bǐ bā bǎ", "长比巴把", "xià gè yǔ men", "下个雨们",
			     "wèn yǒu bàn cóng nǐ", "问有半从你", "cái míng tóng xué", "才明同学", "zì jǐ yi", "自己衣", 
			     "bái de yòu hé", "白的又和", "zhú yá mǎ yòng jǐ", "竹牙马用几", "zhī shí duō chū jiàn", "只石多出见", 
			     "duì mā quán huí", "对妈全回" };
              
    long  __filesize( char * filename )
    {
	    struct stat statbuf;
	    long   	ret;

	    stat( filename, &statbuf );

	    ret = statbuf.st_size;
            printf("filename=%s,Drive letter of file: %c\n", filename,  'A'+statbuf.st_dev); 
            printf("Size of file in bytes: %ld\n", statbuf.st_size); 
            printf("Time file last opened: %s\n", ctime(&statbuf.st_ctime)); 
            return ret; 
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
            if ( num > 112 ) {  // (127-2)/2 取整为112, 因为后面的参数都是成对的, 1个生字表，对应一个生字表限制
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
		    while ( true ) {
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
		    while ( true) {
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

    void  test(int n, ...) 
    {
	    SZ  *p = NULL;
	    va_list  ap;
	    
	    va_start(ap, n);
	    p = va_arg( ap, SZ *);
	    va_end(ap);

	    if ( p == NULL ) {
		    printf(" 结构指针是空指针！！！");
		    return;
            }
	    printf(" 结构指针有效！！！");

            printf("p[0].num=%d, p[0].No=%d\n", p[0].num, p[0].No ); 
	    

	    return;

    }

        char *  adprintf( void * buf )
    { 
	char *     ret;
	printf("\nbuf=%s\n",buf);
	asm( "movb $0x09, %%ah\n\t"
	     "movl %1, %%ebx\n\t"
            : "d"(ret)
	    : "d" ( (long) buf ) 
	   );
	printf("ret = %s \n", ret);
	return ret;
    }




        

    
    void main(void)
{

        /*
    int ret = 0;

    ret = ifOpened("test.txt");
    printf("\nret = %d\n", ret ); 
    if ( ret == 1 )
	 printf("\n文件已经被打开了!\n");   

    char * a = "1234";

    printf( "after adprintf:%s:\n", adprintf( a));

    int  c = atoi(a);
    printf(" a=%s, c=%d", a, c);

    
    FILEMAP *filemap = NULL;
    filemap = initFileMap( "test.txt", filemap );
    printfm( filemap );
    char    tmpbuf[512+1];
    ret = readLine( filemap, tmpbuf, 512 );
    printf("读取长度为: %d, 内容为:%s\n", ret, tmpbuf );
    freeFileMap( filemap );

    getLine(  "test.txt", 10, tmpbuf, 512+1 );
    printf("第10行的内容为:%s\n", tmpbuf );

    char * buf= (char*)malloc(512);
    printf("the size of buf = %d\n", sizeof(buf) );
    
    printf("\nend\n");

    
    char *sztotal[]={"yī èr sān shàng","一二三上","kǒu mù ěr shǒu","口目耳手","rì tián hé huǒ","日田禾火","chóng yún shān","虫云山",
                     "shuǐ qù lái bù","水去来不","xiǎo shǎo niú guǒ niǎo","小少牛果鸟","zǎo shū dāo chǐ běn","早书刀尺本",
                     "mù lín tǔ lì xīn","木林土力心","zhōng wǔ lì zhèng","中五立正"};
    char *ywyd[] = { "bā shí", "八十", "nǚ kāi", "女开", "gōng chǎng", "工厂" };     // 语文园地1， 4， 8
    char *kewen[] = { "le zǐ rén dà", "了子人大", "yuè ér tóu lǐ", "月儿头里", "kě dōng xī", "可东西", "tiān sì shì", "天四是",
                     "zài huò wǒ hǎo", "在后我好", "cháng bǐ bā bǎ", "长比巴把", "xià gè yǔ men", "下个雨们",
                     "wèn yǒu bàn cóng nǐ", "问有半从你", "cái míng tóng xué", "才明同学", "zì jǐ yi", "自己衣", 
		     "bái de yòu hé", "白的又和", "zhú yá mǎ yòng jǐ", "竹牙马用几", "zhī shí duō chū jiàn", "只石多出见", 
		     "duì mā quán huí", "对妈全回" };                 // 课文生字
    
    SZ   * sz, *yw, *kw;
    sz = init_SZ ( sztotal, 2, 9  );       // 初始化1年级上学期 写字表-> 识字
    printf("??sz[0].num=%d, sz[0].No=%d\n", sz[0].num, sz[0].No ); 

    yw = init_SZ ( ywyd, 1, 3  );       // 初始化1年级上学期 写字表-> 语文园地
    printf("??yw[0].num=%d, yw[0].No=%d\n", yw[0].num, yw[0].No ); 
    yw[0].No = 1; yw[1].No = 4; yw[2].No = 8;              // 手工修改语文园地编号, 不是顺序的

  //  kw = init_SZ ( kewen, 1, 14  );       // 初始化1年级上学期 写字表-> 课文
  //  printf("??kw[0].num=%d, kw[0].No=%d\n", kw[0].num, kw[0].No ); 

    time_t start = clock(), end; 
    //getMatchProblem( 8, 3, start, sz, yw, kw, 9,8,12, 9, 3, 14 );

    
    char *title ="姓名:             日期:            时间:                做对:        (50道)";
    char * filename = "练习.txt";

    int maxLines = 56, limit = 50, rows = 10, cols = 5, days = 30;
    
    printf("-------------------now begin math exercise!------------\n");
    getDaysExercise( maxLines, limit, title, rows, cols, filename, days );
    */
/*
    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");

    getMatchProblem( 8, 3, start, sz, yw, kw, 3,1,5 );
    printf("\n");
*/
    //test(1, sz);


  //  freeSZ( sz, 9 );
  //  freeSZ(yw,3);
  //  freeSZ(kw, 14);
  //  free(buf);

    int r1 = rand();
    int r2 = rand();

    printf("r1=%d, r2=%d\n", r1, r2);

    long   size = __filesize("./r.dat");

    printf("/nsize of r.dat=%ld/n", size);
   
    return;
}
