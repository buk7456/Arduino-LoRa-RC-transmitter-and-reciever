/*
  Adapted by BUK, from Adafruit GFX lib.

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

// Many (but maybe not all) non-AVR board installs define macros
// for compatibility with existing PROGMEM-reading AVR code.
// Do our own checks and defines here for good measure...

#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef pgm_read_word
#define pgm_read_word(addr) (*(const unsigned short *)(addr))
#endif
#ifndef pgm_read_dword
#define pgm_read_dword(addr) (*(const unsigned long *)(addr))
#endif

// Pointers are a peculiar case...typically 16-bit on AVR boards,
// 32 bits elsewhere.  Try to accommodate both...

#if !defined(__INT_MAX__) || (__INT_MAX__ > 0xFFFF)
#define pgm_read_pointer(addr) ((void *)pgm_read_dword(addr))
#else
#define pgm_read_pointer(addr) ((void *)pgm_read_word(addr))
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) \
  {                       \
    int16_t t = a;      \
    a = b;              \
    b = t;              \
  }
#endif

GFX::GFX(int16_t w, int16_t h) : WIDTH(w), HEIGHT(h)
{
  _width = WIDTH;
  _height = HEIGHT;
  rotation = 0;
  cursor_y = cursor_x = 0;
  textsize = 1;
  textcolor = textbgcolor = 0xFFFF;
  wrap = true;
}

// Bresenham's algorithm - thx wikpedia
void GFX::writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  int16_t steep = abs(y1 - y0) > abs(x1 - x0);
  if (steep)
  {
    _swap_int16_t(x0, y0);
    _swap_int16_t(x1, y1);
  }

  if (x0 > x1)
  {
    _swap_int16_t(x0, x1);
    _swap_int16_t(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if (y0 < y1)
  {
    ystep = 1;
  }
  else
  {
    ystep = -1;
  }

  for (; x0 <= x1; x0++)
  {
    if (steep)
    {
      writePixel(y0, x0, color);
    }
    else
    {
      writePixel(x0, y0, color);
    }
    err -= dy;
    if (err < 0)
    {
      y0 += ystep;
      err += dx;
    }
  }
}


void GFX::writePixel(int16_t x, int16_t y, uint16_t color)
{
  drawPixel(x, y, color);
}

// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void GFX::writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  drawFastVLine(x, y, h, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call writeLine() instead
void GFX::writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  drawFastHLine(x, y, w, color);
}

void GFX::writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  // Overwrite in subclasses if desired!
  fillRect(x, y, w, h, color);
  
}


// (x,y) is topmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void GFX::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
  writeLine(x, y, x, y + h - 1, color);
}

// (x,y) is leftmost point; if unsure, calling function
// should sort endpoints or call drawLine() instead
void GFX::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
  writeLine(x, y, x + w - 1, y, color);
}


void GFX::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  for (int16_t i = 0; i < w; i++)
  {
    for (int16_t j = 0; j < h; j++)
      drawPixel(x + i, y + j, color);
  }
}

void GFX::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
  // Update in subclasses if desired!
  if (x0 == x1)
  {
    if (y0 > y1)
      _swap_int16_t(y0, y1);
    drawFastVLine(x0, y0, y1 - y0 + 1, color);
  }
  else if (y0 == y1)
  {
    if (x0 > x1)
      _swap_int16_t(x0, x1);
    drawFastHLine(x0, y0, x1 - x0 + 1, color);
  }
  else
  {
    writeLine(x0, y0, x1, y1, color);
  }
}

// Draw a rectangle
void GFX::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  writeFastHLine(x, y, w, color);
  writeFastHLine(x, y + h - 1, w, color);
  writeFastVLine(x, y, h, color);
  writeFastVLine(x + w - 1, y, h, color);
}


// Draw a PROGMEM-resident 1-bit image at the specified (x,y) position,
// using the specified foreground color (unset bits are transparent).
void GFX::drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w, int16_t h, uint16_t color)
{
  int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
  uint8_t byte = 0;

  for (int16_t j = 0; j < h; j++, y++)
  {
    for (int16_t i = 0; i < w; i++)
    {
      if (i & 7)
        byte <<= 1;
      else
        byte = pgm_read_byte(&bitmap[j * byteWidth + i / 8]);
      if (byte & 0x80)
        writePixel(x + i, y, color);
    }
  }
}

// Draw a character
void GFX::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
  if ((x >= _width) ||            // Clip right
      (y >= _height) ||           // Clip bottom
      ((x + 6 * size - 1) < 0) || // Clip left
      ((y + 8 * size - 1) < 0))   // Clip top
    return;

  if (c >= 176)
    c++; // Handle 'classic' charset behavior

  for (int8_t i = 0; i < 5; i++)
  { // Char bitmap = 5 columns
    uint8_t line = pgm_read_byte(&font[c * 5 + i]);
    for (int8_t j = 0; j < 8; j++, line >>= 1)
    {
      if (line & 1)
      {
        if (size == 1)
          writePixel(x + i, y + j, color);
        else
          writeFillRect(x + i * size, y + j * size, size, size, color);
      }
      else if (bg != color)
      {
        if (size == 1)
          writePixel(x + i, y + j, bg);
        else
          writeFillRect(x + i * size, y + j * size, size, size, bg);
      }
    }
  }
  if (bg != color)
  { // If opaque, draw vertical line for last column
    if (size == 1)
      writeFastVLine(x + 5, y, 8, bg);
    else
      writeFillRect(x + 5 * size, y, size, 8 * size, bg);
  }
}

size_t GFX::write(uint8_t c)
{
  if (c == '\n')
  { // Newline?
    cursor_x  = 0;                     // Reset x to zero,
    cursor_y += textsize * 8;          // advance y one line
  }
  else if (c != '\r')
  { // Ignore carriage returns
    if (wrap && ((cursor_x + textsize * 6) > _width))
    { // Off right?
      cursor_x  = 0;                 // Reset x to zero,
      cursor_y += textsize * 8;      // advance y one line
    }
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize * 6;          // Advance x one char
  }
  return 1;
}

void GFX::setCursor(int16_t x, int16_t y)
{
  cursor_x = x;
  cursor_y = y;
}

int16_t GFX::getCursorX(void) const
{
  return cursor_x;
}

int16_t GFX::getCursorY(void) const
{
  return cursor_y;
}

void GFX::setTextSize(uint8_t s)
{
  textsize = (s > 0) ? s : 1;
}

void GFX::setTextColor(uint16_t c)
{
  // For 'transparent' background, we'll set the bg
  // to the same as fg instead of using a flag
  textcolor = textbgcolor = c;
}

void GFX::setTextColor(uint16_t c, uint16_t b)
{
  textcolor = c;
  textbgcolor = b;
}

void GFX::setTextWrap(boolean w)
{
  wrap = w;
}

// Return the size of the display (per current rotation)
int16_t GFX::width(void) const
{
  return _width;
}

int16_t GFX::height(void) const
{
  return _height;
}

