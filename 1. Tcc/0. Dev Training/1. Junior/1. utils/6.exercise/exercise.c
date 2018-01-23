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
    //       �μ�: 
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
    //  �������� : init_SZ()
    //  ��    �� :
    //		��ʼ�����ֱ�
    //  ��ڲ��� :
    //      char **    sztotal : ��ά�ַ�������, ��������ƴ��
    //      int        start   : ��ʼ�κ�, ��Щ���ֱ�ʾ�ӵڶ��ο�ʼ��, �����Ϊ�˼�¼���ı��
    //      int        total   : ��������.
    //
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            SZ  * : ��ά�ṹ����ָ��, ���֣�ƴ�����κ�, �Լ��ÿε���������.
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		�൱�����ݿ���ṹ.         
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
    //				Can be used in console��MFC Application for Windows
    //***************************************************************************
    //
    SZ * DLL_EXPORT init_SZ ( char **sztotal, int start, int total )       // ��ʼ�����ֱ�
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

    //**************************************************************************
    //  �������� : freeSZ()
    //  ��    �� :
    //		�ͷ� ���ֱ�ָ�� 
    //  ��ڲ��� :
    //      char **    sztotal : ��ά�ַ�������, ��������ƴ��
    //      int        start   : ��ʼ�κ�, ��Щ���ֱ�ʾ�ӵڶ��ο�ʼ��, �����Ϊ�˼�¼���ı��
    //      int        total   : ��������.
    //
    //  ���ڲ��� :
    //            ��
    //  �� �� ֵ :
    //            SZ  * : ��ά�ṹ����ָ��, ���֣�ƴ�����κ�, �Լ��ÿε���������.
    //  Author :
    //            Adams
    //  Create Date :
    //            2017.12.12
    //  Modify history (modifier and date) :
    //
    //	ʹ��˵�� :
    //		�൱�����ݿ���ṹ.         
    //	Standard .h  :
    //	        stdio.h   ---- printf();
    //	        stdlib.h  ---- free()
    //	Develope Tools:
    //			  SCO UNIX Development CC   
    //			  Visual C++ 6.0            
    //	Environment:
    //			1. SCO UNIX 5.0.4          
    //			2. Windows NT WorkStation SimpleChinese 4.0 
    //				Can be used in console��MFC Application for Windows
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

    // ��ȡ����ƴ��������, ���ַ�Χ�ǿɱ��, num:���ֱ�����(��Ӧ��������ֱ�������������), ����ı��: arg0: ���ֱ�1, ..., arg(num+0):��һ�����ֱ�������, ....
    //     ���ֱ�������ָ���ǲ��ܳ�����ѧ��Χ
    //     lines:  �����߸���, Ҳ���ǳ�������ָ���
    //     �����������Ϊ<=0, ��ʾ������.
    //     ����num�������� �������ֱ��еĿ���. ���ڴ����ṹָ��.
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
	    
	    //printf( "��ʼ����! ������=%d, ���ֱ�����=%d\n", lines, num );
            if ( num > (MAXARGS-3)/3 ) {  // (127-3)/3 ȡ��Ϊ41, ��Ϊ����Ĳ�������3��һ���, 1�����ֱ�����Ӧһ�����ֱ�����
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
		    while ( 1 ) {
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
		    while ( 1) {
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
