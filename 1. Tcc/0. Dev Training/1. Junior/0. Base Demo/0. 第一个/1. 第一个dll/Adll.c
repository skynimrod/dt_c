/*
 * Adll.c
 * tcc -shared Adll.c 
 * tiny_impdef Adll.dll    # ���� Adll.def �ļ�
 * tcc -L. -lAdll -run Adll_Test.c  ������dll
 */

#define DLL_EXPORT __declspec(dllexport)
    DLL_EXPORT void firstDLL(void)
    {
         printf("this is first dll demo.");
         return(0);
    } 