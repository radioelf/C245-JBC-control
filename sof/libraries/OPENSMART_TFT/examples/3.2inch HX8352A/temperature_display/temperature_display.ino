// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//Demo for TFT LCD Shield 2.8"
//by Open-Smart Team and Catalex Team
//catalex_inc@163.com
//Store:   http://dx.com
//Demo Function: Display temperature value from LM75A sensor.
//Arduino IDE: 1.6.5
// Board:OPEN-SMART UNO R3 5V / 3.3V, Arduino UNO R3, Arduino Mega2560
//Store:   http://dx.com
//           https://open-smart.aliexpress.com/store/1199788
//3.2INCH TFT:
// https://www.aliexpress.com/store/product/3-2-TFT-LCD-Display-module-Touch-Screen-Shield-board-onboard-temperature-sensor-w-Touch-Pen/1199788_32755473754.html?spm=2114.12010615.0.0.bXDdc3
//OPEN-SMART UNO R3 5V / 3.3V:
// https://www.aliexpress.com/store/product/OPEN-SMART-5V-3-3V-Compatible-UNO-R3-CH340G-ATMEGA328P-Development-Board-with-USB-Cable-for/1199788_32758607490.html?spm=2114.12010615.0.0.ckMTaN

#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_TFTLCD.h> // Hardware-specific library


#include <Wire.h>
#include <LM75.h>


#include <MCUFRIEND_kbv.h>
//----------------------------------------|
// TFT Breakout  -- Arduino UNO / Mega2560 / OPEN-SMART UNO Black
// GND              -- GND
// 3V3               -- 3.3V
// CS                 -- A3
// RS                 -- A2
// WR                -- A1
// RD                 -- A0
// RST                -- RESET
// LED                -- GND
// DB0                -- 8
// DB1                -- 9
// DB2                -- 10
// DB3                -- 11
// DB4                -- 4
// DB5                -- 13
// DB6                -- 6
// DB7                -- 7

MCUFRIEND_kbv tft;
LM75 sensor;  // initialize an LM75 object





// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF



void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  Serial.println(F("TFT LCD test"));

#ifdef USE_ADAFRUIT_SHIELD_PINOUT
  Serial.println(F("Using Adafruit 3.2\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Adafruit 3.2\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  tft.begin(0x5252);//to enable HX8352A driver code
  tft.fillScreen(BLACK);

}

void loop(void) {
    tft.setRotation(1);
    
	tft.setCursor(0, 100);
	tft.fillRect(0,100, 144, 24, BLACK);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.print(sensor.temp());
  tft.println(" C");
    delay(3000);
}


