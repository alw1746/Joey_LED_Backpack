/*************************************************** 
  This is a library for I2C LED Backpacks

  Designed specifically to work with the Joey LED Matrix backpacks 
  
  ----> http://www.gooligum.com.au/

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 3 Address Select pads: 0x70 thru 0x77. Solder bridge the gaps.

  Written by Alex Wong, based on Adafruit LED Backpack Library.  
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>

#include "Joey_LEDBackpack.h"

#ifndef _BV
  #define _BV(bit) (1<<(bit))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

//ASCII table in flash
static const uint16_t alphafonttable[] PROGMEM =  {
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
NOTDEF,
0b0000000000000000, //  SPACE
NOTDEF, // !
NOTDEF, // "
NOTDEF, // #
NOTDEF, // $
NOTDEF, // %
NOTDEF, // &
0b0000000000001000, // '
0b0010010000000000, // (
0b0000100100000000, // )
0b0011111111000000, // *
0b0001001011000000, // +
0b0000100000000000, // ,
0b0000000001000000, // -
0b0000100000000000, // .
0b0000110000000000, // /
0b0011000000100111, // 0
0b0000000000000110, // 1
0b0011000001000011, // 2
0b0010000001000111, // 3
0b0000000001100110, // 4
0b0010000001100101, // 5
0b0011000001100101, // 6
0b0000000000000111, // 7
0b0011000001100111, // 8
0b0000000001100111, // 9
0b0000000000010000, // :
NOTDEF, // ;
NOTDEF, // <
0b0010000001000000, // =
NOTDEF, // >
NOTDEF, // ?
NOTDEF, // @
0b0001000001100111, // A
0b0011000001100100, // B
0b0011000000100001, // C
0b0011000001000110, // D
0b0011000001100001, // E
0b0001000001100001, // F
0b0010000001100111, // G
0b0001000001100110, // H
0b0000000000000110, // I
0b0011000000000110, // J
NOTDEF, // K
0b0011000000100000, // L
NOTDEF, // M
0b0001000001000100, // N
0b0011000000100111, // O
0b0001000001100011, // P
NOTDEF, // Q
0b0001000001000000, // R
0b0010000001100101, // S
0b0001000001100000, // T
0b0011000000100110, // U
NOTDEF, // V
NOTDEF, // W
NOTDEF, // X
0b0010000001100110, // Y
NOTDEF, // Z
0b0011000000100001, // [
0b0000000000000000, // 
0b0010000000000111, // ]
NOTDEF, // ^
0b0010000000000000, // _
0b0000000100000000, // `
0b0001000001100111, // a
0b0011000001100100, // b
0b0011000001000000, // c
0b0011000001000110, // d
0b0011000001100001, // e
0b0001000001100001, // f
0b0010000001100111, // g
0b0001000001100100, // h
0b0001000000000000, // i
0b0011000000000110, // j
NOTDEF, // k
0b0001000000100000, // l
NOTDEF, // m
0b0001000001000100, // n
0b0011000001000100, // o
0b0001000001100011, // p
NOTDEF, // q
0b0001000001000000, // r
0b0010000001100101, // s
0b0001000001100000, // t
0b0011000000000100, // u
NOTDEF, // v
NOTDEF, // w
NOTDEF, // x
0b0010000001100110, // y
NOTDEF, // z
0b0011000000100001, // {
0b0000000000000110, // |
0b0010000000000111, // }
0b0000000000000001, // ~
0b0011111111111111

};

Joey_LEDBackpack::Joey_LEDBackpack(void) {
}

void Joey_LEDBackpack::setBrightness(uint8_t b) {
  if (b > 15) b = 15;
  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_CMD_BRIGHTNESS | b);
  Wire.endTransmission();  
}

//blink display with rate
void Joey_LEDBackpack::blinkRate(uint8_t b) {
  Wire.beginTransmission(i2c_addr);
  if (b > 3) b = 0; // turn off if not sure
  
  Wire.write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON | (b << 1)); 
  Wire.endTransmission();
}

//blank display on or off
void Joey_LEDBackpack::blank(bool state){ 

  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_BLINK_CMD | state); // Blanking / blinking command
  Wire.endTransmission();
}


void Joey_LEDBackpack::begin(uint8_t _addr) {
  i2c_addr = _addr;

  Wire.begin();
  Wire.beginTransmission(i2c_addr);
  Wire.write(0x21);  // turn on oscillator
  Wire.endTransmission();
  blinkRate(HT16K33_BLINK_OFF);
  setBrightness(15); // max brightness
}

void Joey_LEDBackpack::writeDisplay(void) {
  Wire.beginTransmission(i2c_addr);
  Wire.write((uint8_t)0x00); // start at address $00

  for (uint8_t i=0; i<8; i++) {
    Wire.write(displaybuffer[i] & 0xFF);    
    Wire.write(displaybuffer[i] >> 8);    
  }
  Wire.endTransmission();  
}

void Joey_LEDBackpack::clear(void) {
  for (uint8_t i=0; i<8; i++) {
    displaybuffer[i] = 0;
  }
}

/******************************* 7 SEGMENT OBJECT */

