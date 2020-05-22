/*------------------------------------------------------------------------
   Stripped down version of CGM12864G lcd driver paired with Adafruit GFX lib
   Using 595 shift register and hardware SPI

   by BUK, buk7456@gmail.com
   Originally written in July 2017

   This lcd has a resolution of 128x64 pixels and was used in Huawei cdma
   phones.

  ===============SHIFT REGISTER 74HC595 TO ARDUINO, WITH SPI HARDWARE==============

   [SR Pin 14 (DS)]    to Arduino pin - (mosi)
   [SR Pin 12 (ST_CP)] to Arduino pin - [latchpin]
   [SR Pin 11 (SH_CP)] to Arduino pin - (sclk)

   NB: Note with hardware SPI MISO and SS pins aren't used but will still be read
   and written to during SPI transfer.  Be careful sharing these pins!

  ------------------Other CGM12864G LCD conections--------------------------
   RS  to Arduino pin - [QRS]
   R/W to GND
   RD  to Arduino pin - [QRD]
   CS  to GND
   RST to Arduino pin - [QRST]
   ----------------------------------------------------------------------------*/

#ifndef _CGM12864G_595_H
#define _CGM12864G_595_H

#include "Arduino.h"

#include <SPI.h>

typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 128
#define LCDHEIGHT 64

class LCDCGM12864G_595 : public GFX
{
  public:
    LCDCGM12864G_595(int8_t QRS, int8_t QRD, int8_t QRST, int8_t latchpin);

    void begin();
    void clearDisplay(void);
    void display();

    void drawPixel(int16_t x, int16_t y, uint16_t color);
    uint8_t getPixel(int16_t x, int16_t y);

  private:
    int8_t _qrs, _qrd, _qrst, _latchPin;
    volatile PortReg *qrsport, *qrdport, *dataPort, *latchPort, *clockPort;

    // The memory buffer for holding the data to be sent to the LCD
    uint8_t dispBuffer[LCDWIDTH * LCDHEIGHT / 8];

    PortMask qrspinmask, qrdpinmask, latchpinmask;

    void setColumn(uint8_t column);
    void lcdDataWrite(uint8_t data);
    void lcdCommand(uint8_t command);
};

#endif
