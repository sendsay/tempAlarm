/*
.########..########..####.##....##.########..######..########.########..####....###....##......
.##.....##.##.....##..##..###...##....##....##....##.##.......##.....##..##....##.##...##......
.##.....##.##.....##..##..####..##....##....##.......##.......##.....##..##...##...##..##......
.########..########...##..##.##.##....##.....######..######...########...##..##.....##.##......
.##........##...##....##..##..####....##..........##.##.......##...##....##..#########.##......
.##........##....##...##..##...###....##....##....##.##.......##....##...##..##.....##.##......
.##........##.....##.####.##....##....##.....######..########.##.....##.####.##.....##.########
*/
void printSerial(String str) {
  while (printFlag) {
    // Serial.println(str);
    printFlag = false;
  }
}

/*
.##......##.########..####.########.########....########.########.########..########...#######..##.....##
.##..##..##.##.....##..##.....##....##..........##.......##.......##.....##.##.....##.##.....##.###...###
.##..##..##.##.....##..##.....##....##..........##.......##.......##.....##.##.....##.##.....##.####.####
.##..##..##.########...##.....##....######......######...######...########..########..##.....##.##.###.##
.##..##..##.##...##....##.....##....##..........##.......##.......##........##...##...##.....##.##.....##
.##..##..##.##....##...##.....##....##..........##.......##.......##........##....##..##.....##.##.....##
..###..###..##.....##.####....##....########....########.########.##........##.....##..#######..##.....##
*/
void writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
}

/*
.########..########....###....########.....########.########.########..########...#######..##.....##
.##.....##.##.........##.##...##.....##....##.......##.......##.....##.##.....##.##.....##.###...###
.##.....##.##........##...##..##.....##....##.......##.......##.....##.##.....##.##.....##.####.####
.########..######...##.....##.##.....##....######...######...########..########..##.....##.##.###.##
.##...##...##.......#########.##.....##....##.......##.......##........##...##...##.....##.##.....##
.##....##..##.......##.....##.##.....##....##.......##.......##........##....##..##.....##.##.....##
.##.....##.########.##.....##.########.....########.########.##........##.....##..#######..##.....##
*/
String readStringFromEEPROM(int addrOffset)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0'; // the character may appear in a weird way, you should read: 'only one backslash and 0'
  return String(data);
}

/*
.##.....##..#######..####..######..########....##.....##..######...######..
.##.....##.##.....##..##..##....##.##..........###...###.##....##.##....##.
.##.....##.##.....##..##..##.......##..........####.####.##.......##.......
.##.....##.##.....##..##..##.......######......##.###.##..######..##...####
..##...##..##.....##..##..##.......##..........##.....##.......##.##....##.
...##.##...##.....##..##..##....##.##..........##.....##.##....##.##....##.
....###.....#######..####..######..########....##.....##..######...######..
*/

void voiceMsg() {
  voiceMsgCount++;

  Serial.println(voiceMsgCount);

  if (voiceMsgCount < 3) {
    sendATCommand("AT+CREC=4,\"C:\\User\\choose.amr\",0,100", false);   // ???????????? ???????????????????? ?? ??????????
  } else {
    sendATCommand("ATH", true);
    enterTemp = "";
    enterGest = "";
    enterPhone = "";
    mode = MODE_MENU;
    voiceMsgCount = 0;
    waitTimer.stop();   
  }
}

/*
..######..##.....##.########..######..##....##.########.########.##.....##.########.
.##....##.##.....##.##.......##....##.##...##.....##....##.......###...###.##.....##
.##.......##.....##.##.......##.......##..##......##....##.......####.####.##.....##
.##.......#########.######...##.......#####.......##....######...##.###.##.########.
.##.......##.....##.##.......##.......##..##......##....##.......##.....##.##.......
.##....##.##.....##.##.......##....##.##...##.....##....##.......##.....##.##.......
..######..##.....##.########..######..##....##....##....########.##.....##.##.......
*/
void CheckTemp() {
  sensor.requestTemperatures();               // ???????????????? ??????????????????????
  currentTemp = sensor.getTempCByIndex(0);    // ???????????????????????? ?? ??????????????

  if (currentTemp < (temp - gest)) {
    if (tempFlag) {
      sendSMS(phone, "WARNING! CHECK TEMPEPATURE!!!");
      tempFlag = false;
    }
  } else if (currentTemp > (temp + gest)){
    tempFlag = true;
  }
}

// void pinModeFast(uint8_t pin, uint8_t mode) {
//   switch (mode) {
//     case INPUT:
//       if (pin < 8) {
//         bitClear(DDRD, pin);    
//         bitClear(PORTD, pin);
//       } else if (pin < 14) {
//         bitClear(DDRB, (pin - 8));
//         bitClear(PORTB, (pin - 8));
//       } else if (pin < 20) {
//         bitClear(DDRC, (pin - 14));   // Mode: INPUT
//         bitClear(PORTC, (pin - 8));  // State: LOW
//       }
//       return;
//     case OUTPUT:
//       if (pin < 8) {
//         bitSet(DDRD, pin);
//         bitClear(PORTD, pin);
//       } else if (pin < 14) {
//         bitSet(DDRB, (pin - 8));
//         bitClear(PORTB, (pin - 8));
//       } else if (pin < 20) {
//         bitSet(DDRC, (pin - 14));  // Mode: OUTPUT
//         bitClear(PORTC, (pin - 8));  // State: LOW
//       }
//       return;
//     case INPUT_PULLUP:
//       if (pin < 8) {
//         bitClear(DDRD, pin);
//         bitSet(PORTD, pin);
//       } else if (pin < 14) {
//         bitClear(DDRB, (pin - 8));
//         bitSet(PORTB, (pin - 8));
//       } else if (pin < 20) {
//         bitClear(DDRC, (pin - 14));  // Mode: OUTPUT
//         bitSet(PORTC, (pin - 14));  // State: HIGH
//       }
//       return;
//   }
// }

/*
.########..####..######...####.########....###....##.......########..########....###....########..########....###.....######..########
.##.....##..##..##....##...##.....##......##.##...##.......##.....##.##.........##.##...##.....##.##.........##.##...##....##....##...
.##.....##..##..##.........##.....##.....##...##..##.......##.....##.##........##...##..##.....##.##........##...##..##..........##...
.##.....##..##..##...####..##.....##....##.....##.##.......########..######...##.....##.##.....##.######...##.....##..######.....##...
.##.....##..##..##....##...##.....##....#########.##.......##...##...##.......#########.##.....##.##.......#########.......##....##...
.##.....##..##..##....##...##.....##....##.....##.##.......##....##..##.......##.....##.##.....##.##.......##.....##.##....##....##...
.########..####..######...####....##....##.....##.########.##.....##.########.##.....##.########..##.......##.....##..######.....##...
*/
// bool digitalReadFast(uint8_t pin) {
//   if (pin < 8) {
//     return bitRead(PIND, pin);
//   } else if (pin < 14) {
//     return bitRead(PINB, pin - 8);
//   } else if (pin < 20) {
//     return bitRead(PINC, pin - 14);    // Return pin state
//   }
//   return false;
// }

/*
.########.##....##.########.....
.##.......###...##.##.....##....
.##.......####..##.##.....##....
.######...##.##.##.##.....##....
.##.......##..####.##.....##....
.##.......##...###.##.....##.###
.########.##....##.########..###
*/