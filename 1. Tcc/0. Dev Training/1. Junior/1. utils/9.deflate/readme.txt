. deflate 解码

  关于rfc1951的解读 参见  E_1_Developement_tools\101.rfc 中的相关文档

. 需要一下头文件


  zutil.h -> zlib.h -> zconf.h

. zcalloc  在zconf.h 中声明了， 但是实现在别的地方。
       