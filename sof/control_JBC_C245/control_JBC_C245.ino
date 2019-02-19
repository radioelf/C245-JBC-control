/*
   Control para soldadores JBC C245, para arduino Mega, TFT HX8352A [OPEN-SMART]  

  JBC C245 tiene un termopar desconocido, según la calibración realizada con una sonda externa
  colocada sobre la punta del soldador, he obtenido el factor de 0.4693221 para usar en la lectura ADC
  con Vcc de 4.9V, el valor medido de Ra1 de 99959.0 ohm y Ra2 de 329.52 ohm.
  
  -el amplificado operacional tiene una ganancia de 1+(Ra1/Ra2) -> 1 + (99959.0 / 329.52)->304.347293032
  -la precisión para el conversor ADC (10 bits) Vcc/1024 (1023 escalones de ~4.88mV)
  -calculo tensión entrada ADC del arduino mega V = (ADC) * Vcc / 1024 el valor ADC corresponde a un
   intervalo de tensión, (de 0 a ~4.88mV-> ADC =0, de ~4.89mV a ~9.76mV-> ADC =1... de 4.99V a 5V-> ADC =1023) 
  -la temperatura en grados centígrados para 1mV es de unos ~29.85°C (calibración a 300°C)

  Calculo para obtener el factor ADC: 
    factor =temperatura por 1mV/((0.001 * (1 + (Ra1/ Ra2)))/(Vcc /1024.0))
    
  En algunos foros se comenta que el termopar es de tipo N que entrega 1mV a 37.63°C
         37.63/((0.001 * (1 + (99959.0 / 329.52)))/(5 / 1024.0)) =0.6037189703392771
  En algunos foros se comenta que el termopar entrega 1mV a 43.5°C
         43.5/((0.001 * (1 + (99959.0 / 329.52)))/(5 / 1024.0)) =0.6978946375168363

  Se realiza una calibración a 300 grados y obtenemos un valor de ~29.85°C para 1mV
  calculo para Vcc a 4.9V-> 29.85/((0.001 * (1 + (99959.0 / 329.52)))/(4.9 / 1024.0)) =0.4693221110639084
  factor = 0.4693221 (arduino mega solo utiliza 7 decimales)

  Valores PID:
  KP: si se incrementa, más fuerte es el control.
  KI: si se decrementa (0 =OFF), más rápidamente reaccionará el controlador a los cambios, pero
      mayor es el riesgo de oscilaciones.
  KD: cuanto mayor es el valor, más amortigua las oscilaciones el controlador (pero el rendimiento
      puede verse afectado)

     Radioelf - Febrero 2019
  http://radioelf.blogspot.com.es/

  Copyright (c) 2019 Radioelf.  All rights reserved.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

*/
#define Version "Control JBC V 0.0.4"                             
const String Fecha = String(__DATE__);                            // obtenemos la fecha
//#define Debug                                                   // ON-OFF debugger serial

#include <Wire.h>
#include <LM75.h>                                                 // https://github.com/thefekete/LM75
#include <EEPROM.h>
#include <PID_v1.h>                                               // https://github.com/br3ttb/Arduino-PID-Library/ (MOD.)
// SD
//#include <SPI.h>
//#include <SD.h>

#define SET(port,bit) (port|=(1<<bit))                            // macro para pasar a 1 el bit indicado
#define CLR(port,bit) (port&=(~(1<<bit)))                         // macro para pasar a 0 el bit indicado

// Memorias de 1 a la 3 (defecto)
uint8_t SelecMem = 0;
uint8_t  Config = B10000111;                                      // mascara de configuración
uint16_t Temp_Max[3] = {450, 450, 450};                           // temperatura máxima 450 °C M1, M2, M3
uint16_t Temp_Standby[3] = {180, 200, 225};                       // temperatura en modo standby M1, M2, M3
uint8_t  Temp_Min[3] = {160, 160, 160};                           // temperatura minina (0-255) M1, M2, M3
uint16_t Ini_Temp[3] = {340, 350 , 375};                          // temperatura inicial de trabajo M1, M2, M3
//Valores para proporcional, derivada e integral
double Kp = 2238, Ki = 14, Kd = 47.5;
double EsperaPID = 50, InputTemp = 0.0, Setpoint = Ini_Temp[0];

