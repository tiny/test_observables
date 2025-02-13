/*
  portfolio_test.cpp
  test app for portfolio speed testing

  author      rmcinnis
  date        april 05, 2004
*/
#include <stdio.h>
#include <stdint.h>
#include <gonumeric.h>
#include <gostring.h>
#include <pointintime.h>
#include <map>
#include "rand_between.h"
#include <stopwatch.h>

using namespace std ; 

#ifdef LINUX
#  define max(a,b)    std::max(a,b)
#  define min(a,b)    std::min(a,b)
#endif

typedef go::GOString   Currency ;
typedef uint32_t       LotSize ;
typedef go::GOLong     Volume ;

class Price : public go::GODouble
{
  private :
    Currency      *_currency ;

  public  :
                   Price( double amt = 0.0, Currency *currency = NULL ) 
                   : go::GODouble( amt )
                   {
                     _currency = currency ;
                   }
                   Price( const Price &p ) { *this = p ; }
    Price         &operator=( const Price &p )
                   {
                     go::GODouble::operator=( (double)p ) ;
                     _currency = p._currency ;
                     return *this ;
                   }
    Price         &operator=( const go::GODouble &d )
                   {
                     go::GODouble::operator=((double)d ) ;
                     return *this ;
                   }
    Price         &operator=( const double &d )
                   {
                     go::GODouble::operator=( d ) ;
                     return *this ;
                   }

    // access methods
    Currency      *currency() { return _currency ; }
} ; // class Price

class Offer
{
  public   :
    Price          _price ;
    go::PointInTime    _ts ;
    LotSize        _qty ;

                   Offer( const Price &p = Price(), const LotSize &qty = LotSize(), const go::PointInTime &ts = go::PointInTime() )
                   : _price( p ), _ts( ts ), _qty( qty ) 
                   {
                   }
    bool           operator< ( const Offer &f ) const
                   { return (_price < f._price) ;
                   }
    bool           operator> ( const Offer &f ) const
                   { return (_price > f._price) ;
                   }
    Offer         &operator= ( const Offer &f )
                   {
                     _price = f._price ;
                     _ts    = f._ts ;
                     _qty   = f._qty ;
                     return *this ;
                   }

    // access methods
    Price         &price() { return _price ; }
    go::PointInTime   &ts() { return _ts ; }
    LotSize       &qty() { return _qty ; }
} ; // class Offer

class Issue
{
  public  :
    go::GOString        _symbol ;
    Offer               _lastTrade ;
    Offer               _best_ask ;
    Offer               _best_bid ;
    Offer               _day_hi ;
    Offer               _day_lo ;
    Price               _close;
    Price               _open ;
    Price               _prevClose ;
    go::GOLong          _nTrades ;
    Volume              _volume ;

                        Issue( const go::GOString &sym )
                        : _symbol( sym )
                        {
                        }
} ; // class Issue

typedef map< go::GOString, Issue* >                IssueMap ;
typedef map< go::GOString, Issue* >::iterator      IssueMap_iter ;
typedef map< go::GOString, Issue* >::value_type    IssueMap_pair ;

class IssueMgr
{
  private :
    IssueMap       _issues ;

  public  :
                   IssueMgr() {}
    Issue         *find(const go::GOString& sym)
                   {
                     IssueMap_iter  iter = _issues.find(sym);
                     return (iter == _issues.end()) ? NULL : (*iter).second;
                   }
    Issue         *get(const go::GOString& sym)
                   {
                     IssueMap_iter  iter = _issues.find(sym);
                     if (iter == _issues.end()) {
                       auto rc = _issues.insert(IssueMap_pair(sym, new Issue(sym)));
                       iter = rc.first;
                     }
                     return (iter == _issues.end()) ? NULL : (*iter).second;
                   }
    short          issue( const go::GOString &sym )
                   {
                     IssueMap_iter  iter = _issues.find( sym ) ;
                     if (iter != _issues.end())
                       return 0 ;
                     _issues.insert( IssueMap_pair( sym, new Issue( sym ))) ;
                     return 0 ;
                   }
    short          trade( const go::GOString &sym, const Offer &f ) 
                   {
                     IssueMap_iter  iter = _issues.find( sym ) ;
                     if (iter == _issues.end())
                       return -1 ;
                     (*iter).second->_lastTrade = f ;
                     (*iter).second->_volume += f._qty ;
                     (*iter).second->_nTrades++ ;
                     if ((*iter).second->_nTrades == 1) {
                       (*iter).second->_open = f._price ;
                       (*iter).second->_day_lo = f ;
                       (*iter).second->_day_hi = f ;
                     }
                     if (f < (*iter).second->_day_lo)
                       (*iter).second->_day_lo = f ;
                     if (f > (*iter).second->_day_hi)
                       (*iter).second->_day_hi = f ;
                     return 0 ;
                   }

    IssueMap      &issues() { return _issues ; }
} ; // class IssueMgr 

class Purchase
{
  public  :
    Price          _price ;
    Price          _cost ;
    go::PointInTime    _ts ;
    LotSize        _qty ;

