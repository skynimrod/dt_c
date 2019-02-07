. 获取文件属性的文章

       http://blog.csdn.net/huapeng_guo/article/details/7614978

. 以下 源程序是独立无依赖的

  1. adler32.c

  2. crc32.c

  3. gzlib.c

  4. trees.c
 
  5. zutil.c 

. 有依赖关系的源程序

  1. gzclose.c

  2. gzwrite.c ---- deflate.c

     gz_error(), deflateInit2_(), deflate(), deflateReset(), deflateParams(), deflateEnd()

  3. gzread.c ----- 

     gz_error(), deflateInit2_(), deflate(), deflateReset(),  deflateEnd()

  4. 
     