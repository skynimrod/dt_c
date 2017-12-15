. 1. 直接编译, 出现下面的错误:

F:\F_3_test\5_TCC>tcc -shared Adll.c
Adll.c:7: error: declaration for parameter 'firstDLL' but no such parameter

     原因是函数定义中如果没有参数, 需要用void 表示, 而不是空着.

. 2. 生成DLL

    tcc -shared Adll.c

. 3. 生成 def

    tiny_impdef.exe Adll.dll

    会在同目录下创建一个Adll.def 文件

. 4. 访问dll文件

    tcc -L. -lAdll -run Adll_Test.c

    注意, 如果没有用-L 指定路径的话, -l 选项就会出错 tcc: error: cannot find '-lAdll'