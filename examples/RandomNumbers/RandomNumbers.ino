/*************************************************** 
  Display various patterns on the 4-digit LEDs depending on jumper settings.
  JP1 & JP3   ASCII table
  JP1         Random numbers
  JP2         Temperature
  JP3         PIC-CLUB
  <open>      snake
  Looks up an ASCII table defined in flash memory for the bit=to-segment map of a character. 
  Character not displayable on a 7 segment LED is represented by '.' (dot).
  
 Written by Alex Wong.  
 MIT license, all text above must be included in any redistribution
****************************************************/

#include <Joey_LEDBackpack.h>
#include <OneWire.h> 
#include <DallasTemperature.h>

Joey_7segment joey = Joey_7segment();
const uint8_t addr = 0x70;                // HT16K33 address

#define toprseg 0b0000000000000010
#define toplseg 0b0000000000100000
#define botrseg 0b0000000000000100
#define botlseg 0b0001000000000000

#define ONE_WIRE_BUS 19
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);

const int potPin  = A0;
uint8_t ledarray[]={1,5,7,0};
uint8_t randnum,bufptr;

void setup() {
  Serial.begin(115200);
  joey.begin(addr);
  joey.setBrightness(15);
  joey.blinkRate(0);
  randomSeed(analogRead(potPin));
}

void loop() {
  int intemp;
  char outstr[8];
  
  uint16_t keybuf=joey.getJumpers();
  if (joey.JP_closed(1,keybuf) && joey.JP_closed(3,keybuf))   //display ASCII table
  {
    int bufptr=0;
    for (int i='0'; i <= '~'; i++) {
      joey.writeDigitAscii(ledarray[bufptr],i);
      joey.writeDisplay();
      delay(1000);
      bufptr++;
      if (bufptr > 3) {
        bufptr=0;
        joey.clear();
      }
    }
    joey.clear();
  }
  else if (joey.JP_closed(1,keybuf))        //random numbers
  {
    for (int i=0; i < 4; i++) {
      randnum=(int)random(0,9999)/1000;
      //Serial.println(randnum);
      writeDigitFx(ledarray[i],randnum);
    }
    joey.blinkRate(HT16K33_BLINK_2HZ);
    for (int i=15; i > -1; i--) {
      joey.setBrightness(i);
      delay(250);
    }
    joey.blinkRate(HT16K33_BLINK_OFF);
    joey.blank();
    delay(1000);
    joey.blank();
    for (int i=0; i < 16; i++) {
      joey.setBrightness(i);
      delay(250);
    }
    clearFx();
    delay(500);
  }

  else if (joey.JP_closed(2,keybuf))      //temperature
  {
    sensors.requestTemperatures(); // Send the command to get temperature readings 
    float temp=sensors.getTempCByIndex(0);
    int intemp=(int)temp;
    if (intemp > 99 || intemp < -9)
      itoa(intemp, outstr, 10);
    else
        dtostrf(temp,4, 1, outstr);
    joey.drawDegree(true);
    joey.writeDigitAscii(0,'C');
    bufptr=0;
    for (int i=0; i < 4; i++) {
      if (outstr[i]=='\0')
        break;
      else if (outstr[i]=='.')
         joey.drawDP(ledarray[bufptr-1]);
      else {
        joey.writeDigitAscii(ledarray[bufptr],outstr[i]);
        bufptr++;
      }
    }
    joey.writeDisplay();
    delay(1000);
  }
  
  else if (joey.JP_closed(3,keybuf))      //PIC-CLUB
  {
    joey.writeDigitAscii(1,'P');
    joey.writeDigitAscii(5,'I');
    joey.writeDigitAscii(7,'C');
    joey.writeDigitAscii(0,'-');
    joey.writeDisplay();
    delay(1000);
    joey.clear();
    joey.writeDigitAscii(1,'C');
    joey.writeDigitAscii(5,'L');
    joey.writeDigitAscii(7,'U');
    joey.writeDigitAscii(0,'B');
    joey.writeDisplay();
    delay(1000);
    joey.clear();
  }
  else {                                 //snake
    joey.drawDegree(false);
    int pausetime=100;
    for (int i=0; i < 4; i++) {
      joey.writeDigitAscii(ledarray[i],'~');
      joey.writeDisplay();
      delay(pausetime);
      joey.displaybuffer[ledarray[i]]=0;
      joey.writeDisplay();
    }
    joey.writeDigitRaw(0,toprseg);
    joey.writeDisplay();
    delay(pausetime);
    joey.displaybuffer[0]=0;
    joey.writeDisplay();
    
    for (int i=3; i > -1; i--) {
      joey.writeDigitAscii(ledarray[i],'-');
      joey.writeDisplay();
      delay(pausetime);
      joey.displaybuffer[ledarray[i]]=0;
      joey.writeDisplay();
    }
    joey.writeDigitRaw(1,botlseg);
    joey.writeDisplay();
    delay(pausetime);
    joey.displaybuffer[1]=0;
    joey.writeDisplay();

    for (int i=0; i < 4; i++) {
      joey.writeDigitAscii(ledarray[i],'_');
      joey.writeDisplay();
      delay(pausetime);
      joey.displaybuffer[ledarray[i]]=0;
      joey.writeDisplay();
    }
    joey.writeDigitRaw(0,botrseg);
    joey.writeDisplay();
    delay(pausetime);
    joey.displaybuffer[0]=0;
    joey.writeDisplay();

    for (int i=3; i > -1; i--) {
      joey.writeDigitAscii(ledarray[i],'-');
      joey.writeDisplay();
      delay(pausetime);
      joey.displaybuffer[ledarray[i]]=0;
      joey.writeDisplay();
    }
    joey.writeDigitRaw(1,toplseg);
    joey.writeDisplay();
    delay(pausetime);
    joey.displaybuffer[1]=0;
    joey.writeDisplay();
  }
}

void clearFx(){
  byte bufmap[]={1,5,7,0};
  for(int i = 3; i > -1; i--){
    joey.displaybuffer[bufmap[i]] = 0;
    joey.writeDisplay();
    segmentOn(bufmap[i],0);
    segmentOn(bufmap[i],5);
    segmentOn(bufmap[i],12);
    segmentOn(bufmap[i],13);
    segmentOn(bufmap[i],2);
    segmentOn(bufmap[i],1);
    joey.displaybuffer[bufmap[i]] = 0;
    joey.writeDisplay();
  }
}

void segmentOn(byte i,uint16_t bitpos) {
  joey.displaybuffer[i] |= 1 << bitpos;
  joey.writeDisplay();
  delay(80);
}

void writeDigitFx(byte i,uint8_t chr) {
  segmentOn(i,0);
  segmentOn(i,1);
  segmentOn(i,2);
  segmentOn(i,13);
  segmentOn(i,12);
  segmentOn(i,5);
  joey.writeDigitNum(i,chr);
  joey.writeDisplay();
}
