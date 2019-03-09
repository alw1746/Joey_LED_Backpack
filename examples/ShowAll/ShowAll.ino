/*
 * Light up every segment on the Joey board in a loop and show corresponding pin-to-segment mappings in the serial monitor.
 *  Watch the LED segments and record the corresponding bit positions displayed on the monitor window.
   
   bit positions(0-15) of a displayBuffer element corresponds to segments of an led.
      0
   +----+
 5 |  6 | 1
   +----+
12 |    | 2
   +----+   o 11
     13

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
// Light up all segments on 4 LEDs.
uint16_t bitval;

  for(int i = 0; i < 8; i++){
    for(int k = 0; k < 16; k++){
      bitval = 1 << k;
      displayBuffer[i] = bitval;
      show();
      Serial.print("LED bitpos bitval = "); Serial.print(i);Serial.print(" ");Serial.print(k);Serial.print(" ");Serial.println(bitval);
      delay(500);
    }
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
    delay(25);
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
