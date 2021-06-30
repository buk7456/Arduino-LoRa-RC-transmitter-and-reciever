
#ifndef _QQKS0108_H
#define _QQKS0108_H

#include "Arduino.h"

typedef volatile uint8_t PortReg;
typedef uint8_t PortMask;

#include <SPI.h>

#define BLACK 1
#define WHITE 0

#define LCDWIDTH 128
#define LCDHEIGHT 64

#define START_LINE 0b11000000
#define SET_ADDRESS 0b01000000
#define SET_PAGE 0b10111000
#define DISP_ON 0b00111111
#define DISP_OFF 0b00111110

class LCDKS0108 : public GFX
{
  public:
    LCDKS0108(int8_t QRS, int8_t QEN, int8_t QCS1, int8_t QCS2, int8_t latchpin);

    void begin();
    void clearDisplay(void);
    void display();

    void drawPixel(uint8_t x, uint8_t y, uint8_t color);
    uint8_t getPixel(uint8_t x, uint8_t y);

  private:
    int8_t _qrs, _qen, _qcs1, _qcs2;
    int8_t _latchPin;

    // The memory buffer for holding the data to be sent to the LCD
    uint8_t dispBuffer[LCDWIDTH * LCDHEIGHT / 8];

    volatile PortReg *qrsport, *qenport, *qcs1port, *qcs2port, *latchPort;
    PortMask qrspinmask, qenpinmask, qcs1pinmask, qcs2pinmask, latchpinmask;

    void lcdCommand(uint8_t command);
    void setNewPage(unsigned char PageData);
};

#endif
