# Arduino-Joey
Arduino support for Joey 4-digit LED display from Gooligum Electronics.

The Joey board from [Gooligum Electronics](http://www.gooligum.com.au) is a nifty 4-digit LED display that is designed to press-fit onto
the first 26 pins of a Raspberry Pi. The holes on the PCB are slightly offset from a standard 2.54mm grid so that the pin-to-hole friction maintains the contact. See this 
[Kickstarter page](https://www.kickstarter.com/projects/gooligumelec/joey-a-sidecar-led-display-for-raspberry-pi) for background information.

![RPi Joey](images/RPi-Joey.jpg)

Python code support is available from their [github site](https://github.com/gooligumelec/Joey-support-Python-code) and I have added some C code here for Arduino. For this project, an ESP-32 was connected to Joey but the code should work on any microcontroller supported by the Arduino framework eg ATmega328, STM32, etc.

![ESP32 Joey](images/ESP32-Joey.png)</center>

## Wiring diagram
  ![ESP32-Joey wiring](images/Wiring.png)

### Pin Connections
|Joey Pin|Name |
|-------|-----|
|1      |3.3V |
|2      |5V   |
|3      |SDA  |
|4      |     |
|5      |SCL  |
|6      |GND  |

## Software
Standalone test programs are found in the Examples directory. I will create a proper class library in future.

1. **[RandomNumDisplay.ino](https://github.com/alw1746/Adafruit_ILI9486_STM32/blob/master/examples/graphicstest/graphicstest.ino)** - Display 4 random digits with some fancy effects.  
[![RandomNumDisplay output](images/grtestvid.png)](https://www.youtube.com/watch?v=hBzeoJun87o&t=2s)

2. **[ShowAll.ino](https://github.com/alw1746/Adafruit_ILI9486_STM32/blob/master/examples/graphicstest/graphicstest.ino)** - Light up every segment on the board in a loop and show corresponding pin-to-segment mappings in the serial monitor. Very useful debug tool.  
[![ShowAll output](images/grtestvid.png)](https://www.youtube.com/watch?v=hBzeoJun87o&t=2s)