                   Purchase( const Price &p, const Price &cost, const LotSize &q, const go::PointInTime &ts ) 
                   : _price( p ), _cost( cost ), _qty(q), _ts(ts)
                   {
                   }
                   Purchase( const Purchase &p )
                   : _price( p._price ), _cost( p._cost ), _qty(p._qty), _ts(p._ts)
                   {
                   }
} ; // class Purchase

typedef uint32_t  HoldingID ;

class Holding
{
  public  :
    HoldingID      _id ;
    Issue         *_issue ;
    Purchase       _price ;   // purchase price, ts, and qty
    Price          _current ; // current value
    Price          _sell_hi ; // sale point - hi
    Price          _sell_lo ; // sale point - lo
    Price          _cost ;    // original cost of holding
    Price          _net ;     // net on stock to date

    virtual void   onPriceChange()
                   {
                     _current = _issue->_lastTrade._price * (double)_price._qty ;
                     _net = _current - _cost ;
                   }

                   Holding( HoldingID id, Issue *i, const Purchase &price )
                   : _id( id ), _issue(i), _price(price)
                   {
                     _cost = _price._cost ;
                     _current = _issue->_lastTrade._price * (double)_price._qty ;
                     _net = _current - _cost ;
                     i->_lastTrade._price.valueCB().install( new go::PokeObserver<Holding>( this, &Holding::onPriceChange )) ;
                   }
} ; // class Holding

typedef map< HoldingID, Holding* >                HoldingMap ;
typedef map< HoldingID, Holding* >::iterator      HoldingMap_iter ;
typedef map< HoldingID, Holding* >::value_type    HoldingMap_pair ;

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
                     _current += (args->asDouble(1) - args->asDouble(0)) ; // add difference
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

typedef map< PortfolioID, Portfolio* >                PortfolioMap ;
typedef map< PortfolioID, Portfolio* >::iterator      PortfolioMap_iter ;
typedef map< PortfolioID, Portfolio* >::value_type    PortfolioMap_pair ;

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

typedef  uint32_t  OrderEntryID ;

class OrderEntry
{
  public  :
    OrderEntryID   _id ;
} ; // class OrderEntry

typedef map< OrderEntryID, OrderEntry* >                OrderEntryMap ;
typedef map< OrderEntryID, OrderEntry* >::iterator      OrderEntryMap_iter ;
typedef map< OrderEntryID, OrderEntry* >::value_type    OrderEntryMap_pair ;

typedef uint32_t  OrderID ;

class Order
{
  public  :
    OrderID        _id ;
    OrderEntryMap  _entries ;
} ; // class Order

typedef map< OrderID, Order* >                OrderMap ;
typedef map< OrderID, Order* >::iterator      OrderMap_iter ;
typedef map< OrderID, Order* >::value_type    OrderMap_pair ;

class OrderBook
{
  public  :
    OrderMap   _active_orders ;
    OrderMap   _cancelled_orders ;
    OrderMap   _completed_orders ;

} ; // class OrderBook

void show( Issue *s )
{
  printf( "%6.6s  %7.3lf  %7.3lf  %7.3lf  %8ld   %8ld  %4ld\n", 
          s->_symbol.c_str(), 
          (double)s->_lastTrade._price, 
          (double)s->_day_hi._price, 
          (double)s->_day_lo._price, 
          (long)s->_nTrades, 
          (long)s->_volume,
          s->_lastTrade._price.valueCB().nWatchers()
        ) ;
} // :: show

void show( IssueMgr &iMgr )
{
  IssueMap_iter  iter ;

  printf( "\n" ) ;
  printf( "symbol     last       hi       lo   nTrades     volume  nInvestors\n" ) ;
  printf( "-------------------------------------------------------------\n" ) ;
  for (iter = iMgr.issues().begin(); iter != iMgr.issues().end(); iter++)
  {
    show( (*iter).second ) ;
  }
  printf( "\n" ) ;
} // :: show

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

go::GOString gbl_issuenames[] =
{ 
  "SITECH", "MSFT", "SUNW", "YHOO",  "AMZN", 
  "LNUX", "RHAT", "ORCL", "CSCO", "DIGL", 
  "AMD", "INTC", "JDSU", "T", "ARBA",
  "RTN", "NOC", "LMT", "NEM", "PAAS", 
  "GD", "MVL", "BORL", "ET", "AMTD", 
  "TYC", (const char*)NULL
} ;

//extern uint32_t    go::Callback::_nInvokes ; // lives in go_data/msgcb.cpp

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
  for (j = 0; j < nPortfolios; j++)
  {
    p = new Portfolio( PortfolioID(100+j) ) ;
    for (k = 0; k < nHoldings; k++)
    {
      tmp = iMgr.get( gbl_issuenames[ rand_between(0, nIssues) ] ) ;
      if (tmp != NULL)
      {
        pershare = tmp->_close + (double)rand_between( -2, 3 ) ;
        if (pershare <= 1.0) pershare = 1.0;
        lot = rand_between( 1, 5 ) * 100 ;
        h = new Holding( 200+j*nHoldings+k, tmp, Purchase( Price(pershare), Price(5.00),LotSize(lot), go::PointInTime() ) ) ;
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


