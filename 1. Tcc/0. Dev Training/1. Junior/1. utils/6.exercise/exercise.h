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
	    
    // 写字表
    typedef struct __sz {    // 生字
	    int     grade;   // 年级, 1-12 对应小学1-6年级尚下学期
	    int     No;           // 编号, 有可能是识字中的生字，也可能是课文中的生字
	    int     num;     // 汉字数量
	    char    **hz;   // 汉字      "一二三上"
	    char    ** py;  // 拼音      {"yīèrsānshàng"
    }SZ;
    typedef struct __xzTbl {
	   SZ  *shizi;        //  识字中的生字
	   SZ  *kewen;        //  课文中的生字
	   SZ  *ywyd;         //  语文园地中的生字

    }XZTBL;

    SZ * init_SZ ( char **sztotal, int start, int total );       // 初始化生字表
    void freeSZ( SZ * sz, int NoNum ); 
    #endif                   // define exercise.h ENd }
