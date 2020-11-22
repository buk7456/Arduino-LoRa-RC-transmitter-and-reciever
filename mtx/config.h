//-------------------------------
#define _SKETCHVERSION "1.6.0"

//-------- PINS -----------------

#define PIN_ROW1       2
#define PIN_ROW2       3
// #define PIN_ROW2      12
#define PIN_COL1 4
#define PIN_COL2 5
#define PIN_COL3 6

#define PIN_LATCH      10

#define PIN_CGM_RST    7
#define PIN_CGM_RD     8
#define PIN_CGM_RSEL   9

#define PIN_KS_RS      8
#define PIN_KS_EN      9
#define PIN_KS_CS1     7
#define PIN_KS_CS2     3

#define PIN_THROTTLE   A0
#define PIN_YAW        A1
#define PIN_PITCH      A2
#define PIN_ROLL       A3
#define PIN_KNOB       A4
#define PIN_BATTVOLTS  A5

//--------- LCD KS0108 ----------
// #include "LCDKS0108.h"
// LCDKS0108 display = LCDKS0108(PIN_KS_RS, PIN_KS_EN, PIN_KS_CS1, PIN_KS_CS2, PIN_LATCH);

//--------- LCD CGM12864G -------
#include "LCDCGM12864G_595.h"
LCDCGM12864G_595 display = LCDCGM12864G_595(PIN_CGM_RSEL, PIN_CGM_RD, PIN_CGM_RST, PIN_LATCH);

//---------- Battery voltage ----
const int battVoltsMin = 3400; //millivolts
const int battVoltsMax = 3900; //millivolts
const int battVfactor  = 487;  //scaling factor

//-------------------------------
#define UART_BAUD_RATE 115200
// #define UART_BAUD_RATE 9600 //Testing 
