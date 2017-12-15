/*
 * Adll.c
 * tcc -shared Adll.c 
 * tiny_impdef Adll.dll    # 生成 Adll.def 文件
 * tcc -L. -lAdll -run Adll_Test.c  来测试dll
 */

#define DLL_EXPORT __declspec(dllexport)
    DLL_EXPORT void firstDLL(void)
    {
         printf("this is first dll demo.");
         return(0);
    } 