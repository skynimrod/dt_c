    //**************************************************
    //
    //     Copyright (C)  2017-  AQUGEN TECH Co., LTD
    //               All Rights Reserved
    //
    //**************************************************
    //**************************************************
    // Model :    exercise_math.h
    //
    // Description : about exercise generation operation.
    //
    // Interface :
    //        1.   ()
    //        2.   ()
    //        3.   ()
    //        4.   
    //
    #ifndef __EXERCISE_MATH_H__    // define exercise_math.h Start {
    #define __EXERCISE_MATH_H__
    char * addlimited ( int limit );       // ����һ�����Ʒ�Χ�ڵļӷ���ʽ
    char * minuslimited(int limit ); 
    int    genEquation( int limit, int row, int col, char * title, char * filename );
    void   getDaysExercise( int maxLines, int limit, char *title, int rows, int cols, char * filename, int days );
    #endif                   // define exercise_math.h ENd }