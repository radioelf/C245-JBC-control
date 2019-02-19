/*
  pines TFT LCD Touch Screen/OPEN-SMART HX8352A 240x400:
  +5V- 150 mA
  GND-
  A0 - LCD-RD lectura datos
  A1 - LCD-WR escritura datos + pantalla táctil digital Y+ horizontal
  A2 - LCD-RS registro instrucciones/datos + pantalla táctil digital X+ vertical
  A3 - LCD-CS selección lcd
  3  - D3 retro iluminación (no 5-3v3)
  4  - LCD-DB4 datos bit 4
  5  - SD_CS selección tarjeta
  6  - LCD_D6 datos bit 6 + pantalla táctil digital X- horizontal
  7  - LCD_D7 datos bit 7 + pantalla táctil digital Y- vertical
  8  - LCD-D0 datos bit 0
  9  - LCD-D1 datos bit 1
  10 - DB2 datos bit 2
  11 - DB3 datos bit 3
  13 - DB5 datos bit 5
  SDA1 -SDA no 5<->3v3
  SCL1 -SCL no 5->3v3
  conector central SPI para SD + LCD_RST
  VCC->5V
  MOSI
  GND
  LCD_RST (no 5->3v3 MAX809 ok->10Us VCC)
  SCK
  MISO
    Y->
   0.0           Y+                 0.400
    ----------------------------------
  X |                                |
  | |                                |
  V |                                |
    |                                |
  X+|                                |X-
    |                                |
    |                                |
    |                                |
    |                                |
    ----------------------------------
  240.0          Y-               240.400

  tft.setRotation(1)                             orientación pantalla, 1->horizontal, 0-> vertical
  tft.setCursor (x0,y0,)                         posiciona el cursor en las posición X0, Y0
  tft.print("texto")                             escribe el texto indicado
  tft.print(X, DEC)                              escribe el valor de las variable x en decimal
  tft.println                                    escribe el valor de las variable x en decimal + salto linea
  tft.setTextSize(3)                             tamaño del texto a 3,  1 = 5x8, 2 = 10x16 3 = 15x24
  tft.setTextColor(color)                        color del texto
  tft.setTextWrap(true)                          true-> sigue en la siguiente linea
  tft.fillScreen(color)                          borra la pantalla con el color indicado
  tft.fillRect (x0,y0, W6, H4, color)            rellena un recuadro de color del punto x0,y0 a con una anchura de 6 y una altura de 4
  tft.drawPixel(x0,y0, color)                    pinta el pixel indicado
  tft.drawFastHLine(x0,y0, L, color)             dibuja una línea horizontal de la longitud L
  tft.drawFastVLine(x0,y0, H, color)             dibuja una línea vertical de la longitud H
  tft.fillCircle(x, y, radio, color)             dibuja un circulo relleno
  tft.drawCircle(x, y, radio, color)             dibuja el contorno de un circulo
  tft.drawRect(X0, Y0, X1, Y1,color)             dibuja un rectángulo
  tft.width()                                    devuelve la anchura de la pantalla
  tft.height()                                   devuelve la altura de la pantalla
*/
#ifndef TFT_H
#define TFT_H

#include <Adafruit_GFX.h>                                             // Adafruit Core graphics library
#include <MCUFRIEND_kbv.h>                                            // TFTLCD LIBRARY MUST BE SPECIFICALLY FOR OPEN-SMART
#include <TouchScreen.h>                                              // https://github.com/adafruit/Adafruit_TouchScreen

// Colores usados:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define GREY    0xCCCC
#define ORANGE  0xFA60
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESION 100
#define MAXPRESION 1000

#define LCD_Ledk_pin 3

// modo retardo y pines para TouchScreen:
uint8_t YP = A1, XM = A2, YM = 7, XP = 6;

uint8_t ReposoMod = 1, ModoTrab = 0, BuzzerOnOFF = 1;
uint16_t tp_y = 500, tp_x = 500;                                      // memoria puntero

MCUFRIEND_kbv tft;
// la resistencia medida entre X+ y X- es de 304 ohms
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 304);                    // instancia ts para  touchscreen
TSPoint tp;
LM75 sensor;                                                          // el sensor se encuentra en la PCB del LCD

