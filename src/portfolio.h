/*
*/
#pragma once

#include "holding.h"

typedef uint32_t  PortfolioID ;

class Portfolio
{
  public  :
    PortfolioID    _id ;
    Price          _current ; // current value of the portfolio
    Price          _cost ;    // total cost for all holdings
    Price          _net ;     // total net value of the portfolio
    HoldingMap     _holdings ;

    virtual short  onHoldingChange( const go::ArgList *args )
                   {
                     _current += (args->asDouble(0) - args->asDouble(1)) ; // add difference
                     _net = _current - _cost ;
                     return 0 ;
                   }
                   Portfolio( const PortfolioID &id )
                   :_id(id)
                   {
                   }
    short          holding( Holding *h )
                   {
                     _holdings.insert( HoldingMap_pair( h->_id, h )) ;
                     h->_current.valueCB().install( new go::Observer<Portfolio>( this, &Portfolio::onHoldingChange ) ) ;
                     _current += h->_current ;
                     _cost += h->_cost ;
                     return 0 ;
                   }
} ; // class Portfolio

typedef std::map< PortfolioID, Portfolio* >                PortfolioMap ;
typedef std::map< PortfolioID, Portfolio* >::iterator      PortfolioMap_iter ;
typedef std::map< PortfolioID, Portfolio* >::value_type    PortfolioMap_pair ;

class PortfolioMgr
{
  public  :
    PortfolioMap   _portfolios ;

    short          portfolio( Portfolio *p )
                   {
                     _portfolios.insert( PortfolioMap_pair( p->_id, p )) ;
                     return 0 ;
                   }
    Portfolio     *get( PortfolioID id ) {
                      PortfolioMap_iter  iter = _portfolios.find( id ) ;
                      return (iter == _portfolios.end()) ? NULL : (*iter).second ;
                   } 
} ; // class PortfolioMgr

void show( Portfolio *p )
{
  HoldingMap_iter  iter ;
  for (iter = p->_holdings.begin(); iter != p->_holdings.end(); iter++)
  {
    printf( "  %ld  %6.6s  %6d  %9.3lf  %9.3lf  %9.3lf\n", 
            (long)  (*iter).second->_id, 
                    (*iter).second->_issue->_symbol.c_str(),
                    (*iter).second->_price._qty,
            (double)(*iter).second->_current,
            (double)(*iter).second->_cost,
            (double)(*iter).second->_net
            ) ;
  }
} // :: show

void show( PortfolioMgr &pMgr )
{
  PortfolioMap_iter  iter ;
  printf( " id  current\n" ) ;
  printf( "------------\n" ) ;
//  for (iter = pMgr._portfolios.begin(); iter != pMgr._portfolios.end(); iter++)
  for (int id = 101; id < 103; id++)
  {
    Portfolio *p = pMgr.get( id ) ;
    if (p != NULL)
    {
      printf( "%ld  %8.3lf\n", (long)p->_id, (double)p->_current ) ;
      show( p ) ;
    }
  } 
} // :: show

