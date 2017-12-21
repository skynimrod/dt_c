    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    exercise.h
    //
    // Description : about exercise generation operation.
    //
    // Interface :
    //        1.   ()
    //        2.   ()
    //        3.   ()
    //        4.   
    //
    #ifndef __EXERCISE_H__    // define exercise.h Start {
    #define __EXERCISE_H__
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
	    
    // д�ֱ�
    typedef struct __sz {    // ����
	    int     grade;   // �꼶, 1-12 ��ӦСѧ1-6�꼶����ѧ��
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

    SZ * init_SZ ( char **sztotal, int start, int total );       // ��ʼ�����ֱ�
    void freeSZ( SZ * sz, int NoNum ); 
    #endif                   // define exercise.h ENd }
