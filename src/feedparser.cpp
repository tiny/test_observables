/*
*/
#include "feedparser.h"


// convert "9999799 15/16" to double "9999799.9375" 
//
double frac2double( const char *frac )
{
  uint32_t  x = 0 ;
  uint32_t  a = 0 ;
  uint32_t  b = 0 ;
  const char   *c = frac ;

  while ((*c) && (*c != ' ')) {
    x = x*10 + (*c++ - '0') ;
  }
  if (*c == ' ') {
    c++;
    while ((*c) && (*c != '/')) {
      a = a*10 + (*c++ - '0') ;
    }
    c++;
    while (*c) {
      b = b*10 + (*c++ - '0') ;
    }
    return ((double) x + ((double)a / (double)b)) ;
  }
  return (double)x ;
} // :: frac2double

uint32_t str2int( const char *qty )
{
  uint32_t  x = 0 ;
  const char   *c = qty ;
  while (*c) {
    x = x*10 + (*c++ - '0') ;
  }
  return x ;
} // :: str2int

uint32_t  __base_stock_tm = 0 ;
uint32_t str2stocktm( const char *tm )
{
  uint32_t  h = 0;
  uint32_t  m = 0 ;
  uint32_t  t = 0 ;
  const char   *c = tm ;
  while ((*c) && (*c != ':')) {
    h = h*10 + (*c++ - '0') ;
  }
  c++ ;
  while ((*c) && (*c != ':')) {
    m = m*10 + (*c++ - '0') ;
  }
  t = h * 60*60 + m * 60 ;
  if (__base_stock_tm == 0)  __base_stock_tm = t ;
  return (t - __base_stock_tm) ;
} // :: str2stocktm


FeedParser::FeedParser() {}

FeedParser::~FeedParser() {}

#define BUFFER_SIZE  (10*1024*1024)

void FeedParser::parse( const std::string &fname, IssueMgr &iMgr ) 
{
    // open file
    FILE    *inf = fopen( fname.c_str(), "rb" ) ;
    if (inf == NULL)
        return ;
    
    // read file
    uint32_t     buffer_sz = BUFFER_SIZE ;
    uint32_t     pos = 0 ;
    uint8_t      buffer[ BUFFER_SIZE ] ;
    TickPacket  *tp = (TickPacket *)buffer ;

    while (!feof(inf)
    {
        tp->rd_packet( inf ) ;
        // parse packet
    }
    fclose( inf ) ;
} // :: parse

