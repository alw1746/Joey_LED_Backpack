/*************************************************** 
 Read the state of the jumpers (JP1-JP3) and light up a dot. When a jumper is inserted into a header, a decimal dot will light up.
 When the jumper is removed, the dot will disappear. This is equivalent to detecting the action of a switch wired across the header.

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
  uint16_t keybuf=joey.getJumpers();     //read jumper states
  
  if (joey.JP_closed(1,keybuf))
    joey.drawDP(7);
  else
    joey.drawDP(7,false);
  
  if (joey.JP_closed(2,keybuf))
    joey.drawDP(5);
  else
    joey.drawDP(5,false);
  
  if (joey.JP_closed(3,keybuf))
    joey.drawDP(1);
  else
    joey.drawDP(1,false);
  joey.writeDisplay();
  delay(100);
}
