/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#include "util/Macros.h"

namespace sl
{
namespace cabl
{

//--------------------------------------------------------------------------------------------------

/**
  \class Font
  \brief The font base class

*/
class Font
{

public:

  virtual uint8_t  getWidth()         const noexcept = 0;
  virtual uint8_t  getHeight()        const noexcept = 0;
  virtual uint8_t  getCharSpacing()   const noexcept = 0;
  
  virtual uint8_t  getFirstChar()     const noexcept = 0;
  virtual uint8_t  getLastChar()      const noexcept = 0;
  
  virtual uint8_t  getBytesPerLine()  const noexcept = 0;
  
  virtual bool     getPixel( uint8_t char_, uint8_t x_, uint8_t y_ ) const noexcept = 0;
  
  virtual inline bool getPixelImpl(
    uint8_t* pFontData_, 
    uint8_t c_, 
    uint8_t x_, 
    uint8_t y_ 
  ) const noexcept
  {
    if( c_ > getLastChar() || x_ >= getWidth() || y_ >= getHeight() )
      return false;
    
    if( getBytesPerLine() == 1 )
    {
      return ( ( pFontData_[ ( c_ * getHeight() ) + y_ ] & ( 0x080 >> x_ ) ) > 0 );
    }
    else
    {
      return (
        ( pFontData_[ ( c_ * getHeight() ) + ( y_ * getBytesPerLine() ) + ( x_ >> 3 ) ] & 
        ( 0x080 >> ( x_ % 8 ) ) ) > 0
      );
    }
  }
};

//--------------------------------------------------------------------------------------------------

template<class TFontClass>
class FontBase : public Font
{
  
public:

  static TFontClass* get()
  {
    static TFontClass m_font;
    return &m_font;
  }
  
};
  
//--------------------------------------------------------------------------------------------------

} // cabl
} // sl
