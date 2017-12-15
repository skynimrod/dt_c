// The first c demo
// compile this c file:
//   tcc helloworld.c
// will create a helloworld.exe in the same path
// run helloworld.exe:
//    c:...> helloworld

void main() 
{
    printf("Hello World!\n");
    
    int  a = -7, b = 4;

    int c = a % 4;

    printf(" -7 %% 4 = %d " , c);

    return 0;
}