Joey_7segment::Joey_7segment(void) {
  position = 0;
}

void Joey_7segment::print(unsigned long n, int base)
{
  if (base == 0) write(n);
  else printNumber(n, base);
}

void Joey_7segment::print(char c, int base)
{
  print((long) c, base);
}

void Joey_7segment::print(unsigned char b, int base)
{
  print((unsigned long) b, base);
}

void Joey_7segment::print(int n, int base)
{
  print((long) n, base);
}

void Joey_7segment::print(unsigned int n, int base)
{
  print((unsigned long) n, base);
}

void  Joey_7segment::println(void) {
  position = 0;
}

void  Joey_7segment::println(char c, int base)
{
  print(c, base);
  println();
}

void  Joey_7segment::println(unsigned char b, int base)
{
  print(b, base);
  println();
}

void  Joey_7segment::println(int n, int base)
{
  print(n, base);
  println();
}

void  Joey_7segment::println(unsigned int n, int base)
{
  print(n, base);
  println();
}

void  Joey_7segment::println(long n, int base)
{
  print(n, base);
  println();
}

void  Joey_7segment::println(unsigned long n, int base)
{
  print(n, base);
  println();
}

void  Joey_7segment::println(double n, int digits)
{
  print(n, digits);
  println();
}

void  Joey_7segment::print(double n, int digits)
{
  printFloat(n, digits);
}

void Joey_7segment::print(long n, int base)
{
  printNumber(n, base);
}

void Joey_7segment::printNumber(long n, uint8_t base)
{
    printFloat(n, 0, base);
}

void Joey_7segment::printFloat(double n, uint8_t fracDigits, uint8_t base) 
{ 
  uint8_t numericDigits = 4;   // available digits on display
  bool isNegative = false;  // true if the number is negative
  
  // is the number negative?
  if(n < 0) {
    isNegative = true;  // need to draw sign later
    --numericDigits;    // the sign will take up one digit
    n *= -1;            // pretend the number is positive
  }
  
  // calculate the factor required to shift all fractional digits
  // into the integer part of the number
  double toIntFactor = 1.0;
  for(int i = 0; i < fracDigits; ++i) toIntFactor *= base;
  
  // create integer containing digits to display by applying
  // shifting factor and rounding adjustment
  uint32_t displayNumber = n * toIntFactor + 0.5;
  
  // calculate upper bound on displayNumber given
  // available digits on display
  uint32_t tooBig = 1;
  for(int i = 0; i < numericDigits; ++i) tooBig *= base;
  
  // if displayNumber is too large, try fewer fractional digits
  while(displayNumber >= tooBig) {
    --fracDigits;
    toIntFactor /= base;
    displayNumber = n * toIntFactor + 0.5;
  }
  
  // did toIntFactor shift the decimal off the display?
  if (toIntFactor < 1) {
    printError();
  } else {
    // otherwise, display the number
    int8_t displayPos = 4;
    
    if (displayNumber)  //if displayNumber is not 0
    {
      for(uint8_t i = 0; displayNumber || i <= fracDigits; ++i) {
        bool displayDecimal = (fracDigits != 0 && i == fracDigits);
        writeDigitNum(displayPos--, displayNumber % base, displayDecimal);
        if(displayPos == 2) writeDigitRaw(displayPos--, 0x00);
        displayNumber /= base;
      }
    }
    else {
      writeDigitNum(displayPos--, 0, false);
    }
  
    // display negative sign if negative
    if(isNegative) writeDigitRaw(displayPos--, 0x40);
  
    // clear remaining display positions
    while(displayPos >= 0) writeDigitRaw(displayPos--, 0x00);
  }
}

