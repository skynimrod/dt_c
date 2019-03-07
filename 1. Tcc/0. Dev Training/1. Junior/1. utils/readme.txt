. 如果使用 __stdcall , 用tcc编译就不会产生def文件,  如果用 __declspec(dllexport), tcc 编译产生dll的同时会产生def文件.

#define DLL_EXPORT __declspec(dllexport)   //__stdcall

. 在makefile文件中, 如果命令前面用@, 就会不会显示该命令的执行(实际已经执行了)

. 编译makefile 用 nmake

. 2019.01.10

 1. Astring.dll  归并到str_tools.dll 中.

 2. FileApi.dll  合并到 file_tools.dll中

 3. 

. 2019.03.07

  修正了 pdf_tools 的内存泄露问题. 原因是  第一次遍历文本的时候， cellMap_p->txtTotal 会根据文本是否在cell 来增加, 但是并没有真正为txtIDs_p 申请内存并存放对应textID, 但是printCellMap()里面确实直接访问  txtIDs_p[i], 那么就出现了内存泄露.  修改printCellMap()， 不判断txtTotal ， 而是判断txtIDs_p 是否已经申请内存了, 再决定是否显示cell 中的文本编号.

  