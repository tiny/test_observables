/*
*/
#pragma once

#include <stdint.h>
#include <map>
#include <gostring.h>
#include <gonumeric.h>
#include <pointintime.h>

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

typedef std::map< go::GOString, Issue* >                IssueMap ;
typedef std::map< go::GOString, Issue* >::iterator      IssueMap_iter ;
typedef std::map< go::GOString, Issue* >::value_type    IssueMap_pair ;

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

