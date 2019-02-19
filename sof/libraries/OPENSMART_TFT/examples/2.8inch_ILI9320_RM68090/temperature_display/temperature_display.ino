// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//Demo for TFT LCD Shield 2.8"
//by Open-Smart Team and Catalex Team
//catalex_inc@163.com
//Store:   http://dx.com
//Demo Function: Display temperature value from LM75A sensor.
//Arduino IDE: 1.6.5
// Board: Arduino UNO R3, Arduino Mega2560,Arduino Leonardo

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

  uint16_t identifier = tft.readID();

  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier == 0x4747) {
    Serial.println(F("Found HX8347A LCD driver"));
  } 
    else if(identifier == 0x9326) {
    Serial.println(F("Found ILI9326 LCD driver"));
  }  else if(identifier == 0x9327) {
    Serial.println(F("Found ILI9327 LCD driver"));
  } 
  else if(identifier == 0x9320) {
    Serial.println(F("Found ILI9320 LCD driver"));
  } 
    else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    return;
  }

  tft.begin(identifier);
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