//********************************************************************************************
// Delay 16000 ciclos * 62,5 ns->1ms para oscilador de 16.0 MHz
//--------------------------------------------------------------------------------------------
void DelayMs(uint16_t Ms) {
  do {
    asm volatile (                                                    // 15994 ciclos
      "    ldi  r18, 21"  "\n"                                        // 1 ciclo, carga el registro r18 con el valor 21
      "    ldi  r19, 197" "\n"                                        // 1 ciclo, carga el registro r19 con el valor 197
      "1:  dec  r19"  "\n"                                            // 1 ciclo, decrementamos r19 en 1
      "    brne 1b" "\n"                                              // 2 ciclos, si 1b!=0 salta a 1
      "    dec  r18"  "\n"                                            // 1 ciclo, decrementamos r18 en 1
      "    brne 1b" "\n"                                              // 2 ciclos, si 1b!=0 salta a 1
    );
  } while (--Ms != 0);                                                // +6 ciclos
}
//********************************************************************************************
// Dibuja el mapa de bits de la imagen indicada
//--------------------------------------------------------------------------------------------
void BitMap (uint8_t x) {
  switch (x) {
    case 1:
      tft.drawRGBBitmap(15, 25, lm75, 48, 48);                        // imagen temperatura PCB
      break;
    case 2:
      tft.drawRGBBitmap(110, 180, drec, 64, 64);                      // imagen decremento
      break;
    case 3:
      tft.drawRGBBitmap(230, 180, incr, 64, 64);                      // imagen incremento
      break;
    case 4:
      tft.drawRGBBitmap(330, 10, config, 48, 48);                     // imagen configuración
      break;
    case 5:
      tft.drawRGBBitmap(330, 70, mem, 48, 48);                        // imagen memoria
      tft.drawRGBBitmap(330, 125, mem, 48, 48);                       // imagen memoria
      tft.drawRGBBitmap(330, 180, mem, 48, 48);                       // imagen memoria
      break;
    case 6:
      tft.drawRGBBitmap(10, 90, sleep, 48, 48);                       // imagen reposo
      break;
    case 7:
      tft.drawRGBBitmap(5, 175, on, 64, 64);                          // imagen ON
      break;
    case 8:
      tft.drawRGBBitmap(5, 175, off, 64, 64);                         // imagen OFF
      break;
  }
}
//********************************************************************************************
// Selección botón espera-trabajo ON<-->OFF
//--------------------------------------------------------------------------------------------
void TFTEsperaOnOff() {
  tft.setTextSize(2);
  tft.setCursor(0, 140);
  if (ModoTrab == 1) {                                                // si nos encontramos en modo trabajo
    tft.setTextColor(CYAN);
    tft.print("Espera");
    ModoTrab = 2;                                                     // modo espera
    BitMap(6);                                                        // imagen en reposo
    return;
  }
  if (ModoTrab == 2) {                                                // si nos encontramos en modo espera
    tft.setTextColor(BLACK);
    tft.print("Espera");                                              // borramos texto Espera
    ModoTrab = 1;                                                     // modo trabajo
    tft.fillCircle(33, 114, 30, BLACK);                               // borramos imagen en reposo
    tft.fillCircle(33, 114, 15, GREEN);
    tft.fillCircle(33, 114, 5, RED);
    return;
  }
}
//********************************************************************************************
// Dibujamos STOP para reposo
//--------------------------------------------------------------------------------------------
void TFTreposo() {
  tft.fillCircle(33, 114, 30, WHITE);
  tft.setTextColor(RED);
  tft.setCursor(10, 105);
  tft.print("STOP");
}
//********************************************************************************************
// Gestión de la selección de  los botones de memorias 1-3
//--------------------------------------------------------------------------------------------
void TFTMemoria(uint8_t mem) {
  //TFTEsperaOnOff(0);
  tft.setCursor(100, 20);
  tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
  tft.setTextSize(2);
  tft.print("SET:");
  tft.drawCircle(225, 10, 4, ModoTrab == 1 ? YELLOW : GREEN);
  tft.setTextSize(4);
  tft.setCursor(215, 10);
  tft.print(" C");
  tft.setTextSize(1);
  tft.setTextColor(RED);
  switch (mem) {
    case 1:                                                           // memoria 1 seleccionada
      tft.setCursor(343, 80);
      tft.print("M  1");
      tft.setTextColor(BLACK);
      tft.setCursor(343, 133);
      tft.print("M  2");
      tft.setCursor(343, 190);
      tft.print("M  3");
      tft.drawCircle(353, 96, 23, BLACK);
      tft.drawCircle(353, 96, 26, BLACK);                             // borramos indicación selección memoria 1
      tft.drawCircle(353, 150, 23, BLACK);
      tft.drawCircle(353, 150, 26, BLACK);                            // borramos indicación selección memoria 2
      tft.drawCircle(353, 205, 23, BLACK);
      tft.drawCircle(353, 205, 26, BLACK);                            // borramos indicación selección memoria 3
      tft.drawCircle(353, 152, 23, WHITE);
      tft.drawCircle(353, 152, 26, WHITE);                            // botón m2 NO pulsado
      tft.drawCircle(353, 207, 23, WHITE);
      tft.drawCircle(353, 207, 26, WHITE);                            // botón m3 NO pulsado
      tft.drawCircle(353, 94, 23, RED);
      tft.drawCircle(353, 94, 26, RED);                               // botón m1 SI pulsado
      tft.setTextSize(4);
      tft.setCursor(150, 10);
      tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
      tft.print(ModoTrab == 1 ? Ini_Temp[0] : Temp_Standby[0]);
      BuzzerOnOFF = bitRead(Config, 0);
      SelecMem = 0;
      break;
    case 2:                                                           // memoria 2 seleccionada
      tft.setCursor(343, 133);
      tft.print("M  2");
      tft.setCursor(343, 80);
      tft.setTextColor(BLACK);
      tft.print("M  1");
      tft.setCursor(343, 190);
      tft.print("M  3");
      tft.drawCircle(353, 152, 23, BLACK);
      tft.drawCircle(353, 152, 26, BLACK);                            // borramos indicación selección memoria 2
      tft.drawCircle(353, 94, 23, BLACK);
      tft.drawCircle(353, 94, 26, BLACK);                             // borramos indicación selección memoria 1
      tft.drawCircle(353, 205, 23, BLACK);
      tft.drawCircle(353, 205, 26, BLACK);                            // borramos indicación selección memoria 3
      tft.drawCircle(353, 96, 23, WHITE);
      tft.drawCircle(353, 96, 26, WHITE);                             // botón m1 NO pulsado
      tft.drawCircle(353, 207, 23, WHITE);
      tft.drawCircle(353, 207, 26, WHITE);                            // botón m3 NO pulsado
      tft.drawCircle(353, 150, 23, RED);
      tft.drawCircle(353, 150, 26, RED);                              // botón m2 SI pulsado
      tft.setTextSize(4);
      tft.setCursor(150, 10);
      tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
      tft.print(ModoTrab == 1 ? Ini_Temp[1] : Temp_Standby[1]);
      BuzzerOnOFF = bitRead(Config, 1);
      SelecMem = 1;
      break;
    case 3:                                                           // memoria 3 seleccionada
      tft.setCursor(343, 190);
      tft.print("M  3");
      tft.setCursor(343, 80);
      tft.setTextColor(BLACK);
      tft.print("M  1");
      tft.setCursor(343, 133);
      tft.print("M  2");
      tft.drawCircle(353, 207, 23, BLACK);
      tft.drawCircle(353, 207, 26, BLACK);                            // borramos indicación selección memoria 3
      tft.drawCircle(353, 94, 23, BLACK);
      tft.drawCircle(353, 94, 26, BLACK);                             // borramos indicación selección memoria 1
      tft.drawCircle(353, 150, 23, BLACK);
      tft.drawCircle(353, 150, 26, BLACK);                            // borramos indicación selección memoria 2
      tft.drawCircle(353, 96, 23, WHITE);
      tft.drawCircle(353, 96, 26, WHITE);                             // botón m1 NO pulsado
      tft.drawCircle(353, 152, 23, WHITE);
      tft.drawCircle(353, 152, 26, WHITE);                            // botón m2 NO pulsado
      tft.drawCircle(353, 205, 23, RED);
      tft.drawCircle(353, 205, 26, RED);                              // botón m3 SI pulsado
      tft.setTextSize(4);
      tft.setCursor(150, 10);
      tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
      tft.print(ModoTrab == 1 ? Ini_Temp[2] : Temp_Standby[2]);
      BuzzerOnOFF = bitRead(Config, 2);
      SelecMem = 2;
      break;
  }
}
//********************************************************************************************
// Selección botón marcha-paro control ON<-->OFF
//--------------------------------------------------------------------------------------------
uint8_t TFTOnOff(uint8_t selec) {
  tft.setTextSize(2);
  tft.setCursor(25, 163);
  tft.setTextColor(BLACK);
  if (ModoTrab != 0 || selec == 0) {                                  // si nos encontramos en trabajo o espera
    tft.print("ON");                                                  // borramos texto ON
    ModoTrab = 0;                                                     // pasamos a modo apagado
    BitMap(8);                                                        // dibujamos botón OFF
    tft.fillRect(69, 84, 212, 45, BLACK);                             // borramos temperatura
    tft.fillRect(200, 149, 51, 25, BLACK);                            // borramos %
    tft.setTextColor(WHITE);
    tft.setCursor(200, 150);
    tft.setTextSize(3);
    tft.print("---%");
    tft.setTextColor(GREEN);
    tft.setCursor(25, 163);
    tft.setTextSize(2);
    tft.print("OFF");                                                 // escribimos texto OFF
    tft.fillCircle(33, 114, 30, BLACK);
    tft.setTextColor(BLACK);
    tft.setCursor(0, 140);
    tft.print("Espera");                                              // borramos texto Espera
    return 0;
  } else {                                                            // si no encontramos en reposo
    tft.print("OFF");                                                 // borramos texto OFF
    ReposoMod == 1 ? ModoTrab = 2 : ModoTrab = 1;                     // 2->espera, 1-> trabajo
    BitMap(7);                                                        // dibujamos botón ON
    tft.setTextColor(GREEN);
    tft.setCursor(25, 163);
    tft.setTextSize(2);
    tft.print("ON");                                                  // escribimos texto ON
    tft.setTextColor(WHITE);
    tft.setCursor(200, 150);
    tft.setTextSize(3);
    tft.print("  0%");
    TFTEsperaOnOff();
    return 1;
  }
}
//********************************************************************************************
// Inicializamos pantalla principal en TFT
//--------------------------------------------------------------------------------------------
void IniTFT (uint16_t Set, double Temp, uint8_t Porcentaje) {
  if (tft.readID() == 0x5252) {
    ts = TouchScreen(XP, YP, XM, YM, 320);
    tft.begin(0x5252);                                                // para habilitar el código de controlador HX8352A
#ifdef Debug
    Serial.print(F("Compilacion: "));
    Serial.println(Fecha);
    Serial.println(F(Version));
    Serial.println(F("Driver LCD HX8352A OK"));
    Serial.println("Dimensiones " + String(tft.height()) + "x" + String(tft.width()));
    Serial.println(F("Posicion: HORIZONTAL"));
    Serial.print(F("Temperatura: "));
    Serial.println(Tem_comp);
#endif
    tft.setRotation(1);                                             // horizontal (0- original, 1- rota 90 grados, 2- 180°, 3- 270°)
  } else {
#ifdef Debug
    Serial.println(F("ERROR!! pantalla NO valida!"));
#endif
    while (1) {
      digitalWrite(Buzzer_pin, HIGH);
      DelayMs (500);
      digitalWrite(Buzzer_pin, LOW);
      DelayMs (3000);
    }
  }
  tft.fillScreen(BLACK);                                            // pantalla en negro
  digitalWrite(LCD_Ledk_pin, HIGH);                                 // encendemos pantalla LCD
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.setCursor(70, 69);
  tft.print(Fecha);
  tft.setCursor(70, 85);
  tft.print(Version);

  for (int8_t x = 1; x < 6; x++) {                                  // dibujamos todos los iconos
    BitMap(x);
  }
  tft.setTextColor(BLACK);
  tft.setCursor(70, 69);
  tft.print(Fecha);
  tft.setCursor(70, 85);
  tft.print(Version);
  tft.setCursor(10, 10);                                            // posición cursor
  tft.setTextSize(2);                                               // color y tamaño fuente texto
  tft.setTextColor(WHITE);                                          // actualizamos
  tft.print(Tem_lm75);
  tft.drawCircle(75, 10, 2, WHITE);
  tft.print(" C");
  tft.setTextColor(BLACK);
  tft.setCursor(343, 190);
  TFTMemoria(1);
  tft.setCursor(100, 20);
  tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
  tft.setTextSize(2);
  tft.print("SET:");
  tft.setTextSize(4);
  tft.setCursor(150, 10);
  tft.print(Set);
  tft.drawCircle(225, 10, 4, ModoTrab == 1 ? YELLOW : GREEN);
  tft.setCursor(215, 10);
  tft.print(" C");
  tft.setCursor(140, 55);
  tft.setTextColor(RED);
  tft.setTextSize(3);
  tft.print("ACTUAL");
  tft.setTextSize(6);
  tft.setCursor(70, 85);
  tft.setTextColor(GREY);
  if (Temp < 100) tft.print(" ");
  tft.print(Temp);
  tft.drawCircle(290, 90, 4, GREY);
  tft.setCursor(270, 90);
  tft.setTextSize(5);
  tft.print(" C");
  tft.setCursor(0, 140);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.print("Espera");
  TFTOnOff(1);
  tft.setCursor(25, 163);
  tft.setTextColor(GREEN);
  tft.print("ON");
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(100, 150);
  tft.print("POWER:");
  tft.setCursor(200, 150);
  tft.print("---%");
}
//********************************************************************************************
// Mostramos la temperatura del sensor interno LM75
//--------------------------------------------------------------------------------------------
void TFTLm75(double PCB_Tem) {
  tft.fillRect(9, 9, 60, 15, BLACK);                              // borramos
  tft.setTextSize(2);                                             // color y tamaño fuente texto
  tft.setCursor(10, 10);
  tft.setTextColor(WHITE);                                        // actualizamos
  tft.print(PCB_Tem);
}
//********************************************************************************************
// Comprobamos si se pulso la pantalla
//--------------------------------------------------------------------------------------------
uint8_t TFTpuntero () {
  tp = ts.getPoint();                                             // obtenemos los valores tp.x, tp.p y tp.z
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  pinMode(XP, OUTPUT);
  pinMode(YM, OUTPUT);
  if (tp.z > MINPRESION && tp.z < MAXPRESION) {
#ifdef Debug
    Serial.print(F("X = ")); Serial.print(tp.x);
    Serial.print(F("\tY = ")); Serial.print(tp.y);
    Serial.print(F("\tPresion = ")); Serial.println(tp.z);
#endif
    tp_y = tp.y;
    tp_x = tp.x;
    return 1;
  }
  return 0;
}
//********************************************************************************************
// Comprobamos si la pulsación se realizó en una zona a gestionar (botones)
//--------------------------------------------------------------------------------------------
uint8_t TFTzona (uint8_t pantalla) {
  uint16_t min_x, max_x, min_y, max_y;
#ifdef Debug
  char const *txt = "";
#endif
  if (pantalla) {
    min_x = 248; max_x = 293; min_y = 278; max_y = 315;              // botón pantalla configuración ->1 Tem máxima -
#ifdef Debug
    txt = "T-Max -";
#endif
  } else {
    min_x = 220; max_x = 323; min_y = 184; max_y = 238;             // botón pantalla principal ->0 configuración
#ifdef Debug
    txt = "Config";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 1;
  }
  if (pantalla) {
    min_x = 334; max_x = 373; min_y = 278; max_y = 310;             // botón pantalla configuración ->1 Tem mínima -
#ifdef Debug
    txt = "T-Min -";
#endif
  } else {
    min_x = 400; max_x = 512; min_y = 174; max_y = 259;            // botón pantalla principal->0 M1
#ifdef Debug
    txt = "M-1";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 2;
  }
  if (pantalla) {
    min_x = 421; max_x = 472; min_y = 230; max_y = 311;             // botón pantalla configuración ->1 Tem standby -
#ifdef Debug
    txt = "T-St -";
#endif
  } else {
    min_x = 586; max_x = 685; min_y = 175; max_y = 256;           	// botón pantalla principal->0 M2
#ifdef Debug
    txt = "M-2";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 3;
  }
  if (pantalla) {
    min_x = 506; max_x = 553; min_y = 274; max_y = 300;           	// botón pantalla configuración ->1 Tem RUN -
#ifdef Debug
    txt = "T-Run -";
#endif
  } else {
    min_x = 763; max_x = 864; min_y = 180; max_y = 244;           	// botón pantalla principal->0 M3
#ifdef Debug
    txt = "M-3";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 4;
  }
  if (pantalla) {
    min_x = 601; max_x = 650; min_y = 212; max_y = 229;           	// botón pantalla configuración ->1 ON-OFF buzzer
#ifdef Debug
    txt = "Buzzer";
#endif
  } else {
    min_x = 761; max_x = 892; min_y = 356; max_y = 464;           	// botón pantalla principal->0 incrementar
#ifdef Debug
    txt = "Tem +";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 5;
  }
  if (pantalla) {
    min_x = 746; max_x = 815; min_y = 766; max_y = 929;             // botón pantalla configuración ->1 M1
#ifdef Debug
    txt = "Conf M1";
#endif
  } else {
    min_x = 751; max_x = 887; min_y = 603; max_y = 708;             // botón pantalla principal->0 decrementar
#ifdef Debug
    txt = "Tem -";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {
#ifdef Debug
    Serial.println(txt);
#endif
    return 6;
  }
  if (pantalla) {
    min_x = 743; max_x = 819; min_y = 581; max_y = 740;           	// botón pantalla configuración ->1 M2
#ifdef Debug
    txt = "Conf M2";
#endif
  } else {
    min_x = 755; max_x = 879; min_y = 836; max_y = 918;             // botón pantalla principal->0 ON/OFF
#ifdef Debug
    txt = "ON-OFF";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {// comprobamos
#ifdef Debug
    Serial.println(txt);
#endif
    return 7;
  }
  if (pantalla) {
    min_x = 740; max_x = 816; min_y = 391; max_y = 580;             // botón pantalla configuración ->1 M3
#ifdef Debug
    txt = "Conf M3";
#endif
  } else {
    min_x = 486; max_x = 594; min_y = 854; max_y = 932;             // botón pantalla principal->0 REPOSO
#ifdef Debug
    txt = "Reposo";
#endif
  }
  if ((tp_x > min_x && tp_x < max_x) && (tp_y > min_y && tp_y < max_y)) {// comprobamos
#ifdef Debug
    Serial.println(txt);
#endif
    return 8;
  }
  if (pantalla == 1) {
    if ((tp_x > 745 && tp_x < 807) && (tp_y > 154 && tp_y < 264)) {// botón pantalla configuración ->salir
#ifdef Debug
      Serial.println(F("Salir"));
#endif
      return 9;
    }
    if ((tp_x > 249 && tp_x < 283) && (tp_y > 213 && tp_y < 232)) { // botón pantalla configuración ->1 Tem maxima +
#ifdef Debug
      Serial.println(F("T-Max +"));
#endif
      return 10;
    }
    if ((tp_x > 327 && tp_x < 375) && (tp_y > 213 && tp_y < 232)) { // pantalla configuración ->1 Tem mínima +
#ifdef Debug
      Serial.println(F("T-Min +"));
#endif
      return 11;
    }
    if ((tp_x > 424 && tp_x < 464) && (tp_y > 213 && tp_y < 232)) { // pantalla configuración ->1 Tem standby +
#ifdef Debug
      Serial.println(F("T-St +"));
#endif
      return 12;
    }
    if ((tp_x > 515 && tp_x < 559) && (tp_y > 213 && tp_y < 232)) { // pantalla configuración ->1 Tem RUN +
#ifdef Debug
      Serial.println(F("T-Run +"));
#endif
      return 13;
    }
  }
  return 0;
}
//*************************************************************************************
// Escribimos en pantalla el nuevo valor de consigna
//-------------------------------------------------------------------------------------
void TFTUpdateSet (uint16_t SetBoton) {
  tft.fillRect(149, 9, 70, 35, BLACK);                              // borramos
  tft.setTextColor(ModoTrab == 1 ? YELLOW : GREEN);
  tft.setTextSize(4);
  tft.setCursor(150, 10);
  tft.print(SetBoton);
  tft.drawCircle(225, 10, 4, ModoTrab == 1 ? YELLOW : GREEN);
  tft.setCursor(215, 10);
  tft.print(" C");
#ifdef Debug
  Serial.print(F("Seleccion temperatura a: "));
  Serial.println(SetBoton);
#endif
}
//*************************************************************************************
// Nuevo valor y color según temperatura y consigna
//-------------------------------------------------------------------------------------
void TFTColorTemp(uint16_t TempSet, double TempRun) {
  if (TempRun > Temp_Max[SelecMem]) return;                         // NO mostramos lecturas erróneas
  //int16_t TempDIV = round((double)TempSet - TempRun * 8.5);       // calculamos el color según diferencia temperatura con setpoint
  //TempDIV = TempDIV > 254 ? TempDIV = 254 : TempDIV < 0 ? TempDIV = 0 : TempDIV;
  //uint16_t  RGBcolor = ((TempDIV & 0xF8) << 8) | ((255 - TempDIV & 0xFC) << 3) | (0 >> 3); //40us
  uint16_t RGBcolor = RED;
  if (TempSet <= uint16_t (TempRun + 3) && TempSet >= uint16_t (TempRun - 3)) RGBcolor = GREEN;
  else if (TempSet > uint16_t (TempRun)) RGBcolor = BLUE;
  tft.setTextColor(RGBcolor);
  tft.setTextSize(6);                                               //4us
  tft.fillRect(69, 84, 212, 45, BLACK);                             //24596us  borramos
  tft.setCursor(70, 85);                                            //4-8us
  if (TempRun < 100) tft.print(" ");                                //152us
  if (TempRun < 10) tft.print(" ");                                 //152us
  tft.print(TempRun);                                               //44650us
  tft.drawCircle(290, 90, 4, RGBcolor);                             //3384us
  tft.setCursor(270, 90);                                           //4us
  tft.setTextSize(5);                                               //4us
  tft.print(" C");                                                  //5196us
}
//*************************************************************************************
// Mostramos porcentaje de trabajo en pantalla
//-------------------------------------------------------------------------------------
void TFTPorcentaje(uint8_t valor) {
  tft.setTextSize(3);
  tft.fillRect(200, 149, 51, 25, BLACK);                            // borramos
  tft.setCursor(200, 150);
  tft.setTextColor(WHITE);
  if (valor < 10) tft.print(" ");
  if (valor < 100) tft.print(" ");
  tft.print(valor);
}
//*************************************************************************************
// Mostramos error o cambio punta JBC en pantalla
//-------------------------------------------------------------------------------------
void TFTError(uint8_t TollError) {
  tft.fillRect(69, 84, 256, 55, BLACK);
  tft.setTextSize(5);
#ifdef Debug
  interrupts();
  digitalWrite(Control_pin, LOW);
  Serial.print(F("Temp PCB: "));
  Serial.println(Tem_lm75);
  Serial.print(F("Temp JBC: "));
  Serial.println(InputTemp);
  Serial.print(F("Errror "));
  if (TollError == 200) Serial.println(F("alimentacion AC"));
  if (TollError == 100) Serial.println(F("sin punta soldador"));
  if (TollError == 10) Serial.println(F("temperatura"));
  noInterrupts();
  digitalWrite(Control_pin, LOW);
  TCCR1B &= ~_BV(CS11);                                             // paramos timer1
  TIMSK1 &= ~_BV(OCIE1A); 
#endif
  uint8_t Ciclo = 0;
  uint16_t Col = RED;
  double TempError;
  while (1) {
    tft.setTextColor(Col);
    tft.setCursor(70, 85);
    if (TollError == 100) {
      tft.print("NO TOOL");                                         // cambio punta soldador
      return;
    }
    TollError == 200 ? tft.print("!FUSE!") : tft.print("!ERROR!");
    DelayMs(490);
    Col == RED ? Col = GREY : Col = RED;                            // parpadeo rojo-gris
    if (++Ciclo == 10) {
      digitalWrite(Buzzer_pin, HIGH);
      DelayMs (500);
      digitalWrite(Buzzer_pin, LOW);
      Ciclo = 0;
    }
  }
}
//*************************************************************************************
// Pantalla configuración
//-------------------------------------------------------------------------------------
void TFTConfigJBC() {
  tft.fillScreen(WHITE);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  for (uint8_t y = 10; y < 131; y += 30) {
    if (y < 101) {
      tft.fillRect(370, y, 20, 20, RED);
      tft.drawRect(369, y - 1, 21, 21, BLUE);
      tft.setCursor(376, y + 5);
      tft.print("+");
      tft.fillRect(330, y, 20, 20, BLUE);
      tft.drawRect(329, y - 1, 21, 21, RED);
      tft.setCursor(336, y + 6);
      tft.print("-");
    } else {
      tft.fillRect(370, 130, 20, 20, CYAN);
      tft.drawRect(369, 129, 21, 21, BLACK);
    }
  }
  EepromRx(1);                                                      // siempre se accede a la configuración leyendo la memoria 1
  uint16_t Tmax = Temp_Max[0];
  uint16_t  Tmin = uint16_t (Temp_Min[0]);
  uint16_t Tstan = Temp_Standby[0];
  uint16_t Trun = Ini_Temp[0];
  EepromRx(0);
  uint8_t bit_buzzer = bitRead(Config, 0);
  tft.setTextSize(2);
  tft.setCursor(20, 10);
  tft.print("Temperatura maxima:  ");
  tft.print(Tmax);
  tft.setCursor(20, 40);
  tft.print("Temperatura minima:  ");
  tft.print(Tmin);
  tft.setCursor(20, 70);
  tft.print("Temperatura standby: ");
  tft.print(Tstan);
  tft.setCursor(20, 100);
  tft.print("Temperatura RUN:     ");
  tft.print(Trun);
  tft.setCursor(20, 130);
  tft.print("Zumbador ON/OFF: ");
  tft.setTextColor(RED);
  tft.setCursor(223, 129);
  bit_buzzer == 1 ? tft.print("[ON]") : tft.print("[OFF]");
  tft.fillRect(330, 180, 65, 30, YELLOW);
  tft.setCursor(335, 187);
  tft.print("SALIR");
  tft.fillRect(10, 180, 85, 30, GREEN);
  tft.setCursor(15, 187);
  tft.print("Mem. 1");
  tft.fillRect(100, 180, 85, 30, GREY);
  tft.setCursor(105, 187);
  tft.setTextColor(BLACK);
  tft.print("Mem. 2");
  tft.fillRect(190, 180, 85, 30, GREY);
  tft.setCursor(195, 187);
  tft.print("Mem. 3");
  uint8_t Memo = 1, Camb_Tmax = 0, Camb_Tmin = 0,
          Camb_Tstan = 0, Camb_Trun = 0, Camb_buzzer = 0, menu = 1;
  while (menu) {
    DelayMs(75);
    if (TFTpuntero() == 1) {
      uint8_t Selec = TFTzona(1);
      switch (Selec) {                                                 // obtenemos el botón pulsado
        case 1: case 10:                                               // botón incrementar y decrementar
          tft.fillRect(272, 9, 55, 18, WHITE);
          tft.setCursor(273, 10);
          Selec == 1 ? Tmax-- : Tmax++;
          if (Tmax < Tmin + 50 || Tmax > 459) {                        // limites temperatura máxima
            Selec == 1 ? Tmax++ : Tmax--;
          }
          tft.print(Tmax);
          Camb_Tmax = 1;
          break;
        case 2: case 11:
          tft.fillRect(272, 39, 55, 18, WHITE);
          tft.setCursor(273, 40);
          Selec == 2 ? Tmin-- : Tmin++;
          if (Tmin < 75 || Tmin > Tmax - 50) {                         // limites temperatura mínima
            Selec == 2 ? Tmin++ : Tmin--;
          }
          tft.print(Tmin);
          Camb_Tmin = 1;
          break;
        case 3: case 12:
          tft.fillRect(272, 69, 55, 18, WHITE);
          tft.setCursor(273, 70);
          Selec == 3 ? Tstan-- : Tstan++;
          if (Tstan < Tmin + 10 || Tstan > Tmax - 50) {                 // limites temperatura Standby
            Selec == 3 ? Tstan++ : Tstan--;
          }
          tft.print(Tstan);
          Camb_Tstan = 1;
          break;
        case 4: case 13:
          tft.fillRect(272, 99, 55, 18, WHITE);
          tft.setCursor(273, 100);
          Selec == 4 ? Trun-- : Trun++;
          if (Trun < Tmin + 10 || Trun > Tmax - 10) {                   // limites temperatura run
            Selec == 4 ? Trun++ : Trun--;
          }
          tft.print(Trun);
          Camb_Trun = 1;
          break;
        case 5:
          bit_buzzer = !bit_buzzer;
          tft.fillRect(225, 128, 60, 18, WHITE);
          tft.setTextColor(RED);
          tft.setCursor(223, 129);
          bit_buzzer == 1 ? tft.print("[ON]") : tft.print("[OFF]");
          tft.setTextColor(BLACK);
          DelayMs (500);
          Camb_buzzer = 1;
          break;
        case 6: case 7: case 8: case 9:                                   // si seleccionamos una memoria o salimos
          // guardamos los valores cambiados de la memoria anterior
          if (Camb_buzzer) {
            bitWrite(Config, Memo - 1, bit_buzzer);
            EepromTx(0);                                                  // guardamos bits de configuración
            Camb_buzzer = 0;
          }
          if (Camb_Tmax) {
            Temp_Max[Memo - 1] = Tmax;
            EepromTx(4 * Memo);
            Camb_Tmax = 0;
          }
          if (Camb_Tmin) {
            Temp_Min[Memo - 1] = Tmin;
            EepromTx(1 + (4 * Memo));
            Camb_Tmin = 0;
          }
          if (Camb_Tstan) {
            Temp_Standby[Memo - 1] = Tstan;
            EepromTx(2 + (4 * Memo));
            Camb_Tstan = 0;
          }
          if (Camb_Trun) {
            Ini_Temp[Memo - 1] = Trun;
            EepromTx(3 + (4 * Memo));
            Camb_Trun = 0;
          }
          uint8_t AntMemo = Memo;                                         // memoria anterior
          if (Selec != 9) {
            Memo = Selec - 5;                                             // obtenemos memoria actual
            EepromRx(0);                                                  // actualizamos el valor de configuración
            EepromRx(Memo);
            // obtenemos los nuevos valores a mostrar en pantalla
            Tmax = Temp_Max[Memo - 1];
            Tmin = Temp_Min[Memo - 1];
            Tstan = Temp_Standby[Memo - 1];
            Trun = Ini_Temp[Memo - 1];
            bit_buzzer = bitRead(Config, Memo - 1);
            // mostramos nuevos valores para la memoria en curso
            tft.fillRect(225, 128, 60, 18, WHITE);                        // borramos zona ON-OFF
            tft.setTextColor(RED);
            tft.setCursor(223, 129);
            bit_buzzer == 1 ? tft.print("[ON]") : tft.print("[OFF]");
            tft.setTextColor(BLACK);
            tft.fillRect(272, 9, 55, 18, WHITE);                          // borramos zona Tmax
            tft.setCursor(273, 10);
            tft.print(Tmax);
            tft.fillRect(272, 39, 55, 18, WHITE);                         // borramos zona Tmin
            tft.setCursor(273, 40);
            tft.print(Tmin);
            tft.fillRect(272, 69, 55, 18, WHITE);                         // borramos zona Tstan
            tft.setCursor(273, 70);
            tft.print(Tstan);
            tft.fillRect(272, 99, 55, 18, WHITE);                         // borramos zona Trun
            tft.setCursor(273, 100);
            tft.print(Trun);
            // actualizamos cambios de la memoria actual
            if (AntMemo == 2 && Memo == 1) {                              // si pasamos de la memoria 2 a la 1
              tft.fillRect(10, 180, 85, 30, GREEN);                       // botón memoria 1 activo
              tft.fillRect(100, 180, 85, 30, GREY);                       // botón memoria 2 desactivado
              tft.setTextColor(RED);
              tft.setCursor(15, 187);
              tft.print("Mem. 1");
              tft.setTextColor(BLACK);
              tft.setCursor(105, 187);
              tft.print("Mem. 2");
            }
            if (AntMemo == 3 && Memo == 1) {                              // si pasamos de la memoria 3 a la 1
              bitWrite(Config, 2, bit_buzzer);
              tft.fillRect(10, 180, 85, 30, GREEN);                       // botón memoria 1 activo
              tft.fillRect(190, 180, 85, 30, GREY);                       // botón memoria 3 desactivado
              tft.setTextColor(RED);
              tft.setCursor(15, 187);
              tft.print("Mem. 1");
              tft.setTextColor(BLACK);
              tft.setCursor(195, 187);
              tft.print("Mem. 3");
            }
            if (AntMemo == 1 && Memo == 2) {                              // si pasamos de la memoria 1 a la 2
              bitWrite(Config, 0, bit_buzzer);
              tft.fillRect(100, 180, 85, 30, GREEN);                      // botón memoria 2 activo
              tft.fillRect(10, 180, 85, 30, GREY);                        // botón memoria 1 desactivado
              tft.setTextColor(RED);
              tft.setCursor(105, 187);
              tft.print("Mem. 2");
              tft.setTextColor(BLACK);
              tft.setCursor(15, 187);
              tft.print("Mem. 1");
            }
            if (AntMemo == 3 && Memo == 2) {                              // si pasamos de la memoria 3 a la 2
              bitWrite(Config, 2, bit_buzzer);
              tft.fillRect(100, 180, 85, 30, GREEN);                      // botón memoria 2 activo
              tft.fillRect(190, 180, 85, 30, GREY);                       // botón memoria 3 desactivado
              tft.setTextColor(RED);
              tft.setCursor(105, 187);
              tft.print("Mem. 2");
              tft.setTextColor(BLACK);
              tft.setCursor(195, 187);
              tft.print("Mem. 3");
            }
            if (AntMemo == 1 && Memo == 3) {                              // si pasamos de la memoria 1 a la 3
              bitWrite(Config, 0, bit_buzzer);
              tft.fillRect(190, 180, 85, 30, GREEN);                      // botón memoria 3 activo
              tft.fillRect(10, 180, 85, 30, GREY);                        // botón memoria 1 desactivado
              tft.setTextColor(RED);
              tft.setCursor(195, 187);
              tft.print("Mem. 3");
              tft.setTextColor(BLACK);
              tft.setCursor(15, 187);
              tft.print("Mem. 1");
            }
            if (AntMemo == 2 && Memo == 3) {                              // si pasamos de la memoria 2 a la 3
              tft.fillRect(190, 180, 85, 30, GREEN);                      // botón memoria 3 activo
              tft.fillRect(100, 180, 85, 30, GREY);                       // botón memoria 2 desactivado
              tft.setTextColor(RED);
              tft.setCursor(195, 187);
              tft.print("Mem. 3");
              tft.setTextColor(BLACK);
              tft.setCursor(105, 187);
              tft.print("Mem. 2");
            }
          } else {
            if (BuzzerOnOFF == 1) {
              digitalWrite(Buzzer_pin, HIGH);
              DelayMs (250);
              digitalWrite(Buzzer_pin, LOW);
            }
            asm("jmp 0x0000");                                            // reset por software
            break;
          }
          digitalWrite(Buzzer_pin, HIGH);
          DelayMs (150);
          digitalWrite(Buzzer_pin, LOW);
          break;
      }
    }
  }
}

#endif
