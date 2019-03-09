/*
   Read the state of the jumpers (JP1-JP3) and light up a dot.The jumpers are the functional equivalent of a DIP switch.
   When a jumper is inserted into a header, a decimal dot will light up. When the jumper is removed, the dot will disappear. 
   This is similar to detecting the action of a switch wired across the header.

   bit positions(0-15) of a displayBuffer element corresponds to segments of an led.
      0
   +----+
 5 |  6 | 1
   +----+
12 |    | 2
   +----+   o 11
     13

Eg:
1 << 0 = top seg
1 << 1 = top right seg
1 << 5 = top left seg
1 << 11 = decimal dot
1 << 13 = bot seg

displayBuffer elements [ 1 | 5 | 7 | 0 ] corresponds to 4 physical leds (left->right).
displayBuffer elements [ 4 ] - degree dot
displayBuffer elements [ 6 ] - colon
Eg:
displayBuffer[0] = 1<<0 | 1<<5 | 1<<11 | 1<<12 | 1<<13 | 1<<6 | 1<<1 | 1<<2;    //digit 8. in segments, rightmost LED
displayBuffer[1] = 0b0011100001100111;   //digit 8. in binary, leftmost LED
displayBuffer[4] = 1 << 3;  (degree dot)
displayBuffer[6] = 1 << 4;  (colon)

 */
#include <Wire.h>

#define  __HT16K33_ADDRESS_KEY_DATA  0x40

void setColon(bool state=true);
void setDegrees(bool state=true);
void setDP(uint8_t digit,bool state=true);

const uint8_t addr = 0x70; // HT16K33 default address
uint16_t displayBuffer[8];

void setup() {
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(addr);
  Wire.write(0x20 | 1); // turn on oscillator
  Wire.endTransmission();
  setBrightness(15);
  blink(0);
}

void loop() {
  uint16_t keybuf=getJumpers();     //read jumper states
  
  if (JP1_closed(keybuf))
    setDP(7);
  else
    setDP(7,false);
  
  if (JP2_closed(keybuf))
    setDP(5);
  else
    setDP(5,false);
  
  if (JP3_closed(keybuf))
    setDP(1);
  else
    setDP(1,false);
  show();
  delay(100);
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

void writeDigitRaw(uint8_t d, uint16_t bitmask) {
  if (d > 7) return;
  displayBuffer[d] = bitmask;
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
  uint16_t pattern;
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
    delay(25);               //switch debounce
    return getKeys(0);
}

uint8_t JP1_closed(uint16_t keybuf) {
  return (keybuf & 0b001);   //bit 0
}

uint8_t JP2_closed(uint16_t keybuf) {
  return (keybuf & 0b010);   //bit 1
}

uint8_t JP3_closed(uint16_t keybuf) {
  return (keybuf & 0b100);   //bit 2
}
