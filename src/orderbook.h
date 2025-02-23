/*
*/
#pragma once

#include "portfolio.h"

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
