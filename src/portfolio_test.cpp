/*
  portfolio_test.cpp
  test app for portfolio speed testing

  author      rmcinnis
  date        april 05, 2004
*/
#include <stdio.h>
#include "rand_between.h"
#include <stopwatch.h>
#include "portfolio.h"

using namespace std ; 

go::GOString gbl_issuenames[] =
{ 
  "SITECH", "MSFT", "SUNW", "YHOO",  "AMZN", 
  "LNUX", "RHAT", "ORCL", "CSCO", "DIGL", 
  "AMD", "INTC", "JDSU", "T", "ARBA",
  "RTN", "NOC", "LMT", "NEM", "PAAS", 
  "GD", "MVL", "BORL", "ET", "AMTD", 
  "TYC", (const char*)NULL
} ;

void portfolio_test() 
{
  srand( ::time(NULL) ) ;
  go::GOString  sitech( "SITECH" ) ;

  PortfolioMgr     pMgr ;
  IssueMgr         iMgr ;
  Portfolio       *p ;
  Holding         *h ;
  Issue           *i, *tmp ;
  long             j, k, nIssues = 0, nHoldings = 10, nPortfolios = 200 ;
  long             a, b, lot, count = 10000 ;
  uint32_t         invokeCount = 0 ;
  double           pershare, t ;

  // populate issues manager
  j = 0 ;
  while (!gbl_issuenames[j].isEmpty())
  {
    i = iMgr.get( gbl_issuenames[j] ) ;
    if (i != NULL)
    {
      i->_close = rand_between(10, 40);
      i->_lastTrade._price = i->_close;
      i->_lastTrade._qty   = rand_between(1, 5) * 100;
    }
    j++ ;
    nIssues++ ;
  }

  // banner
  printf( "\n" ) ;
  printf( "portfolio_test:\n" ) ;
  printf( "  This test will simulate %ld portfolios, each with %ld positions\n", nPortfolios, nHoldings ) ;
  printf( "  observing from a set of %ld stocks.  As a trade is registered,\n", nIssues ) ;
  printf( "  each stock will have its appropriate data updated, and dependent\n" ) ;
  printf( "  portfolios will then be updated.\n\n" ) ;

  // create portfolios
  HoldingID    hid = 200 ;
  PortfolioID  pid = 100 ;
  for (j = 0; j < nPortfolios; j++)
  {
    p = new Portfolio( pid++ ) ;
    for (k = 0; k < nHoldings; k++)
    {
      tmp = iMgr.get( gbl_issuenames[ rand_between(0, nIssues) ] ) ;
      if (tmp != NULL)
      {
        pershare = tmp->_close + (double)rand_between( -2, 3 ) ;
        if (pershare <= 1.0) pershare = 1.0;
        lot = rand_between( 1, 5 ) * 100 ;
        h = new Holding( hid++, tmp, Purchase( Price(pershare), Price(5.00), LotSize(lot), go::PointInTime() ) ) ;
        p->holding( h ) ;
      }
    }
    pMgr.portfolio( p ) ;
  }

  show( pMgr ) ;

  go::StopWatch    timer ;
  go::PointInTime  ts ;
  Offer        o ;
  o.qty()  = 100 ;
  o.ts()   = ts ;
  o._price = 24.00 ;

  // push trades
  invokeCount = go::Callback::_nInvokes ;

  timer.start() ;

  k = 0 ;
  for (j = 0; j < count; j++)
  {
    // creating a fake trade using 3 rands definitely adds time to the per-trade time
    tmp = iMgr.get( gbl_issuenames[rand_between(0, nIssues)] ) ;
    o._price = tmp->_lastTrade._price + (double)rand_between(-2, 3);
    if (o._price <= 1.0) o._price = 1.0; // don't let it walk below 1.00
    o._qty = rand_between(1, 5) * 100;

    iMgr.trade( tmp->_symbol, o ) ;
  }

  timer.stop() ;
  int64_t  ops_tm = timer.diff_usec() ;

  printf( "\n" ) ;
  printf( "number of portfolios:  %ld\n", pMgr._portfolios.size() ) ;
  printf( "number of holdings per portfolio:  %ld\n", nHoldings ) ;
  printf( "number of trades pushed: %ld\n", count ) ;
  printf( "number of invokes:  %d\n", (go::Callback::_nInvokes - invokeCount) ) ;
  printf( "total operation time:  %ld useconds\n", ops_tm ) ;
  printf( "average time per invoke:  %5.3lf useconds\n", (double)ops_tm/(double)(go::Callback::_nInvokes - invokeCount) ) ;
  printf( "average invokes per trade:  %5.3lf invokes\n\n", (double)(go::Callback::_nInvokes - invokeCount)/(double)(count) ) ;
  show( iMgr ) ;
  show( pMgr ) ;
} // :: portfolio_test


