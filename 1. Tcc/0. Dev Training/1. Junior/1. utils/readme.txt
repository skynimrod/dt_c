. 如果使用 __stdcall , 用tcc编译就不会产生def文件,  如果用 __declspec(dllexport), tcc 编译产生dll的同时会产生def文件.

#define DLL_EXPORT __declspec(dllexport)   //__stdcall

. 在makefile文件中, 如果命令前面用@, 就会不会显示该命令的执行(实际已经执行了)

. 编译makefile 用 nmake

. 2019.01.10

 1. Astring.dll  归并到str_tools.dll 中.

 2. FileApi.dll  合并到 file_tools.dll中

 3. 