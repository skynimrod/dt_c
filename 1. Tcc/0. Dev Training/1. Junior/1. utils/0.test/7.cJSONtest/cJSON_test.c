
    #include <stdio.h>
    #include "cJSON.h"

    void main(void)
    {
        int    a;

        printf("\n����������:");
        //scanf("����������%d", &a);   // ���������Ҫ����"����������"Ȼ����������Ҫ�����ֲſ���.
        scanf("%d", &a);
        printf("\na=%d(%d)\n",a, pow2gt(a));
        printf("\n21=%d\n", pow2gt(21 ));
    }
