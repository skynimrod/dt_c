. ��ȡ�ļ����Ե�����

       http://blog.csdn.net/huapeng_guo/article/details/7614978

. �����ʱ������ deflate() , deflateEnd() û�е����ɹ�. ������Zlib.h �е�ͬ������������Ҳ������DLL_EXPORT�� �͵����ɹ���.

. ��tiny_impdef zlib.dll ������zlib.def �ļ�, �鿴�����ĺ�������. Ҳ����ͨ��depends.exe ��ֱ�Ӳ鿴zlib.dll �е����ĺ�������.

. �ܹ���: 5 + 22 + 32 + 6 + 6 + 16 = 87������

. ��������

  1. zlibVersion()

  2. deflate()

  3. deflateEnd()

  4. inflate()

  5. inflatedEnd()

. ��ǿ����

  1. deflateSetDictionary()

  2. deflateGetDictionary()

  3. deflateCopy()

  4. deflateReset()

  5. deflateParam()

  6. deflateTune();

  7. deflateBound()

  8. deflatePending()

  9. deflatePrime()

  10.deflateSetHeader()

  11.inflateSetDictionary()

  12.inflateGetDictionary()

  13.inflateSync()

  14.inflateCopy()

  15.inflateReset()

  16.inflateReset2()

  17.inflatePrime()

  18.inflateMark()

  19.inflateGetHeader()

  20.inflateBack()

  21.inflateBackEnd()

  22.zlibCompileFlags()

. �����ຯ��

  1. compress()

  2. compress2()

  3. compressBound()

  4. uncompress()

  5. uncompress2()

  6. gzopen()

  7. gzdopen()

  8. gzbuffer()

  9. gzsetparams()

  10.gzread()

  11.gzfread()

  12.gzwrite()

  13.gzfwrite()

  14.gzprintf()

  15.gzvprintf()

  16.gzputs()

  17.gzgets()

  18.gzputc()

  19.gzgetc()

  20.gzungetc()

  21.gzflush()

  22.gzseek()

  23.gzrewind()

  24.gztell()

  25.gzoffset()

  26.gzeof()
 
  27.gzdirect()

  28.gzclose()

  29.gzclose_r()

  30.gzclose_w()

  31.gzeror()

  32.gzclearerr()

. ���ļ�����

  1. gzopen64()

  2. gzseek64()

  3. gztell64()

  4. gzoffset64()

  5. adler32_combine64()

  6. crc32_combine64()

. У�麯��

  1. adler32()

  2. adler32_z()

  3. crc32()

  4. crc32_z()

  5. adler32_combine()

  6. crc32_combine()

. ����hacks,��:)

  1. deflateInit_()

  2. deflateInit2_()

  3. inflateInit_()

  4. inflateInit2_()

  5. inflateBackInit_()

  6. gzgetc_()

  7. zError()

  9. inflateSyncPoint()

  10.get_crc_table()

  11.inflateUndermine()

  12.inflateValidate()

  13.inflateCodesUsed()

  14.inflateResetKeep()

  15.deflateResetKeep()

  16.gzopen_w()

.-------------------------------

. zlib.h

  typedef voidpf (*alloc_func) OF((voidpf opaque, uInt items, uInt size));
  typedef void   (*free_func)  OF((voidpf opaque, voidpf address));

ZEXTERN const char * ZEXPORT zlibVersion OF((void));
const char * DLL_EXPORT zlibVersion()
{
    return ZLIB_VERSION;
}

. zconf.h

#ifndef OF /* function prototypes */
#  ifdef STDC
#    define OF(args)  args
#  else
#    define OF(args)  ()
#  endif
#endif

#ifdef STDC
   typedef void const *voidpc;
   typedef void FAR   *voidpf;
   typedef void       *voidp;
#else
   typedef Byte const *voidpc;
   typedef Byte FAR   *voidpf;
   typedef Byte       *voidp;
#endif

typedef unsigned char  Byte;  /* 8 bits */
#endif
typedef unsigned int   uInt;  /* 16 bits or more */
typedef unsigned long  uLong; /* 32 bits or more */

#ifdef SMALL_MEDIUM
   /* Borland C/C++ and some old MSC versions ignore FAR inside typedef */
#  define Bytef Byte FAR
#else
   typedef Byte  FAR Bytef;
#endif
typedef char  FAR charf;
typedef int   FAR intf;
typedef uInt  FAR uIntf;
typedef uLong FAR uLongf;