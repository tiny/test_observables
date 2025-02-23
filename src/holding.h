/*
*/
#pragma once

#include "issue.h"

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

    virtual short  onPriceChange( const go::ArgList *args )
                   {
                      _current = args->asDouble(0) * (double)_price._qty ;
                     _net = _current - _cost ;
                     return 0;
                   }

                   Holding( HoldingID id, Issue *i, const Purchase &price )
                   : _id( id ), _issue(i), _price(price)
                   {
                     _cost = _price._cost ;
                     _current = _issue->_lastTrade._price * (double)_price._qty ;
                     _net = _current - _cost ;
                     i->_lastTrade._price.valueCB().install( new go::Observer<Holding>( this, &Holding::onPriceChange )) ;
                   }
} ; // class Holding

typedef std::map< HoldingID, Holding* >                HoldingMap ;
typedef std::map< HoldingID, Holding* >::iterator      HoldingMap_iter ;
typedef std::map< HoldingID, Holding* >::value_type    HoldingMap_pair ;