const double Factor = 0.4693221;								                  // factor calculado para 29.85 grados centígrados a 1mV

volatile double Temp = 0.0;
volatile uint8_t ContaISR = 0;
volatile uint16_t ADC_ISR = 0, ReposoADC;
double Tem_lm75, Tem_comp;

#define Buzzer_pin 2                                                // Pin zumbador      
//#define LCD_Ledk_pin 3
#define PasoCero_pin 18                                           	// Pin 46 ATMEGA2560-> PD3
#define Control_pin 16                                            	// Pin 13 ATMEGA2560-> PH1
#define LedRD_pin 14                                              	// led rojo
#define LedGN_pin 15                                              	// led verde
#define TC_pin A5                                                 	// ADC termopar JBC
#define Reposo_Pin A6                                             	// ADC reposo

#include "bitmap.h"
#include "eeprom.h"
#include "tft.h"

// InputTemp entrada de la temperatura del soldador
// EsperaPID valor a enviar Timer1 tras el paso por cero
// Setpoint valor de la temperatura deseada
// DIRECT al disminuir el valor se disminuye la temperatura (tiempo en ON)
// REVERSE al disminuir el valor se aumenta la temperatura (tiempo en OFF)
PID myPID(&InputTemp, &EsperaPID, &Setpoint, Kp, Ki, Kd, DIRECT);

