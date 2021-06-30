/*
  Adapted by BUK, from Adafruit GFX lib.
  
  This library has been tailored to this project so this means some features have been removed or 
  modified for lower flash usage, reduced overhead, and to speed up drawing operations.
  Changes made:
    - Reimplemented drawHLine, drawVLine, drawRect, fillRect
    - Removed unused features like Bresenham's line algorithm, circles, triangles, round rectangles,
      fonts, etc.
    - Using uint8_t types

  Copyright (c) 2013 Adafruit Industries.  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

#include "GFX.h"
#include <avr/pgmspace.h>


GFX::GFX(uint8_t w, uint8_t h) : WIDTH(w), HEIGHT(h)
{
  _width = WIDTH;
  _height = HEIGHT;
  rotation = 0;
  cursor_y = cursor_x = 0;
  textcolor = 0xFF;
  wrap = true;
}

void GFX::drawHLine(uint8_t x, uint8_t y, uint8_t w, uint8_t color)
{
  for (uint8_t i = 0; i < w; i++)
    drawPixel(x + i, y, color);
}


void GFX::drawVLine(uint8_t x, uint8_t y, uint8_t h, uint8_t color)
{
  for (uint8_t i = 0; i < h; i++)
    drawPixel(x, y+i, color);
}


void GFX::drawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
  drawHLine(x, y, w, color);
  drawHLine(x, y+h-1, w, color);
  drawVLine(x, y, h, color);
  drawVLine(x+w-1, y, h, color);
}


void GFX::fillRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color)
{
  for (uint8_t i = 0; i < w; i++)
  {
    for (uint8_t j = 0; j < h; j++)
      drawPixel(x + i, y + j, color);
  }
}


// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void GFX::drawBitmap(uint8_t x, uint8_t y, const uint8_t bitmap[], uint8_t w, uint8_t h, uint8_t color)
{
  uint8_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  for (uint8_t j = 0; j < h; j++, y++)
  {
    for (uint8_t i = 0; i < w; i++)
    {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
      if (byte & 0x80)
        drawPixel(x + i, y, color);
    }
  }
}

// Draw a character
void GFX::drawChar(uint8_t x, uint8_t y, unsigned char c, uint8_t color)
{
  if (c >= 176)
    c++; // Handle 'classic' charset behavior

  for (int8_t i = 0; i < 5; i++)
  { // Char bitmap = 5 columns
    uint8_t line = pgm_read_byte(&font[c * 5 + i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1)
    {
      if (line & 1)
        drawPixel(x + i, y + j, color);
    }
  }
}

size_t GFX::write(uint8_t c)
{
  if (c == '\n')
  { // Newline?
    cursor_x  = 0;  // Reset x to zero,
    cursor_y += 8;  // advance y one line
  }
  else if (c != '\r')
  { // Ignore carriage returns
    if (wrap && ((cursor_x + 6) > _width))
    { // Off right?
      cursor_x  = 0; // Reset x to zero,
      cursor_y += 8; // advance y one line
    }
    drawChar(cursor_x, cursor_y, c, textcolor);
    cursor_x += 6;   // Advance x one char
  }
  return 1;
}

void GFX::setCursor(uint8_t x, uint8_t y)
{
  cursor_x = x;
  cursor_y = y;
}

uint8_t GFX::getCursorX(void) const
{
  return cursor_x;
}

uint8_t GFX::getCursorY(void) const
{
  return cursor_y;
}

void GFX::setTextColor(uint8_t c)
{
  textcolor = c;
}

void GFX::setTextWrap(boolean w)
{
  wrap = w;
}

// Return the size of the display (per current rotation)
uint8_t GFX::width(void) const
{
  return _width;
}

uint8_t GFX::height(void) const
{
  return _height;
}

