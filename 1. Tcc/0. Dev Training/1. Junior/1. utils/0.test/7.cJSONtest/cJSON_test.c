
    #include <stdio.h>
    #include "cJSON.h"

    void main(void)
    {
        int    a;

        printf("\n请输入数字:");
        //scanf("请输入数字%d", &a);   // 这种情况需要输入"请输入数字"然后再输入需要的数字才可以.
        scanf("%d", &a);
        printf("\na=%d(%d)\n",a, pow2gt(a));
        printf("\n21=%d\n", pow2gt(21 ));
    }
