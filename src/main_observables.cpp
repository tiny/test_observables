/*
*/
#include <stdio.h>
#include <gonumeric.h>
#include <cb_lambda.h>

go::GOInteger x, y, z ;

void func_1()
{
    y = x + 2 ;
} // :: func_1

void func_2()
{
    z = y * 3 ;
} // :: func_2

void init()
{
    x.valueCB().install( new go::LambdaPokeCB( func_1 )) ;
    y.valueCB().install( new go::LambdaPokeCB( func_2 )) ;
} // :: init

int main(int argc, char *argv[])
{
    printf("Hello, World!\n");
    init() ;
    x = 5 ;
    printf("x = %d, y = %d, z = %d\n", (int)x, (int)y, (int)z) ;
    x = 1 ;
    printf("x = %d, y = %d, z = %d\n", (int)x, (int)y, (int)z) ;
    x = 9 ;
    printf("x = %d, y = %d, z = %d\n", (int)x, (int)y, (int)z) ;
    
    return 0;
} // :: main
