
//To update default values in EEPROM change the value below to anything between 0xAA and 0xAF.
#define EE_INITFLAG 0xAB  //WARNING: Changing this will overwrite all your existing model data

//---------------------UART baud rate----------------------------------------------------
// #define UARTBAUDRATE 57600
#define UARTBAUDRATE 115200

//------------------- Only enable one lcd ------------------------------------------------

//---- LCD KS0108
// #include "LCDKS0108.h"
// LCDKS0108 display = LCDKS0108(8, 9, 7, 3, 10); //RS, EN, CS1, CS2, 595Latch


//---- LCD CGM12864G
#include "LCDCGM12864G_595.h"
LCDCGM12864G_595 display = LCDCGM12864G_595(9, 8, 7, 10);

//--------------- Buttons ----------------------------------------------------------------
#define ROW1_MTRX_PIN 2
#define ROW2_MTRX_PIN 3
// #define ROW2_MTRX_PIN 12
#define COL1_MTRX_PIN 4
#define COL2_MTRX_PIN 5
#define COL3_MTRX_PIN 6

#define LONGPRESSTIME 350

//---------------- Analog input pins -----------------------------------------------------
#define BATTVOLTSPIN  A5
#define AUX2PIN       A4
#define ROLLINPIN     A3
#define PITCHINPIN    A2
#define YAWINPIN      A1
#define THROTTLEINPIN A0

//---------------- Battery ---------------------------------------------------------------
#define BATTVFACTOR 487  //determined experimentally. Whole numbers only
#define BATTV_MIN   3400 //in millivolts. Whole numbers only
#define BATTV_MAX   3850 //in millivolts Whole numbers only

//----------------------------------------------------------------------------------------
#define _SKETCHVERSION "1.2.0"
