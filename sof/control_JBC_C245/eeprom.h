
#ifndef EEPROM_H
#define EEPROM_H

union double_Byte {
  double    datoF;
  uint8_t  datoB[4];
} unionFB;
union Integer_Byte {
  uint16_t  datoI;
  uint8_t   datoB[2];
} unionIB;
//********************************************************************************************
// Programación EEPROM
//--------------------------------------------------------------------------------------------
void EepromTx(uint8_t direcc) {
  switch (direcc)  {
    case 0:
      EEPROM.update(0, Config);                   // bit0->buzzer ON/OFF M1, bit2->buzzer ON/OFF M3, bit 3->buzzer ON/OFF M3, bit 7 eeprom OK
      break;
    case 1:
      unionFB.datoF = Kp;                         // valor de proporcional para PID
      EEPROM.update(1, unionFB.datoB[0]);
      EEPROM.update(2, unionFB.datoB[1]);
      EEPROM.update(3, unionFB.datoB[2]);
      EEPROM.update(4, unionFB.datoB[3]);
      break;
    case 2:
      unionFB.datoF = Ki;                         // valor de integral para PID
      EEPROM.update(5, unionFB.datoB[0]);
      EEPROM.update(6, unionFB.datoB[1]);
      EEPROM.update(7, unionFB.datoB[2]);
      EEPROM.update(8, unionFB.datoB[3]);
      break;
    case 3:
      unionFB.datoF = Kd;                         // valor de derivada para PID
      EEPROM.update(9, unionFB.datoB[0]);
      EEPROM.update(10, unionFB.datoB[1]);
      EEPROM.update(11, unionFB.datoB[2]);
      EEPROM.update(12, unionFB.datoB[3]);
      break;
    case 4:
      unionIB.datoI = Temp_Max[0];                // temperatura máxima M1
      EEPROM.update(13, unionIB.datoB[0]);
      EEPROM.update(14, unionIB.datoB[1]);
      break;
    case 5:
      unionIB.datoI = Temp_Min[0];                // temperatura mínima M1
      EEPROM.update(15, unionIB.datoB[0]);
      EEPROM.update(16, unionIB.datoB[1]);
      break;
    case 6:
      unionIB.datoI = Temp_Standby[0];            // temperatura espera M1
      EEPROM.update(17, unionIB.datoB[0]);
      EEPROM.update(18, unionIB.datoB[1]);
      break;
    case 7:
      unionIB.datoI = Ini_Temp[0];                //  temperatura inicial de trabajo M1
      EEPROM.update(19, unionIB.datoB[0]);
      EEPROM.update(20, unionIB.datoB[1]);
      break;
    case 8:
      unionIB.datoI = Temp_Max[1];                // temperatura máxima M2
      EEPROM.update(21, unionIB.datoB[0]);
      EEPROM.update(22, unionIB.datoB[1]);
      break;
    case 9:
      unionIB.datoI = Temp_Min[1];                // temperatura mínima M2
      EEPROM.update(23, unionIB.datoB[0]);
      EEPROM.update(24, unionIB.datoB[1]);
      break;
    case 10:
      unionIB.datoI = Temp_Standby[1];            // temperatura espera M2
      EEPROM.update(25, unionIB.datoB[0]);
      EEPROM.update(26, unionIB.datoB[1]);
      break;
    case 11:
      unionIB.datoI = Ini_Temp[1];                //  temperatura inicial de trabajo M2
      EEPROM.update(27, unionIB.datoB[0]);
      EEPROM.update(28, unionIB.datoB[1]);
      break;
    case 12:
      unionIB.datoI = Temp_Max[2];                // temperatura máxima M3
      EEPROM.update(29, unionIB.datoB[0]);
      EEPROM.update(30, unionIB.datoB[1]);
      break;
    case 13:
      unionIB.datoI = Temp_Min[2];                // temperatura mínima M3
      EEPROM.update(31, unionIB.datoB[0]);
      EEPROM.update(32, unionIB.datoB[1]);
      break;
    case 14:
      unionIB.datoI = Temp_Standby[2];            // temperatura espera M3
      EEPROM.update(33, unionIB.datoB[0]);
      EEPROM.update(34, unionIB.datoB[1]);
      break;
    case 15:
      unionIB.datoI = Ini_Temp[2];                //  temperatura inicial de trabajo M3
      EEPROM.update(35, unionIB.datoB[0]);
      EEPROM.update(36, unionIB.datoB[1]);
      break;
  }
}
//********************************************************************************************
// Leer EEPROM
//--------------------------------------------------------------------------------------------
void EepromRx(uint8_t grupo) {
  switch (grupo)  {
    case 0:
      Config =  EEPROM.read(0);                           // configuración
      break;
    case 1:
      unionIB.datoB[0] =  EEPROM.read(13);
      unionIB.datoB[1] =  EEPROM.read(14);
      Temp_Max[0] = unionIB.datoI;                        // temperatura máxima M1
      unionIB.datoB[0] =  EEPROM.read(15);
      unionIB.datoB[1] =  EEPROM.read(16);
      Temp_Min[0] = unionIB.datoI;                        // temperatura mínima M1
      unionIB.datoB[0] =  EEPROM.read(17);
      unionIB.datoB[1] =  EEPROM.read(18);
      Temp_Standby [0] = unionIB.datoI;                   // temperatura espera M1
      unionIB.datoB[0] =  EEPROM.read(19);
      unionIB.datoB[1] =  EEPROM.read(20);
      Ini_Temp[0] = unionIB.datoI;                        // temperatura RUN M1
      break;
    case 2:
      unionIB.datoB[0] =  EEPROM.read(21);
      unionIB.datoB[1] =  EEPROM.read(22);
      Temp_Max[1] = unionIB.datoI;                        // temperatura máxima M2
      unionIB.datoB[0] =  EEPROM.read(23);
      unionIB.datoB[1] =  EEPROM.read(24);
      Temp_Min[1] = unionIB.datoI;                        // temperatura mínima M2
      unionIB.datoB[0] =  EEPROM.read(25);
      unionIB.datoB[1] =  EEPROM.read(26);
      Temp_Standby [1] = unionIB.datoI;                   // temperatura espera M2
      unionIB.datoB[0] =  EEPROM.read(27);
      unionIB.datoB[1] =  EEPROM.read(28);
      Ini_Temp[1] = unionIB.datoI;                        // temperatura RUN M2
      break;
    case 3:
      unionIB.datoB[0] =  EEPROM.read(29);
      unionIB.datoB[1] =  EEPROM.read(30);
      Temp_Max[2] = unionIB.datoI;                        // temperatura máxima M3
      unionIB.datoB[0] =  EEPROM.read(31);
      unionIB.datoB[1] =  EEPROM.read(32);
      Temp_Min[2] = unionIB.datoI;                        // temperatura mínima M3
      unionIB.datoB[0] =  EEPROM.read(33);
      unionIB.datoB[1] =  EEPROM.read(34);
      Temp_Standby [2] = unionIB.datoI;                   // temperatura espera M3
      unionIB.datoB[0] =  EEPROM.read(35);
      unionIB.datoB[1] =  EEPROM.read(36);
      Ini_Temp[2] = unionIB.datoI;                        // temperatura RUN M3
      break;
    case 4:                                               // PID
      unionFB.datoB[0] =  EEPROM.read(1);
      unionFB.datoB[1] =  EEPROM.read(2);
      unionFB.datoB[2] =  EEPROM.read(3);
      unionFB.datoB[3] =  EEPROM.read(4);
      Kp = unionFB.datoF;
      unionFB.datoB[0] =  EEPROM.read(5);
      unionFB.datoB[1] =  EEPROM.read(6);
      unionFB.datoB[2] =  EEPROM.read(7);
      unionFB.datoB[3] =  EEPROM.read(8);
      Ki = unionFB.datoF;
      unionFB.datoB[0] =  EEPROM.read(9);
      unionFB.datoB[1] =  EEPROM.read(10);
      unionFB.datoB[2] =  EEPROM.read(11);
      unionFB.datoB[3] =  EEPROM.read(12);
      Kd = unionFB.datoF;
      break;
  }
}
#endif
