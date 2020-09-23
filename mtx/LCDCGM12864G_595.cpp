#include "Arduino.h"

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

#include <stdlib.h>

#include "GFX.h"
#include "LCDCGM12864G_595.h"

LCDCGM12864G_595::LCDCGM12864G_595(int8_t QRS, int8_t QRD, int8_t QRST, int8_t latchpin) : GFX(LCDWIDTH, LCDHEIGHT)
{
  _qrs = QRS;
  _qrd = QRD;
  _qrst = QRST;
  _latchPin = latchpin;
}

// the most basic function, set a single pixel
void LCDCGM12864G_595::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  /*  
  if ((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
    return;

  int16_t t;
  switch (rotation)
  {
    case 1:
      t = x;
      x = y;
      y = LCDHEIGHT - 1 - t;
      break;
    case 2:
      x = LCDWIDTH - 1 - x;
      y = LCDHEIGHT - 1 - y;
      break;
    case 3:
      t = x;
      x = LCDWIDTH - 1 - y;
      y = t;
      break;
  } */

  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return;

  if (color)
    dispBuffer[x + (y / 8) * LCDWIDTH] |= _BV(y % 8);
  else
    dispBuffer[x + (y / 8) * LCDWIDTH] &= ~_BV(y % 8);
}

// the most basic function, get a single pixel
uint8_t LCDCGM12864G_595::getPixel(int16_t x, int16_t y)
{
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return 0;

  return (dispBuffer[x + (y / 8) * LCDWIDTH] >> (y % 8)) & 0x1;
}

void LCDCGM12864G_595::begin()
{
  pinMode(_qrs, OUTPUT);
  pinMode(_qrd, OUTPUT);
  pinMode(_qrst, OUTPUT);
  pinMode(_latchPin, OUTPUT);

  //Set ports and masks
  qrsport = portOutputRegister(digitalPinToPort(_qrs));
  qrspinmask = digitalPinToBitMask(_qrs);
  qrdport = portOutputRegister(digitalPinToPort(_qrd));
  qrdpinmask = digitalPinToBitMask(_qrd);
  latchPort = portOutputRegister(digitalPinToPort(_latchPin));
  latchpinmask = digitalPinToBitMask(_latchPin);
 
  digitalWrite(_qrst, LOW);
  delay(30);
  digitalWrite(_qrst, HIGH);
  delay(20);
  
  //Send init commands
  SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
  lcdCommand(0xaf);
  lcdCommand(0xc8);
  lcdCommand(0x2f);
  lcdCommand(0x26);
  lcdCommand(0x81);
  lcdCommand(0x1f);
  SPI.endTransaction();
}

void LCDCGM12864G_595::display(void)
{
  
  uint8_t col;
  uint8_t page;
  uint8_t theByte;

  SPI.beginTransaction(SPISettings(8000000, LSBFIRST, SPI_MODE0));
  for (page = 0xb0; page < 0xb8; page++)
  {
    lcdCommand(page);
    lcdCommand(0x10);
    lcdCommand(0x00);

    setColumn(4); //not sure why we do this. Couldnt find a datasheet for lcd

    for (col = 0; col < LCDWIDTH; col++)
    {
      theByte = dispBuffer[((page & 0x07) * LCDWIDTH + col)];
      lcdDataWrite(theByte);
    }
  }
  SPI.endTransaction();
}

void LCDCGM12864G_595::clearDisplay(void) //Clear virtual buffer
{
  memset(dispBuffer, 0, LCDWIDTH * LCDHEIGHT / 8);
  cursor_y = cursor_x = 0; //These are in GFX lib
}


inline void LCDCGM12864G_595::lcdCommand(uint8_t command)
{
  *qrsport &= ~qrspinmask;     //qrs low
  *qrdport |= qrdpinmask;      //qrd high
  *latchPort &= ~latchpinmask; //latch low

  // Hardware SPI write.
  SPI.transfer(command);

  *latchPort |= latchpinmask; //latch high
  *qrdport &= ~qrdpinmask;    //qrd low
}

inline void LCDCGM12864G_595::lcdDataWrite(uint8_t data)
{
  *qrsport |= qrspinmask;      //qrs high
  *qrdport |= qrdpinmask;      //qrd high
  *latchPort &= ~latchpinmask; //latch low

  // Hardware SPI write.
  SPI.transfer(data);

  *latchPort |= latchpinmask; //latch high
  *qrdport &= ~qrdpinmask;    //qrd low
}

inline void LCDCGM12864G_595::setColumn(uint8_t column)
{
  uint8_t temp;
  temp = column;
  column = column & 0x0f;
  column = column | 0x00;
  lcdCommand(column);
  temp = temp >> 4;
  column = temp & 0x0f;
  column = column | 0x10;
  lcdCommand(column);
}