//********************************************************************************************
// ISR por paso por cero (10ms->50hz) leemos la entrada analógica, obtenemos el valor del
// periodo de marcha e iniciamos el timer 1
//--------------------------------------------------------------------------------------------
void PasoCero_ISR() {
  if  (PINH & (1 << PH1)) {                                         // si el pin 16 se encuentra activo
    CLR(PORTH, PH1);
    return;
  }
  ADC_ISR = analogRead(TC_pin);
  Temp = InputTemp = (ADC_ISR * Factor) + Tem_comp;
  if (Temp - 5.0 < Setpoint) SET(PORTH, PH1);                       // ON pin 16 control si temperatura es < de setpoin+5grados
  if (Setpoint > Temp + 50.0 )                                      // si la diferencia es mayor de 50 grados
    EsperaPID = 19000;                                              // pulso ON de 9,500ms, OFF 500us
  else
    myPID.Compute(1);                                               // EsperaPID toma el valor del calculo PID
  TCNT1 = 0;                                                     	  // contador a 0
  OCR1A = uint16_t (EsperaPID);                                   	// pulso de (EsperaPID*0,5us)-> de 25us a 9025us
  TCCR1B |= _BV(CS11);                                            	// ON Timer 1, ~0,5uS pre-escaler a 8
  TIMSK1 |= _BV(OCIE1A);                                           	// RUN ISR comparador
}
//********************************************************************************************
// ISR comparador timer 1, se para la salida hasta el próximo paso por cero (modo PID DIRECT)
// cada 10 ciclos leemos la entrada analógica de soldador en reposo
//--------------------------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect) {
  CLR(PORTH, PH1);                                                	// paramos señal de control
  TCCR1B &= ~_BV(CS11);                                           	// paramos timer1
  TIMSK1 &= ~_BV(OCIE1A);                                           // STOP ISR comparador
  if (++ContaISR == 10) {
    ReposoADC = analogRead(Reposo_Pin);                           	// ~25us-9ms después de leer TC_pin->A5
    ContaISR = 0;
  }
}
//********************************************************************************************
// ISR comparador timer 5, paramos timer 5 y off buzzer
//--------------------------------------------------------------------------------------------
ISR(TIMER5_COMPA_vect) {
  CLR(PORTE, PE4);
  TCCR5B = 0;                                                       // off timer 5
  TIMSK5 &= ~_BV(OCIE5A);                                           // STOP ISR comparador
}
//********************************************************************************************
// Configuración
//--------------------------------------------------------------------------------------------
void setup() {
  pinMode(TC_pin, INPUT);
  pinMode(Reposo_Pin, INPUT);
  pinMode(PasoCero_pin, INPUT_PULLUP);
  pinMode(Control_pin, OUTPUT);
  pinMode(LedRD_pin, OUTPUT);
  pinMode(LedGN_pin, OUTPUT);
  pinMode(Buzzer_pin, OUTPUT);
  pinMode(LCD_Ledk_pin, OUTPUT);
  digitalWrite(Control_pin, LOW);                                   // calentamiento soldador parado
  digitalWrite(LedRD_pin, HIGH);
  digitalWrite(LedGN_pin, HIGH);                                  	// led en naranja (rojo+verde)
  digitalWrite(Buzzer_pin, LOW);                                    // zumbador parado
  digitalWrite(LCD_Ledk_pin, LOW);                                  // retro-iluminación pantalla LCD OFF
#ifdef Debug
  Serial.begin(115200);
#endif
  Wire.setClock(400000);                                          	// 400kHz I2C clock.
  Wire.begin();

  //El preescaler ADC por defecto 128->16MHz/128 = 125 KHz, 13.5 ciclos cada muestra, sampling rate 125/13.5 =9.2Khz->108.7uS
  // preescale mínimo recomendado es de 16->1Mhz, 16MHz/16 =1Mhz, sampling rate 1000/13.5 =74Khz->13.5uS
  ADCSRA |= _BV(ADPS2);                                           	// bit 2 registro ADCSRA a 1
  ADCSRA &= ~_BV(ADPS1);                                          	// bit 1 registro ADCSRA a 0
  ADCSRA &= ~_BV(ADPS0);                                          	// bit 0 registro ADCSRA a 0

  //for (uint8_t x = 0; x < 16; x++) {EepromTx(x);}
  for (uint8_t x = 0; x < 4; x++) {
    EepromRx(x);                                                  	// leemos la configuración de la EEPROM de la posición 0 a la 3
  }
  BuzzerOnOFF = bitRead(Config, 0);                               	// leemos el estado del bit 0 (memoria 1)

  myPID.SetMode(AUTOMATIC);                                       	// enciende el PID
  myPID.SetOutputLimits(50, 18050);                               	// limites  de 50 a 18050, pulso de ~25us-9ms
  myPID.SetSampleTime(10);                                        	// se llama a Compute() cada 10 ms

  if (analogRead(Reposo_Pin) > 100) {
    Setpoint = Ini_Temp[0];
    ReposoMod = 0;
  } else {
    Setpoint = Temp_Standby[0];
  }

  Tem_lm75 = sensor.temp();                                       	// obtenemos la temperatura de la PCB
  Tem_comp = Tem_lm75;                                              // al arrancar es la misma que la del ambiente (union fria)
  InputTemp = Tem_lm75;                                             // la temperatura inicial es la de la PCB

  IniTFT (Setpoint, InputTemp, 0);                                  // inicializamos LCD
  digitalWrite(Buzzer_pin, HIGH);
  if (BuzzerOnOFF == 1) {
    DelayMs (50);
    digitalWrite(Buzzer_pin, LOW);
    DelayMs (100);
    digitalWrite(Buzzer_pin, HIGH);
    DelayMs (50);
  } else {
    DelayMs (50);
  }
  digitalWrite(Buzzer_pin, LOW);
  digitalWrite(LedRD_pin, LOW);
  digitalWrite(LedGN_pin, LOW);
  noInterrupts();
  // Config. pre-escaler a 8 bit, modo CTC, Timer 1 OFF (pre-escaler CS10, CS11 y CS12 a 0)
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= _BV(WGM12);                                             // modo CTC, bit WGM12 a 1
  TCCR1B |= _BV(CS11);                                              // Timer 1 ON, pre-escaler a 8->0,5uS, bit CS11 a 1
  TCNT1 = 0;                                                        // contador a 0
  OCR1A = 0;                                                        // comparador a 0
  //TIMSK1 |= _BV(OCIE1A);                                          // ON ISR comparador, bit1 OCIE1A del registro TIMSK1 a 1
  TIMSK1 &= ~_BV(OCIE1A);                                         	// STOP ISR comparador, bit1 OCIE1A del registro TIMSK1 a 0
  // configuramos Timer 5 STOP
  TCCR5A = 0;
  TCCR5B = 0;                                                       // timer 5 OFF
  OCR5A = 0;                                                        // comparador a 0
  TIMSK5 &= ~_BV(OCIE5A);                                           // STOP ISR comparador
  interrupts();
  Buzzer(0);
  attachInterrupt(digitalPinToInterrupt(PasoCero_pin), PasoCero_ISR, RISING); // flanco de subida de LOW a HIGH
  DelayMs (55);                                                     // nos aseguramos de realizar varias lecturas (ISR)
}

