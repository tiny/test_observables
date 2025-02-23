/*
*/
#include <stdio.h>
#include <gonumeric.h>
#include <cb_lambda.h>
#include "portfolio_test.h"

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
    x >> poke( func_1 ) ;
    y >> poke( func_2 ) ;
} // :: init

void simple_rules_test()
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
} // :: simple_rules_test

int main(int argc, char *argv[])
{
    simple_rules_test() ;
    portfolio_test() ;  

    return 0;
} // :: main
