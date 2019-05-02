/*************************************************** 
 Light up every segment on the board in a loop and show corresponding pin-to-segment mappings in the serial monitor.
 To discover the mappings, watch both the LED segment and corresponding bitpos on the serial monitor output.
 
 Written by Alex Wong.  
 MIT license, all text above must be included in any redistribution
****************************************************/

#include <Joey_LEDBackpack.h>

Joey_7segment joey = Joey_7segment();
const uint8_t addr = 0x70;                // HT16K33 address

void setup() {
  Serial.begin(115200);
  joey.begin(addr);
  joey.setBrightness(15);
  joey.blinkRate(0);
}

void loop() {
// Light up all segments on 4 LEDs.
uint16_t bitval;

  for(int i = 0; i < 8; i++){
    for(int k = 0; k < 16; k++){
      bitval = 1 << k;
      joey.displaybuffer[i] = bitval;
      joey.writeDisplay();
      Serial.print("LED bitpos bitval = "); Serial.print(i);Serial.print(" ");Serial.print(k);Serial.print(" ");Serial.println(bitval);
      delay(500);
    }
  }
}
