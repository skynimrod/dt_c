#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>       // �����������ʱ����ʱ��������

#include "file_tools.h"

           // ƴ����   # �������� ��������  �������� �������� �������� ����������
	    
	   /*  �� 100����  
	     * ʶ��
	     *  2     y��     ��r      s��n     sh��ng            һ   ��   ��  ��
	     *  3     k��u    m��      ��r      sh��u             ��   Ŀ   ��  ��
	     *  4     r��     ti��n    h��      hu��              ��   ��   ��  ��
	     *  5     ch��ng  y��n     sh��n                      ��   ��   ɽ 
	     *  ����԰��
	     *  1     b��     sh��                                ��  ʮ
	     *  ����
	     *  1     le      z��      r��n     d��               ��   ��   ��  ��
	     *  2     yu��    ��r      t��u     l��               ��   ��   ͷ  ��
	     *  3     k��     d��ng    x��                        ��   ��   ��
	     *  4     ti��n   s��      sh��                       ��   ��   ��
	     *  ����԰����
	     *  4     n��     k��i                                Ů   ��
	     *  ʶ��
	     *  6     shu��   q��      l��i     b��               ˮ   ȥ   ��  ��  
	     *  7     xi��o   sh��o    ni��     gu��   ni��o      С   ��   ţ  ��   ��
	     *  8     z��o    sh��     d��o     ch��   b��n       ��   ��   ��  ��   ��
	     *  9     m��     l��n     t��      l��    x��n       ľ   ��   ��  ��   ��
	     *  10    zh��ng  w��      l��      zh��ng            ��   ��   ��  ��
	     *  ����
	     *  5     z��i    hu��     w��      h��o              ��   ��   ��  ��
	     *  6     ch��ng  b��      b��      b��               ��   ��   ��  ��
	     *  7     xi��    g��      y��      men               ��   ��   ��  ��
	     *  8     w��n    y��u     b��n     c��ng   n��       ��   ��   ��  ��  ��
	     *  9     c��i    m��ng    t��ng    xu��              ��   ��   ͬ  ѧ
	     *  10    z��     j��      yi                         ��   ��   ��
	     *  11    b��i    de       y��u     h��               ��   ��   ��  ��
	     *  12    zh��    y��      m��      y��ng   j��       ��   ��   ��  ��  ��
	     *  13    zh��    sh��     du��     ch��    ji��n     ֻ   ʯ   ��  ��  ��
	     *  14    du��    m��      qu��n    hu��              ��   ��   ȫ  ��
	     *  ����԰�ذ�
	     *  8     g��ng   ch��ng                              ��   ��
	     *
	     */
	    // ƴ������ȫ����һ��Ϊ�˼���ռ�
	    
	   char sztotal1[]="y����rs��nsh��ngһ������k��um����rsh��u��Ŀ����r��ti��nh��hu������̻�ch��ngy��nsh��n����ɽ"\
	                  "shu��q��l��ib��ˮȥ����xi��osh��oni��gu��ni��oС��ţ����z��osh��d��och��b��n���鵶�߱�"\
			  "m��l��nt��l��x��nľ��������zh��ngw��l��zh��ng��������";
	   char *sztotal[]={"y�� ��r s��n sh��ng","һ������","k��u m�� ��r sh��u","��Ŀ����","r�� ti��n h�� hu��","����̻�","ch��ng y��n sh��n","����ɽ",
	                  "shu�� q�� l��i b��","ˮȥ����","xi��o sh��o ni�� gu�� ni��o","С��ţ����","z��o sh�� d��o ch�� b��n","���鵶�߱�",
			  "m�� l��n t�� l�� x��n","ľ��������","zh��ng w�� l�� zh��ng","��������"};
	   char *ywyd[] = { "b�� sh��", "��ʮ", "n�� k��i", "Ů��", "g��ng ch��ng", "����" };
	   
	   char *kewen[] = { "le z�� r��n d��", "�����˴�", "yu�� ��r t��u l��", "�¶�ͷ��", "k�� d��ng x��", "�ɶ���", "ti��n s�� sh��", "������",
	                     "z��i hu�� w�� h��o", "�ں��Һ�", "ch��ng b�� b�� b��", "���ȰͰ�", "xi�� g�� y�� men", "�¸�����",
			     "w��n y��u b��n c��ng n��", "���а����", "c��i m��ng t��ng xu��", "����ͬѧ", "z�� j�� yi", "�Լ���", 
			     "b��i de y��u h��", "�׵��ֺ�", "zh�� y�� m�� y��ng j��", "�������ü�", "zh�� sh�� du�� ch�� ji��n", "ֻʯ�����", 
			     "du�� m�� qu��n hu��", "����ȫ��" };
              

    // д�ֱ�
    typedef struct __sz {    // ����
	    int     No;           // ���, �п�����ʶ���е����֣�Ҳ�����ǿ����е�����
	    int     num;     // ��������
	    char    **hz;   // ����      "һ������"
	    char    ** py;  // ƴ��      {"y����rs��nsh��ng"
    }SZ;
    typedef struct __xzTbl {
	   SZ  *shizi;        //  ʶ���е�����
	   SZ  *kewen;        //  �����е�����
	   SZ  *ywyd;         //  ����԰���е�����

    }XZTBL;


    /*
     * start  ����ʼ���,  total ��ָ�ܹ��ж��ٿ�
     */
    SZ * init_SZ ( char **sztotal, int start, int total )       // ��ʼ�����ֱ�
    { 
	   SZ  * sztbl;

	   // ʶ���е�����
	   int		hznum  = 0, len = 0;   // ������ʱ��¼�ַ�������, len �����ƶ�ά������Ҫ���ڴ�ռ�
	   char		* result = NULL;    // �������ƴ��
	   int          NoNum = total;        //  sizeof(sztotal)/sizeof(char *) �����ж��ٸ��ַ���, ����2,  �͵õ��ж��ٱ��
	   //int		rec = 0, lastrec = 0;   // ������¼һ����¼�ĳ���

	   sztbl = (SZ *) malloc( NoNum * sizeof(SZ) );
	   //printf("����ǰ: NoNum=%d\n", NoNum);
	   
	   for ( int i = 0; i < NoNum; i ++ ) {     // 
		memset( &sztbl[i], 0, sizeof(SZ) );        // ��ʼ��

		sztbl[i].No = i+start;                         // ʶ���еı��

		
		hznum  = strlen( sztotal[ i*2 + 1 ] )/2;  // ÿ������2���ֽ�, ����2����Ǻ��ֵĸ���

		sztbl[i].num  = hznum;                         // ��������
		//printf( "hznum=%d       ??\n", hznum );

		//���ֲ���,  ÿ����������һ��ָ��
		sztbl[i].hz = (char **)malloc(  hznum * sizeof(char * ) );
		for ( int j = 0; j < hznum; j ++ ) {
			sztbl[i].hz[j] = (char *)malloc( 2 + 1);    // 1������ռ��2���ֽ�
			memset(sztbl[i].hz[j], 0, 2+1);
			memcpy(sztbl[i].hz[j], sztotal[i*2+1]+j*2, 2 );
			//printf( "i = %d,j =%d, ����:%s;\n", i,j,sztbl[i].hz[j] );
		}

		// ƴ������
		sztbl[i].py = (char **)malloc( hznum * sizeof(char *) );
		
		result = strtok( sztotal[i*2], " " );

		for ( int j = 0; j < hznum && result != NULL; j ++ ) {
                        //printf( "ƴ��Ϊ:%s\n", result );
			len = strlen( result )  ;
			sztbl[i].py[j] = (char *)malloc( len + 1 );    // 1������ռ��2���ֽ�
			
			memset(sztbl[i].py[j], 0, len + 1);
			memcpy(sztbl[i].py[j], result, len );  // ��ȡƴ��
			
			//printf( "i = %d,j =%d, ƴ��:%s;\n", i,j,sztbl[i].py[j] );
			result = strtok( NULL, " " );
		}
		
	   }
	   
           printf("Init End: sz[0].num=%d, sz[0].No=%d\n", sztbl[0].num, sztbl[0].No ); 

	   return sztbl;
    }

    void freeSZ( SZ * sz, int NoNum ) 
    {
	    int  hznum = 0 ;
	    if ( sz != NULL ) {
       	          //printf("--NoNum=%d-------\n", NoNum );
                  for ( int i = 0; i < NoNum; i ++ ) {
                        hznum = sz[i].num;
			//printf( "            hznum =%d,  sz[i].num=%d, sz[i].No=%d\n", hznum,  sz[i].num, sz[i].No ); 
			for ( int j = 0; j < hznum; j ++ ) {
				//printf( "    ����:%s, ƴ��:%s\n", sz[i].hz[j], sz[i].py[j] );
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

        // �ж�һ�������������Ƿ����ĳ������
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

    // ��ȡ����ƴ��������, ���ַ�Χ�ǿɱ��, num:���ֱ����(��Ӧ��������ֱ�������������), ����ı��: arg0: ���ֱ�1, ..., arg(num+0):��һ�����ֱ������, ....
    //     ���ֱ������ָ���ǲ��ܳ�����ѧ��Χ
    //     lines:  �����߸���, Ҳ���ǳ�������ָ���
    //     �����������Ϊ<=0, ��ʾ������.
    //     ����num�������� �������ֱ��еĿ���. ���ڴ���ṹָ��.
    // ����: 
    #define MAXARGS  127
    // start ���������������, ��Ϊ�˳�ʼ�������
    char * getMatchProblem( int  lines, int num, time_t start, ... )
    {

	    va_list  ap;
	    SZ       * sz[(MAXARGS -2)/3];
            int      limit[(MAXARGS-2)/3];  
	    int      KeTotal[(MAXARGS-2)/3];
	    int      ptr_no = 0;    // ����������θ���, ���ܳ���31��
	    time_t   end;
	    
	    //printf( "��ʼ����! ������=%d, ���ֱ����=%d\n", lines, num );
            if ( num > 112 ) {  // (127-2)/2 ȡ��Ϊ112, ��Ϊ����Ĳ������ǳɶԵ�, 1�����ֱ���Ӧһ�����ֱ�����
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


	    // ��������������ȡ����
	    // 1. �������������ȡ���ֳ���,  0: ʶ��, 1: ����԰��, 2: ����
	    // 2. �������������ȡ����λ��   �ڼ���<=limit, �ڼ�������
	    int    KeNo, ZiNo;    // �α��, �ֱ��
	    int    lim, sum;      // sum �ǿ��ں�����
	    char   hz[lines][3], py[lines][8];     // ����������2���ֽ�, �ټ�1��β��,��3�ֽ�.  ƴ�������7�ֽ�(chu��ng), ��β��1�ֽ�, ��8�ֽ�.
	    char  recs [512];      // ��¼"{����:�α��:�ֱ��},"
	    char  rec[20];
	    int    j;
	    struct __rec{
		    int         lim;        // ����
		    int		KeNo;  // ���ı��
		    int		no;    // ���ֱ��
	    }rec1;
	    
	    struct __rec recs1[lines];

	    memset( recs, 0, 512);

	    int     l = 0, p1=0, p2 = 0;  // 2 ��������, ������¼ƴ�ӵ�λ��
	    char space[] = "          ";
	    
            Sleep(1);        // ȷ���������õ�ʱ���ʼ�������������ͬ.
	    end = clock();
	    srand( (unsigned int)(end-start) );
	    char   buf[20];
	    for ( int i =0; i < lines; i ++ ) {
		    while ( true ) {
			lim = rand() % num;         // ����
		        //printf(" %d\n", lim );
		     
			if ( lim != 1 )     // ��������԰�صĻ�, ��������Ծ�κ� 
   		            KeNo = rand() % limit[lim];     // �����ڵ�ĳ��
			else {
			    for( int x=0; x < KeTotal[lim]; x++ ) {   // ����԰���п�������Ծ�κ�,����: 1,4,8, ����Ҫ�ж�������Ƿ�����Ч��, �����Ч�ͼ������������.
		                KeNo =  rand() % limit[lim];  
				if ( KeNo == KeTotal[x] ) 
				    break;

			    }
			}

    		        sum = sz[lim][KeNo].num;        // ��ȡ�ÿε���������
  		        j = rand() % sum;

    		        memset( rec, 0, 20);
			//sprintf( rec, "{%d:%d:%d}", lim, KeNo, j );    // һ������sprintf() , ���޸���py1[i]�����ݡ�������
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
			if ( strstr( recs, rec ) == NULL ) {  // ��ѡ����֮ǰû��ѡ��
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
	    int   flag = -1;    // ȱʡ��Ϊ����û���ظ�
	    for ( int j = 0; j < lines; j ++ ) {
   		    //Asrand();         // ��װ��ĳ�ʼ�������, ����Ϊ�����, ��ֹ�����ظ�
         	    end = clock();
	            srand( (unsigned int)(end-start) );
		    while ( true) {
		        k = rand() % lines;
			flag = ifContain( lines, rec3, k );

			if (flag == -1  ) {  // ���flag �� -1 , ��˵��������û���ظ�, �����ظ���
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
		    printf(" �ṹָ���ǿ�ָ�룡����");
		    return;
            }
	    printf(" �ṹָ����Ч������");

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
    int ret = 0;

    ret = ifOpened("test.txt");
    printf("\nret = %d\n", ret ); 
    if ( ret == 1 )
	 printf("\n�ļ��Ѿ�������!\n");   

    char * a = "1234";

    printf( "after adprintf:%s:\n", adprintf( a));

    int  c = atoi(a);
    printf(" a=%s, c=%d", a, c);

    
    FILEMAP *filemap = NULL;
    filemap = initFileMap( "test.txt", filemap );
    printfm( filemap );
    char    tmpbuf[512+1];
    ret = readLine( filemap, tmpbuf, 512 );
    printf("��ȡ����Ϊ: %d, ����Ϊ:%s\n", ret, tmpbuf );
    freeFileMap( filemap );

    getLine(  "test.txt", 10, tmpbuf, 512+1 );
    printf("��10�е�����Ϊ:%s\n", tmpbuf );

    char * buf= (char*)malloc(512);
    printf("the size of buf = %d\n", sizeof(buf) );
    
    printf("\nend\n");

    
    char *sztotal[]={"y�� ��r s��n sh��ng","һ������","k��u m�� ��r sh��u","��Ŀ����","r�� ti��n h�� hu��","����̻�","ch��ng y��n sh��n","����ɽ",
                     "shu�� q�� l��i b��","ˮȥ����","xi��o sh��o ni�� gu�� ni��o","С��ţ����","z��o sh�� d��o ch�� b��n","���鵶�߱�",
                     "m�� l��n t�� l�� x��n","ľ��������","zh��ng w�� l�� zh��ng","��������"};
    char *ywyd[] = { "b�� sh��", "��ʮ", "n�� k��i", "Ů��", "g��ng ch��ng", "����" };     // ����԰��1�� 4�� 8
    char *kewen[] = { "le z�� r��n d��", "�����˴�", "yu�� ��r t��u l��", "�¶�ͷ��", "k�� d��ng x��", "�ɶ���", "ti��n s�� sh��", "������",
                     "z��i hu�� w�� h��o", "�ں��Һ�", "ch��ng b�� b�� b��", "���ȰͰ�", "xi�� g�� y�� men", "�¸�����",
                     "w��n y��u b��n c��ng n��", "���а����", "c��i m��ng t��ng xu��", "����ͬѧ", "z�� j�� yi", "�Լ���", 
		     "b��i de y��u h��", "�׵��ֺ�", "zh�� y�� m�� y��ng j��", "�������ü�", "zh�� sh�� du�� ch�� ji��n", "ֻʯ�����", 
		     "du�� m�� qu��n hu��", "����ȫ��" };                 // ��������
    
    SZ   * sz, *yw, *kw;
    sz = init_SZ ( sztotal, 2, 9  );       // ��ʼ��1�꼶��ѧ�� д�ֱ�-> ʶ��
    printf("??sz[0].num=%d, sz[0].No=%d\n", sz[0].num, sz[0].No ); 

    yw = init_SZ ( ywyd, 1, 3  );       // ��ʼ��1�꼶��ѧ�� д�ֱ�-> ����԰��
    printf("??yw[0].num=%d, yw[0].No=%d\n", yw[0].num, yw[0].No ); 
    yw[0].No = 1; yw[1].No = 4; yw[2].No = 8;              // �ֹ��޸�����԰�ر��, ����˳���

    kw = init_SZ ( kewen, 1, 14  );       // ��ʼ��1�꼶��ѧ�� д�ֱ�-> ����
    printf("??kw[0].num=%d, kw[0].No=%d\n", kw[0].num, kw[0].No ); 

    time_t start = clock(); 
    getMatchProblem( 8, 3, start, sz, yw, kw, 9,8,12, 9, 3, 14 );
    printf("\n");
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
    test(1, sz);


    freeSZ( sz, 9 );
    freeSZ(yw,3);
    freeSZ(kw, 14);
    free(buf);

    int r1 = rand();
    int r2 = rand();

    printf("r1=%d, r2=%d\n", r1, r2);
   
    return;
}
