/*
  Display various patterns on the 4-digit LEDs depending on jumper settings.
  JP1 & JP3   ASCII table
  JP1         Random numbers
  JP2         Temperature
  JP3         PIC-CLUB
  <open>      snake
  Looks up an ASCII table defined in flash memory for the bit=to-segment map of a character. 
  Character not displayable on a 7 segment LED is represented by '.' (dot).
  
   Bit positions(0-15) of a displayBuffer element corresponds to segments of an led.
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
displayBuffer[4] = 1 << 3;  (degree dot)
displayBuffer[6] = 1 << 4;  (colon)
show();       //send displayBuffer to Joey

*/

#include <Wire.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

//Blink rate
#define  __HT16K33_BLINKRATE_OFF     0x00
#define  __HT16K33_BLINKRATE_2HZ     0x01
#define  __HT16K33_BLINKRATE_1HZ     0x02
#define  __HT16K33_BLINKRATE_HALFHZ  0x03
#define  __HT16K33_ADDRESS_KEY_DATA  0x40
#define SEVENSEG_DIGITS 5
#define toprseg 0b0000000000000010
#define toplseg 0b0000000000100000
#define botrseg 0b0000000000000100
#define botlseg 0b0001000000000000
#define NOTDEF  0b0000100000000000      //undefined char = '.'
//default paramter value declaration.
void setColon(bool state=true);
void setDegrees(bool state=true);
void setDP(uint8_t digit,bool state=true);
void writeDigitNum(uint8_t d, uint8_t num, bool dot=false);

#define ONE_WIRE_BUS 19
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

const uint8_t addr = 0x70; // HT16K33 default address
const int potPin  = A0;
uint8_t ledarray[]={1,5,7,0};
uint8_t randnum,bufptr;
uint16_t displayBuffer[8];

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


void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(addr);
  Wire.write(0x20 | 1); // turn on oscillator
  Wire.endTransmission();
  setBrightness(15);
  blink(0);
  randomSeed(analogRead(potPin));
}

void loop() {
  int intemp;
  char outstr[8];
  
  uint16_t keybuf=getJumpers();
  if (JP1_closed(keybuf) && JP3_closed(keybuf))   //display ASCII table
  {
    int bufptr=0;
    for (int i='0'; i <= '~'; i++) {
        uint16_t font = pgm_read_word(alphafonttable+i);
        displayBuffer[ledarray[bufptr]] = font;
        show();
        delay(1000);
        bufptr++;
        if (bufptr > 3) {
          bufptr=0;
          clear();
        }
    }
    clear();
  }
  else if (JP1_closed(keybuf))        //random numbers
  {
    for (int i=0; i < 4; i++) {
      randnum=(int)random(0,9999)/1000;
      //Serial.println(randnum);
      writeDigitFx(ledarray[i],randnum);
    }
    blink(__HT16K33_BLINKRATE_2HZ);
    for (int i=15; i > -1; i--) {
      setBrightness(i);
      delay(250);
    }
    blink(__HT16K33_BLINKRATE_OFF);
    blank();
    delay(1000);
    blank();
    for (int i=0; i < 16; i++) {
      setBrightness(i);
      delay(250);
    }
    clearFx();
    delay(500);
  }

  else if (JP2_closed(keybuf))      //temperature
  {
    sensors.requestTemperatures(); // Send the command to get temperature readings 
    float temp=sensors.getTempCByIndex(0);
    int intemp=(int)temp;
    if (intemp > 99 || intemp < -9)
      itoa(intemp, outstr, 10);
    else
        dtostrf(temp,4, 1, outstr);
    setDegrees(true);
    writeDigitAscii(0,'C');
    bufptr=0;
    for (int i=0; i < 4; i++) {
      if (outstr[i]=='\0')
        break;
      else if (outstr[i]=='.')
         setDP(ledarray[bufptr-1]);
      else {
        writeDigitAscii(ledarray[bufptr],outstr[i]);
        bufptr++;
      }
    }
    show();
    delay(1000);
  }
  
  else if (JP3_closed(keybuf))      //PIC-CLUB
  {
    writeDigitAscii(1,'P');
    writeDigitAscii(5,'I');
    writeDigitAscii(7,'C');
    writeDigitAscii(0,'-');
    show();
    delay(1000);
    clear();
    writeDigitAscii(1,'C');
    writeDigitAscii(5,'L');
    writeDigitAscii(7,'U');
    writeDigitAscii(0,'B');
    show();
    delay(1000);
    clear();
  }
  else {        //snake
    setDegrees(false);
    int pausetime=100;
    for (int i=0; i < 4; i++) {
      writeDigitAscii(ledarray[i],'~');
      show();
      delay(pausetime);
      displayBuffer[ledarray[i]]=0;
      show();
    }
    writeDigitRaw(0,toprseg);
    show();
    delay(pausetime);
    displayBuffer[0]=0;
    show();
    
    for (int i=3; i > -1; i--) {
      writeDigitAscii(ledarray[i],'-');
      show();
      delay(pausetime);
      displayBuffer[ledarray[i]]=0;
      show();
    }
    writeDigitRaw(1,botlseg);
    show();
    delay(pausetime);
    displayBuffer[1]=0;
    show();

    for (int i=0; i < 4; i++) {
      writeDigitAscii(ledarray[i],'_');
      show();
      delay(pausetime);
      displayBuffer[ledarray[i]]=0;
      show();
    }
    writeDigitRaw(0,botrseg);
    show();
    delay(pausetime);
    displayBuffer[0]=0;
    show();

    for (int i=3; i > -1; i--) {
      writeDigitAscii(ledarray[i],'-');
      show();
      delay(pausetime);
      displayBuffer[ledarray[i]]=0;
      show();
    }
    writeDigitRaw(1,toplseg);
    show();
    delay(pausetime);
    displayBuffer[1]=0;
    show();
  }
}

