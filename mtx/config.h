#pragma once

//-------------------------------
#define _SKETCHVERSION "2.1.0"

//-------- PINS -----------------

#define PIN_ROW1       2
// #define PIN_ROW2       3
#define PIN_ROW2      12
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

// --------- LCD ----------
#define DISPLAY_KS0108
// #define DISPLAY_CGM12864G

//---------- Battery voltage ----
const int battVoltsMin = 3500; //millivolts
const int battVoltsMax = 4000; //millivolts
const int battVfactor  = 503;  //scaling factor

//-------------------------------
#define UART_BAUD_RATE 115200  //should match secondary mcu baud
