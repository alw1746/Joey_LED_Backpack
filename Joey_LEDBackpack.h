/*************************************************** 
  This is a library for I2C LED Backpacks

  Designed specifically to work with the Joey 7 segment LED display backpack.
  
  ----> http://www.gooligum.com.au/

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 3 Address Select pads: 0x70 thru 0x77. Solder bridge the gaps.

  Written by Alex Wong, based on Adafruit LED Backpack Library.  
  MIT license, all text above must be included in any redistribution
 ****************************************************

  Note: bit positions(0-15) of a displayBuffer element corresponds to segments of an led.

      0
   +----+
 5 |  6 | 1
   +----+
12 |    | 2
   +----+   o 11
     13

displayBuffer elements [ 1 | 5 | 7 | 0 ] corresponds to 4 physical leds (left->right).
displayBuffer elements [ 4 | 6 ] corresponds to degree dot and colon.

Examples:

displayBuffer[0] = 1<<0 | 1<<5 | 1<<11 | 1<<12 | 1<<13 | 1<<6 | 1<<1 | 1<<2;    //digit 8. segments, rightmost LED
displayBuffer[1] = 0b0011100001100111;   //digit 8. bitmask, leftmost LED
displayBuffer[4] = 1 << 3;  //degree dot
displayBuffer[6] = 1 << 4;  //colon
show();       //send displayBuffer to Joey

****************************************************/
#ifndef Joey_LEDBackpack_h   //prevent multiple .h includes
#define Joey_LEDBackpack_h

#if (ARDUINO >= 100)
 #include "Arduino.h"       //use this for Arduino IDE >= 1.0
#else
 #include "WProgram.h"      //otherwise use this
#endif


#include <Wire.h>           //I2C library

#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BLINK_OFF 0
#define HT16K33_BLINK_2HZ  1
#define HT16K33_BLINK_1HZ  2
#define HT16K33_BLINK_HALFHZ  3
#define HT16K33_ADDRESS_KEY_DATA  0x40
#define HT16K33_CMD_BRIGHTNESS 0xE0
#define NOTDEF  0b0000100000000000      //undefined char = '.'
#define DEC 10
#define HEX 16
#define OCT 8
#define BIN 2
#define BYTE 0


//Base class: this is the raw HT16K33 controller
class Joey_LEDBackpack {
 public:
  Joey_LEDBackpack(void);              //constructor
  void begin(uint8_t _addr=0x70);      //default i2c address
  void setBrightness(uint8_t b);       //brightness
  void blinkRate(uint8_t b);           //blink display with rate
  void blank(bool state=true);         //blank display
  void writeDisplay(void);             //send display buffer to chip
  void clear(void);                    //init display

  uint16_t displaybuffer[8];           //public display buffer
 protected:
  uint8_t i2c_addr;
};

//Derived class: functions to handle 7 segment LEDs. inherit all the accessible members of base.
class Joey_7segment : public Joey_LEDBackpack {
 public:
  Joey_7segment(void);   //constructor

  void print(char, int = BYTE);
  void print(unsigned char, int = BYTE);
  void print(int, int = DEC);
  void print(unsigned int, int = DEC);
  void print(long, int = DEC);
  void print(unsigned long, int = DEC);
  void print(double, int = 2);
  void println(char, int = BYTE);
  void println(unsigned char, int = BYTE);
  void println(int, int = DEC);
  void println(unsigned int, int = DEC);
  void println(long, int = DEC);
  void println(unsigned long, int = DEC);
  void println(double, int = 2);
  void println(void);
  void printNumber(long, uint8_t = 2);
  void printFloat(double, uint8_t = 2, uint8_t = DEC);
  void printError(void);

  size_t write(uint8_t c);
  void writeDigitRaw(uint8_t x, uint8_t bitmask);
  void writeDigitAscii(uint8_t n, uint8_t a);
  void writeDigitNum(uint8_t d, uint8_t num, bool dot=false);
  void drawColon(bool state=true);
  void drawDegree(bool state=true);
  void drawDP(uint8_t digit,bool state=true);
  
  uint16_t getKeys(uint8_t row);
  uint16_t getJumpers();
  uint8_t JP_closed(uint8_t exponent,uint16_t keybuf);

  
 private:
  uint8_t position;
};
#endif // Joey_LEDBackpack_h

