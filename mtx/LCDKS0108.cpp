
#include "Arduino.h"
#include <SPI.h>
#include "GFX.h"
#include "LCDKS0108.h"

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

LCDKS0108::LCDKS0108(int8_t QRS, int8_t QEN, int8_t QCS1, int8_t QCS2, int8_t latchpin) : GFX(LCDWIDTH, LCDHEIGHT)
{
  _qrs = QRS;
  _qen = QEN;
  _qcs1 = QCS1;
  _qcs2 = QCS2;

  _latchPin = latchpin;
}

// the most basic function, set a single pixel
void LCDKS0108::drawPixel(uint8_t x, uint8_t y, uint8_t color)
{
  //Rotation code removed here

  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return;

  if (color)
    dispBuffer[x + (y / 8) * LCDWIDTH] |= _BV(y % 8);
  else
    dispBuffer[x + (y / 8) * LCDWIDTH] &= ~_BV(y % 8);
}

// the most basic function, get a single pixel
uint8_t LCDKS0108::getPixel(uint8_t x, uint8_t y)
{
  if ((x >= LCDWIDTH) || (y >= LCDHEIGHT))
    return 0;

  return (dispBuffer[x + (y / 8) * LCDWIDTH] >> (y % 8)) & 0x1;
}

void LCDKS0108::begin()
{
  pinMode(_qrs, OUTPUT);
  pinMode(_qen, OUTPUT);
  pinMode(_qcs1, OUTPUT);
  pinMode(_qcs2, OUTPUT);
  pinMode(_latchPin, OUTPUT);

  //Set ports and masks
  qrsport = portOutputRegister(digitalPinToPort(_qrs));
  qrspinmask = digitalPinToBitMask(_qrs);

  qenport = portOutputRegister(digitalPinToPort(_qen));
  qenpinmask = digitalPinToBitMask(_qen);

  qcs1port = portOutputRegister(digitalPinToPort(_qcs1));
  qcs1pinmask = digitalPinToBitMask(_qcs1);

  qcs2port = portOutputRegister(digitalPinToPort(_qcs2));
  qcs2pinmask = digitalPinToBitMask(_qcs2);

  latchPort = portOutputRegister(digitalPinToPort(_latchPin));
  latchpinmask = digitalPinToBitMask(_latchPin);

  delay(50);

  //initialise lcd
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  lcdCommand(DISP_ON);
  lcdCommand(START_LINE);
  SPI.endTransaction();
}

void LCDKS0108::setPage(uint8_t pageNo)
{
  lcdCommand(SET_PAGE | pageNo);
  lcdCommand(SET_Y_ADDRESS);
}

void LCDKS0108::display(void)
{
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  uint16_t dataIdx = 0;
  for(uint8_t page = 0; page < 8; page++)
  {
    setPage(page);

    bool isCS2 = false;
    //enable chip1
#if defined (CS_ACTIVE_LOW)
    *qcs1port &= ~qcs1pinmask;  
    *qcs2port |= qcs2pinmask;
#else
    *qcs1port |= qcs1pinmask;  
    *qcs2port &= ~qcs2pinmask; 
#endif 

    for(uint8_t column = 0; column < 128; column++)
    {
      if(!isCS2 && column >= 64) 
      {
        isCS2 = true;
        //enable chip2
#if defined (CS_ACTIVE_LOW)
        *qcs2port &= ~qcs2pinmask;
        *qcs1port |= qcs1pinmask;
#else 
        *qcs2port |= qcs2pinmask; 
        *qcs1port &= ~qcs1pinmask;
#endif
      }
      
      *qrsport |= qrspinmask; //rs high
      
      *latchPort &= ~latchpinmask; //latch low
      SPI.transfer(dispBuffer[dataIdx++]);
      *latchPort |= latchpinmask; //latch high
      
      //toggle EN
      delayMicroseconds(3);
      *qenport |= qenpinmask;  //EN high
      delayMicroseconds(3);
      *qenport &= ~qenpinmask; //EN low
    }
  }

  SPI.endTransaction();
}

void LCDKS0108::clearDisplay(void) //Clear virtual buffer
{
  memset(dispBuffer, 0, LCDWIDTH * LCDHEIGHT / 8);
  cursor_y = cursor_x = 0; //These are in GFX lib
}

inline void LCDKS0108::lcdCommand(uint8_t command)
{
  //enable both controllers
#if defined (CS_ACTIVE_LOW)
  *qcs1port &= ~qcs1pinmask;
  *qcs2port &= ~qcs2pinmask;
#else
  *qcs1port |= qcs1pinmask;
  *qcs2port |= qcs2pinmask;
#endif

  *qrsport &= ~qrspinmask; //rs low

  *latchPort &= ~latchpinmask; //latch low
  SPI.transfer(command);
  *latchPort |= latchpinmask; //latch high

  //Toggle EN
  delayMicroseconds(3);
  *qenport |= qenpinmask; //EN high
  delayMicroseconds(3);
  *qenport &= ~qenpinmask; //EN low
}
