/*
        ##########    Copyright (C) 2015 Vincenzo Pacella
        ##      ##    Distributed under MIT license, see file LICENSE
        ##      ##    or <http://opensource.org/licenses/MIT>
        ##      ##
##########      ############################################################# shaduzlabs.com #####*/

#pragma once

#ifndef SL_FONT_SMALL_H
#define SL_FONT_SMALL_H

#include <gfx/Font.h>

namespace sl
{
namespace cabl
{

class FontSmall : public FontBase<FontSmall> // CRTP
{

public:
  
  inline uint8_t getWidth() const noexcept        { return 3; };
  
  inline uint8_t getHeight() const noexcept       { return 5; };

  inline uint8_t getCharSpacing() const noexcept  { return 1 + getWidth(); };
  
  inline uint8_t getFirstChar() const noexcept    { return 32; };
  
  inline uint8_t getLastChar() const noexcept     { return 126; };
  
  inline uint8_t getBytesPerLine() const noexcept { return 1; };

  inline bool    getPixel( uint8_t c_, uint8_t x_, uint8_t y_ ) const noexcept
  {
    static uint8_t fontData[] =
    {
#include "data/FONT_small.h"
    };
    
    return getPixelImpl( fontData, c_, x_, y_ );
  }

};
  
//--------------------------------------------------------------------------------------------------

} // cabl
} // sl

#endif // SL_FONT_SMALL_H
