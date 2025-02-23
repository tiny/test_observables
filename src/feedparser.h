/*
*/
#pragma once

#include <string>   
#include "issue.h"

#define MSG_HEADSTONE   0x5449434B   /* TICK */

#define MSG_TIMESTAMP   100
#define MSG_LEVEL_1     101 /* trade */
#define MSG_LEVEL_2     102 /* ask and bid */
#define MSG_TRADE_HI    103
#define MSG_TRADE_LO    104

#define STX    0x02
#define ETX    0x03
#define SEP    0x09


struct TickPacket
{
  uint32_t  headstone;
  uint16_t  msgid;
  uint16_t  sz;
  uint8_t   payload[2];

  void to_host() {
    headstone = ntohl(headstone);
    msgid = ntohs(msgid);
    sz = ntohs(sz);
  }
  void to_network() {
    headstone = htonl(headstone);
    msgid = htons(msgid);
    sz = htons(sz);
  }

  int16_t  hdr_size() const { return offsetof(TickPacket, payload); }
  int16_t  rd_packet( FILE *inf ) {
    int16_t amt = fread( this, hdr_size(), 1, inf ) ;
    to_host();
    amt += fread( this, sz, 1, inf ) ;
    return amt;
  }

  uint16_t  size() { return (offsetof(TickPacket, payload) + sz); }
} ; 

class FeedParser
{
  public:
    FeedParser() ;
    ~FeedParser() ;

    void parse( const std::string &fname, IssueMgr &iMgr ) ;
} ; // class FeedParser 

