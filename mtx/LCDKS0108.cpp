#include <avr/pgmspace.h>

#include "Arduino.h"

#ifndef _BV
#define _BV(x) (1 << (x))
#endif

#include <stdlib.h>

#include "GFX.h"
#include "LCDKS0108.h"

LCDKS0108::LCDKS0108(int8_t QRS, int8_t QEN, int8_t QCS1, int8_t QCS2, int8_t latchpin) : GFX(LCDWIDTH, LCDHEIGHT)
{
  _qrs = QRS;
  _qen = QEN;
  _qcs1 = QCS1;
  _qcs2 = QCS2;

  _latchPin = latchpin;
}

// the most basic function, set a single pixel
void LCDKS0108::drawPixel(int16_t x, int16_t y, uint16_t color)
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
  } 
  */

  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
    return;

  if (color)
    dispBuffer[x + (y / 8) * LCDWIDTH] |= _BV(y % 8);
  else
    dispBuffer[x + (y / 8) * LCDWIDTH] &= ~_BV(y % 8);
}

// the most basic function, get a single pixel
uint8_t LCDKS0108::getPixel(int16_t x, int16_t y)
{
  if ((x < 0) || (x >= LCDWIDTH) || (y < 0) || (y >= LCDHEIGHT))
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

  // Setup hardware SPI.
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  //SPI.setBitOrder(LSBFIRST);
  //SPI.setClockDivider(SPI_CLOCK_DIV2);

  //initialise lcd
  lcdCommand(DISP_OFF);
  lcdCommand(START_LINE);
  lcdCommand(DISP_ON);
  lcdCommand(SET_ADDRESS);
  lcdCommand(SET_PAGE);
}

void LCDKS0108::setNewPage(unsigned char PageData)
{
  PageData = PageData + SET_PAGE;
  lcdCommand(PageData);
  lcdCommand(SET_ADDRESS);
}

void LCDKS0108::display(void)
{
  uint8_t data;
  uint8_t column = 0, page = 0;

  setNewPage(0);

  for (uint16_t j = 0; j < 1024; j++)
  {
    data = dispBuffer[j];

    if (column == 128)
    {
      column = 0;
      page += 1;
      setNewPage(page);
    }

    if (column <= 63)
    {
      *qcs1port |= qcs1pinmask;  //cs1 high
      *qcs2port &= ~qcs2pinmask; //cs2 low
    }

    if (column >= 64)
    {
      *qcs2port |= qcs2pinmask;  //cs2 high
      *qcs1port &= ~qcs1pinmask; //cs1 low
    }


    *qrsport |= qrspinmask;      //rs high

    *latchPort &= ~latchpinmask; //latch low

    //###
    SPI.transfer(data);

    *latchPort |= latchpinmask; //latch high

    //toggle EN
    *qenport |= qenpinmask;  //EN high
    delayMicroseconds(3);
    *qenport &= ~qenpinmask; //EN low

    column++;
  }
}

void LCDKS0108::clearDisplay(void) //Clear virtual buffer
{
  memset(dispBuffer, 0, LCDWIDTH * LCDHEIGHT / 8);
  cursor_y = cursor_x = 0; //These are in GFX lib
}

inline void LCDKS0108::lcdCommand(uint8_t command)
{
  //enable both controllers
  *qcs1port |= qcs1pinmask;
  *qcs2port |= qcs2pinmask;

  //drive RS to low
  *qrsport &= ~qrspinmask;

  *latchPort &= ~latchpinmask; //latch low

  //send command
  SPI.transfer(command);

  *latchPort |= latchpinmask; //latch high

  //Toggle EN
  *qenport |= qenpinmask; //EN high
  delayMicroseconds(3);
  *qenport &= ~qenpinmask; //EN low
}
