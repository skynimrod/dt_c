. 1. ֱ�ӱ���, ��������Ĵ���:

F:\F_3_test\5_TCC>tcc -shared Adll.c
Adll.c:7: error: declaration for parameter 'firstDLL' but no such parameter

     ԭ���Ǻ������������û�в���, ��Ҫ��void ��ʾ, �����ǿ���.

. 2. ����DLL

    tcc -shared Adll.c

. 3. ���� def

    tiny_impdef.exe Adll.dll

    ����ͬĿ¼�´���һ��Adll.def �ļ�

. 4. ����dll�ļ�

    tcc -L. -lAdll -run Adll_Test.c

    ע��, ���û����-L ָ��·���Ļ�, -l ѡ��ͻ���� tcc: error: cannot find '-lAdll'