void show(){
  Wire.beginTransmission(addr);
  Wire.write(0x00); // start at address 0x0

  for (int i = 0; i < 8; i++) {
    Wire.write(displayBuffer[i] & 0xFF);    
    Wire.write(displayBuffer[i] >> 8);    
  }
  Wire.endTransmission();  
}

void clear(){
  for(int i = 0; i < 8; i++){
    displayBuffer[i] = 0;
  }
}

void clearFx(){
  byte bufmap[]={1,5,7,0};
  for(int i = 3; i > -1; i--){
    displayBuffer[bufmap[i]] = 0;
    show();
    segmentOn(bufmap[i],0);
    segmentOn(bufmap[i],5);
    segmentOn(bufmap[i],12);
    segmentOn(bufmap[i],13);
    segmentOn(bufmap[i],2);
    segmentOn(bufmap[i],1);
    displayBuffer[bufmap[i]] = 0;
    show();
  }
}

void setBrightness(uint8_t b){
  if(b > 15) return;

  Wire.beginTransmission(addr);
  Wire.write(0xE0 | b); // Dimming command
  Wire.endTransmission();
}

void blank(){
  static boolean blankOn;  

  Wire.beginTransmission(addr);
  Wire.write(0x80 | blankOn); // Blanking / blinking command
  Wire.endTransmission();

  blankOn = !blankOn;
}

void blink(uint8_t b){
  if(b > 3) return;

  Wire.beginTransmission(addr);
  Wire.write(0x80 | b << 1 | 1); // Blinking / blanking command
  Wire.endTransmission();
}

void writeDigitAscii(uint8_t n, uint8_t a) {
  if (n > 7) return;
  uint16_t font = pgm_read_word(alphafonttable+a);
  displayBuffer[n] = font;
}

void writeDigitRaw(uint8_t d, uint16_t bitmask) {
  if (d > 7) return;
  displayBuffer[d] = bitmask;
}

void writeDigitNum(uint8_t d, uint8_t num, boolean dot) {
  if (d > 7) return;
  uint16_t numfont = pgm_read_word(alphafonttable+'0'+num);
  if (dot)
    writeDigitRaw(d, numfont | 1 << 11);
  else
    writeDigitRaw(d, numfont);
}

void setColon(bool state) {
  if (state)
    displayBuffer[6] |= 1 << 4;
  else
    displayBuffer[6] &= ~(1 << 4);
}

void setDegrees(bool state) {
  if (state)
    displayBuffer[4] |= 1 << 3;
  else
    displayBuffer[4] &= ~(1 << 3);
}

void setDP(uint8_t digit,bool state) {
  if (digit > 7)
    return;
  if (state)
    displayBuffer[digit] |= 1 << 11;
  else
    displayBuffer[digit] &= ~(1 << 11);
}

uint16_t getKeys(uint8_t row) {
  uint8_t byteval[2];
  uint8_t numbytes;

  numbytes=2;
  Wire.beginTransmission(addr);
  Wire.write(__HT16K33_ADDRESS_KEY_DATA + row*2); 
  Wire.endTransmission(); 
  Wire.requestFrom(addr,numbytes);
  int count=Wire.available();
  if (count == 2) {
    byteval[0] = Wire.read();
    byteval[1] = Wire.read();    //1st 3 bits = JP3,2,1 states
  }
  return byteval[1];
}

uint16_t getJumpers() {
    getKeys(0);
    delay(25);   //switch debounce
    return getKeys(0);
}

uint8_t JP1_closed(uint16_t keybuf) {
  return (keybuf & 0b001);
}

uint8_t JP2_closed(uint16_t keybuf) {
  return (keybuf & 0b010);
}

uint8_t JP3_closed(uint16_t keybuf) {
  return (keybuf & 0b100);
}

void segmentOn(byte i,uint16_t bitpos) {
  displayBuffer[i] |= 1 << bitpos;
  show();
  delay(80);
}

void writeDigitFx(byte i,uint8_t chr) {
  segmentOn(i,0);
  segmentOn(i,1);
  segmentOn(i,2);
  segmentOn(i,13);
  segmentOn(i,12);
  segmentOn(i,5);
  writeDigitNum(i,chr);
  show();
}

void printFloat(double n, uint8_t fracDigits, uint8_t base) 
{ 
  uint8_t numericDigits = 4;   // available digits on display
  boolean isNegative = false;  // true if the number is negative
  
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
        boolean displayDecimal = (fracDigits != 0 && i == fracDigits);
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

void printError(void) {
  for(uint8_t i = 0; i < SEVENSEG_DIGITS; ++i) {
    writeDigitRaw(i, (i == 2 ? 0x00 : 0x40));
  }
}