void Joey_7segment::printError(void) {
//error condition, display '-' on all LEDs.
  writeDigitAscii(0,'-');
  writeDigitAscii(1,'-');
  writeDigitRaw(4,0);       //clr degree dot
  writeDigitAscii(5,'-');
  writeDigitRaw(6,0);       //clr colon
  writeDigitAscii(7,'-');
}

size_t Joey_7segment::write(uint8_t c) {

  uint8_t r = 0;

  if (c == '\n') position = 0;
  if (c == '\r') position = 0;

  if ((c >= '0') && (c <= '9')) {
    writeDigitNum(position, c-'0');
    r = 1;
  }

  position++;
  if (position == 2) position++;

  return r;
}

//display raw bitmask
void Joey_7segment::writeDigitRaw(uint8_t d, uint8_t bitmask) {
  if (d > 7) return;
  displaybuffer[d] = bitmask;
}

//display ASCII char
void Joey_7segment::writeDigitAscii(uint8_t n, uint8_t a) {
  if (n > 7) return;
  displaybuffer[n] = pgm_read_word(alphafonttable+a);

}

//display binary number as ASCII with/out decimal.
void Joey_7segment::writeDigitNum(uint8_t d, uint8_t num, bool dot) {
  if (d > 7) return;
  uint16_t numfont = pgm_read_word(alphafonttable+'0'+num);
  if (dot)
    writeDigitRaw(d, numfont | 1 << 11);
  else
    writeDigitRaw(d, numfont);
}

//display colon :
void Joey_7segment::drawColon(bool state) {
  if (state)
    displaybuffer[6] |= 1 << 4;
  else
    displaybuffer[6] &= ~(1 << 4);
}

//display degree symbol
void Joey_7segment::drawDegree(bool state) {
  if (state)
    displaybuffer[4] |= 1 << 3;
  else
    displaybuffer[4] &= ~(1 << 3);
}

//display decimal point .
void Joey_7segment::drawDP(uint8_t digit,bool state) {
  if (digit > 7)
    return;
  if (state)
    displaybuffer[digit] |= 1 << 11;
  else
    displaybuffer[digit] &= ~(1 << 11);
}

//return jumper states of scan row in first 3 bits.
uint16_t Joey_7segment::getKeys(uint8_t row) {
  uint8_t byteval[2];
  uint8_t numbytes;

  numbytes=2;
  Wire.beginTransmission(i2c_addr);
  Wire.write(HT16K33_ADDRESS_KEY_DATA + row*2); 
  Wire.endTransmission(); 
  Wire.requestFrom(i2c_addr,numbytes);
  int count=Wire.available();
  if (count == 2) {
    byteval[0] = Wire.read();
    byteval[1] = Wire.read();    //1st 3 bits = JP3,2,1 states
  }
  return byteval[1];
}

uint16_t Joey_7segment::getJumpers() {
    //getKeys(0);
    //delay(25);   //switch debounce
    return getKeys(0);    //Joey's jumpers are on scan row 0 of chip.
}

//returns 1 if JPx is bridged else 0 where x=1,2 or 3.
uint8_t Joey_7segment::JP_closed(uint8_t exponent,uint16_t keybuf) {
  if (exponent > 0 && exponent < 4)
    return (keybuf & (uint8_t)pow(2,exponent-1));
  else
    return 0;   //reject other JPx.
}