uint8_t SelecZona = 0, LedRun = 0, Pulsacion = 0, PorceAnt = 0, Media = 0, ErrorISR = 0;
uint8_t RepTra = 0, Ciclo = 0, Error = 0, OK_Temp = 1, STOP = 0, CicloLoop = 0;
int8_t MemISR = -1;
uint16_t  MediaTemp = 0;
uint32_t Perido_Reposo = 0;
double Tem_PCB, TempAnt = 0.0;
//********************************************************************************************
// Principal
//--------------------------------------------------------------------------------------------
void loop() {
  DelayMs (5);
  if (STOP == 0) {
    actualiza (CicloLoop++);                                       	// como mínimo cada 5ms actualizamos
  } else {
    if (++STOP == 11) {
      uint8_t Parpadeo = !digitalRead(LedRD_pin);
      digitalWrite(LedRD_pin, Parpadeo);
      digitalWrite(LedGN_pin, Parpadeo);
      STOP = 1;
    }
    DelayMs (45);
    if (TFTpuntero()) {                                             // si nos encontramos en STOP
      if (TFTzona(0) == 7) {                                       	// pulsado botón ON-OFF?
        OffOn_JBC(1);                                               // ON control soldador
        Buzzer(50);
        Perido_Reposo = millis();
      } else {                                                      // pulsación NO valida
        if (BuzzerOnOFF == 1) {
          digitalWrite(Buzzer_pin, HIGH);
          DelayMs (250);
          digitalWrite(Buzzer_pin, LOW);
        }
      }
    }
  }
}
//********************************************************************************************
// Actualización, una opción en cada paso por cero (10ms a 50Hz)
//--------------------------------------------------------------------------------------------
void actualiza(uint8_t opcion) {
  switch (opcion) {
    case 1:                                                         // 265us
      Pulsacion = TFTpuntero();                                     // comprobamos si se esta pulsando en la pantalla
      break;
    case 2:                                                         // 4us
      if (Pulsacion)                                                // comprobamos si se pulso una zona valida
        SelecZona = TFTzona(0);                                     // obtenemos la zona pulsada para su gestión
      break;
    case 3:                                                         // 4us
      if (SelecZona) {                                              // zonas de 1 al 8
        Buzzer(50);                                                 // indicamos pulsación en zona valida
        if (SelecZona  == 8) {                                      // inicializar periodo de reposo
          Perido_Reposo = millis();
          SelecZona = 0;
          break;
        }
        Boton(SelecZona);                                           // gestión botón pulsado
        DelayMs(50);
        while (TFTpuntero()) {
          Boton(TFTzona(0));
          DelayMs(150);
        }
      }
      break;
    case 4:                                                         // 24us
      if (++Ciclo == 10) {                                          // leemos la temperatura de la PCB
        Tem_PCB = sensor.temp();
        Ciclo = 0;
        if (Tem_lm75 != Tem_PCB) {                                  // si la temperatura es distinta a la mostrada anteriormente en pantalla
          TFTLm75(Tem_PCB);                                         // actualizamos nueva temperatura en pantalla
          Tem_lm75 = Tem_PCB;
        }
      } else {
        RepTra = Reposo();                                          // comprobamos si tenemos el soldador en reposo
        if (ModoTrab != RepTra && ModoTrab != 0) {                  // si tenemos cambio de estado y no estamos en apagado
          noInterrupts();
          RepTra == 1 ? Setpoint = Ini_Temp[SelecMem] : Setpoint = Temp_Standby[SelecMem];
          int16_t IntSetPoint = Setpoint;
          interrupts();
          TFTUpdateSet(IntSetPoint);
          TFTEsperaOnOff();                                        	// actualizamos estado botón ON/OFF en TFT
          ModoTrab = RepTra;
          RepTra == 2 ? OK_Temp = 0 : OK_Temp = RepTra;
        }
      }
      break;
    case 5:
      if (Ciclo == 5) {
        int16_t TempMedia = round(MediaTemp / Media);				        // 85ms
        MediaTemp = 0;
        Media = 0;
        if (TempMedia != TempAnt && Error == 0) {                  	// si tenemos un valor distinto actualizamos en TFT
          TFTColorTemp(Setpoint, Temp);
          TempAnt = TempMedia;
        }
      } else {
        if (Temp < (Temp_Max[SelecMem] + 25)) {
          MediaTemp += uint16_t (Temp);
          Media++;
        }
        if (Setpoint <= Temp && OK_Temp != 0 ) {
          Buzzer(50);
          OK_Temp = 0;
        }
      }
      break;
    case 6:                                                         // 56us
      Led_status ();                                                // leds
      if (Ciclo % 3 == 0) {
        if (ContaISR == MemISR) {
          ErrorISR++;
        } else {
          MemISR = ContaISR;
          ErrorISR = 0;
        }
        if ((Temp > 460) || (Temp <= Tem_comp) || (ErrorISR > 5) ) { // seguridad
          noInterrupts();
          uint16_t AdcIsr = ADC_ISR;
          interrupts();
          if ((AdcIsr > 800 || AdcIsr == 0) && Error >= 2) Error = 99;// sin punta del soldador
          if (ErrorISR > 2) Error = 199;                            // error en ISR
          if (++Error > 9) {
            OffOn_JBC(0);                                           // OFF control soldador
            TFTError(Error);                                        // si es un cambio de punta soldador (la función retorna)
            CambTool();                                             // cambio de punta
            Error = 0;
            ErrorISR = 0;
          }
        } else {
          Error = 0;
        }
      }
      if (Ciclo % 2 == 0) {
        uint8_t Porce = Porcentaje();                               // obtenemos el porcentaje de trabajo
        if (Porce != PorceAnt) {                                    // si es distinto actualizamos nuevo valor en TFT
          TFTPorcentaje(Porce);
          PorceAnt = Porce;
        }
      }
      CicloLoop = 0;
      break;
  }
}
//********************************************************************************************
// Gestión del botón pulsado en pantalla TFT
//--------------------------------------------------------------------------------------------
void Boton(uint8_t Selec) {
  uint16_t IntSetPoint;
  SelecZona = 0;
  switch (Selec) {
    case 1:                                                         // selección a acceso a configuración
      OffOn_JBC(0);                                                 // OFF control soldador
      TFTConfigJBC();
      break;
    case 2:                                                         // selección memoria 1
      if (ModoTrab == 0) {                                          // si estamos es OFF salimos
        Buzzer(250);
        break;
      }
      TFTMemoria(1);
      noInterrupts();
      ModoTrab == 1 ? Setpoint = double (Ini_Temp[SelecMem]) : Setpoint = double (Temp_Standby[SelecMem]);
      IntSetPoint = uint16_t (Setpoint);
      interrupts();
      TFTUpdateSet(IntSetPoint);
      break;
    case 3:                                                         // selección memoria 2
      if (ModoTrab == 0) {                                          // si estamos es OFF salimos
        Buzzer(250);
        break;
      }
      TFTMemoria(2);
      noInterrupts();
      ModoTrab == 1 ? Setpoint = double (Ini_Temp[SelecMem]) : Setpoint = double (Temp_Standby[SelecMem]);
      IntSetPoint = uint16_t (Setpoint);
      interrupts();
      TFTUpdateSet(IntSetPoint);
      break;
    case 4:                                                         // selección memoria 3
      if (ModoTrab == 0) {                                          // si estamos es OFF salimos
        Buzzer(250);
        break;
      }
      TFTMemoria(3);
      noInterrupts();
      ModoTrab == 1 ? Setpoint = double (Ini_Temp[SelecMem]) : Setpoint = double (Temp_Standby[SelecMem]);
      IntSetPoint = uint16_t (Setpoint);
      interrupts();
      TFTUpdateSet(IntSetPoint);
      break;
    case 5:                                                         // selección incremento/decremento setpoint
      if (ModoTrab == 0) {                                          // si estamos es OFF salimos
        Buzzer(250);
        break;
      }
      if (Setpoint >= Temp_Max[SelecMem]) break;                    // limite superior?
      noInterrupts();
      ModoTrab == 1 ? Setpoint = ++InputTemp : Setpoint = double (++Temp_Standby[SelecMem]);
      IntSetPoint = uint16_t (Setpoint);
      interrupts();
      TFTUpdateSet(IntSetPoint);
      break;
    case 6:
      if (Setpoint <= Temp_Min[SelecMem]) break;                    // limite inferior?
      noInterrupts();
      ModoTrab == 1 ? Setpoint = --InputTemp : Setpoint = double (--Temp_Standby[SelecMem]);
      IntSetPoint = uint16_t (Setpoint);
      interrupts();
      TFTUpdateSet(IntSetPoint);
      break;
    case 7:                                                         // pulsación botón ON-OFF
      Buzzer(50);
      OffOn_JBC(TFTOnOff(1));                                       // mostramos el estado de la pulsación del botón de ON-OFF
      break;
    case 8:                                                         // pulsación botón espera
      Perido_Reposo = millis();                                     // iniciamos periodo reposo
      Buzzer(50);
      break;
  }
}
//********************************************************************************************
// Gestión led según temperatura y setpoint
//--------------------------------------------------------------------------------------------
void Led_status() {
  LedRun++;
  if (LedRun == 9) {
    digitalWrite (Temp + 2 < Setpoint ? LedRD_pin : LedGN_pin, HIGH);
  }
  if (LedRun == 18) {
    LedRun = 0;
    digitalWrite(LedRD_pin, LOW);
    digitalWrite(LedGN_pin, LOW);
  }
}
//********************************************************************************************
// Comprobamos si el soldador se encuentra en reposo
//--------------------------------------------------------------------------------------------
uint8_t Reposo() {
  if (ReposoADC < 100) {                                            // si el soldador se encuentra es reposos
    if (millis() > Perido_Reposo + 900000L) {                     	// más de 15 minutos en reposo
      OffOn_JBC(0);                                                 // OFF control soldador
      TFTOnOff(0);
      TFTreposo();
      uint8_t IniRun = 0;
      do {
        if (BuzzerOnOFF == 1) {
          digitalWrite(Buzzer_pin, HIGH);
          DelayMs (500);
          if (TFTpuntero()) IniRun = TFTzona(0);
          digitalWrite(Buzzer_pin, LOW);
        }
        DelayMs (300);
        if (TFTpuntero()) IniRun = TFTzona(0);                      // si se pulsa la opción 8 (espera) salimos
      } while (analogRead(Reposo_Pin) < 100 && IniRun != 8);        // salimos si de descuelga el soldador
      Perido_Reposo = millis();
      ModoTrab = 1;
      OffOn_JBC(1);                                                 // ON control
      TFTOnOff(1);
      if (IniRun == 8) return 2;                                    // en reposo
      return 1;                                                     // en trabajo
    }
    return 2;                                                       // en reposo
  } else {
    DelayMs (5);
    if (analogRead(Reposo_Pin) < 100) return ModoTrab;
    Perido_Reposo = millis();
    return 1;                                                       // en trabajo
  }
}
//********************************************************************************************
// Gestión zumbador
//--------------------------------------------------------------------------------------------
void Buzzer(uint8_t ms) {
  if (BuzzerOnOFF == 1) {                                           // comprobamos si tenemos permiso para buzzer
    TCNT5 = 0;
    OCR5A = uint16_t (ms * 250);
    TCCR5B = B0001011;                                              // modo CTC, Prescaler a 64 8->4uS
    digitalWrite(Buzzer_pin, HIGH);
    TIMSK5 |= _BV(OCIE5A);                                          // ON ISR comparador
  }
}
//********************************************************************************************
// Obtenemos el porcentaje de trabajo, 50-18050 limites PID
//--------------------------------------------------------------------------------------------
uint8_t Porcentaje() {
  uint8_t x = 0;
  if (Temp - 5.0 > Setpoint) return 0;
  noInterrupts();
  uint16_t Espera = uint16_t (EsperaPID);
  interrupts();
  Espera <= 50 ? x = 1 : x = round(Espera  * 0.005540166f);         //  100/18050 =0,005540166
  return x;
}
//********************************************************************************************
// Paramos/arrancamos el control del soldador
//--------------------------------------------------------------------------------------------
void OffOn_JBC(uint8_t OffOn) {
  if (OffOn == 0) {                                                 // desactiva la interrupciones
    noInterrupts();
    digitalWrite(Control_pin, LOW);
    digitalWrite(Buzzer_pin, LOW);
    TCCR1B &= ~_BV(CS11);                                           // paramos timer1
    TIMSK1 &= ~_BV(OCIE1A);                                         // STOP ISR comparador
    TFTPorcentaje(0);
    CicloLoop = 0;
    STOP = 1;
    if (BuzzerOnOFF == 1) {
      digitalWrite(Buzzer_pin, HIGH);
      DelayMs (250);
      digitalWrite(Buzzer_pin, LOW);
    }
    digitalWrite(LedRD_pin, HIGH);
    digitalWrite(LedGN_pin, HIGH);                                  // led en naranja (rojo+verde)
  } else {
    digitalWrite(LedGN_pin, LOW);
    interrupts();
    TFTOnOff(1);
    PorceAnt = 1;
    TempAnt = 0;
    STOP = 0;
    CicloLoop = 0;
    TFTPorcentaje(PorceAnt);
    Buzzer(100);
  }
}
//********************************************************************************************
// Cambio punta soldador (al leer sin punta leemos +880 en un periodo y 0 en el siguiente)
//--------------------------------------------------------------------------------------------
void CambTool() {
  uint16_t AdcTool = 0;
  if (BuzzerOnOFF == 1) {
    digitalWrite(Buzzer_pin, HIGH);
    DelayMs (25);
    digitalWrite(Buzzer_pin, LOW);
  }
  do {
    digitalWrite(LedRD_pin, HIGH);
    digitalWrite(LedGN_pin, LOW);
    DelayMs(250);
    digitalWrite(LedRD_pin, LOW);
    digitalWrite(LedGN_pin, HIGH);
    DelayMs(250);
    while (digitalRead (PasoCero_pin) == 0) {                         // esperamos paso por cero
      ;
    }
    if (AdcTool == 0) {                                               // 2 pulsos para comprobar si calentamos punta 
      DelayMs(3);                                                     // 3 ms después del paso por cero
      digitalWrite(Control_pin, HIGH);
      DelayMs(3);                                                     // 6 ms después del paso por cero
      digitalWrite(Control_pin, LOW);
      DelayMs(7);                                                     // 13 ms después del paso por cero
      digitalWrite(Control_pin, HIGH);
      DelayMs(3);                                                     // 16 ms después del paso por cero
      digitalWrite(Control_pin, LOW);
      while (digitalRead (PasoCero_pin) == 0) {                       // esperamos paso por cero
        ;
      }
    }
    AdcTool = analogRead(TC_pin);                                     // sin punta se lee ~890 y 0
  } while (AdcTool > 800 || AdcTool == 0);                            // ADC =1  (como mínimo ~7.8uV en termopar)
  Perido_Reposo = millis();
  ModoTrab = 2;                                                    	  // 2->espera
  STOP = 0;
  Error = 0;
  interrupts();
  Buzzer(250);
  CicloLoop = 0;
  TFTEsperaOnOff();
  DelayMs(35);
}
