/*
*/
#include <stdio.h>
#include <gonumeric.h>
#include <cb_lambda.h>

// data model
go::GOInteger x, y, z ;

void show_business_rules() 
{
    printf( "business rules:\n" ) ;
    printf( "---------------\n" ) ;    
    printf( "x = < literal >\n" ) ;
    printf( "y = x + 2\n" ) ;
    printf( "z = y * 3\n" ) ;
    printf( "---------------\n" ) ;
} // :: show_business_rules

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
    show_business_rules() ;
    init() ; // hook up dependencies

    printf( "\nchanging just x and printing all values\n" ) ;
    printf( "----------------------------------------\n" ) ;
    x = 5 ;
    printf("x = %2d, y = %2d, z = %2d\n", (int)x, (int)y, (int)z) ;
    x = 1 ;
    printf("x = %2d, y = %2d, z = %2d\n", (int)x, (int)y, (int)z) ;
    x = 9 ;
    printf("x = %2d, y = %2d, z = %2d\n", (int)x, (int)y, (int)z) ;

    printf( "\nnote the values adhere to the business rules\n" ) ;
    printf( "\n" ) ;
    
    return 0;
} // :: main
