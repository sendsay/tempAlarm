#include <Arduino.h>
/*
.########..########.##.....##.########
.##.....##....##....###...###.##......
.##.....##....##....####.####.##......
.##.....##....##....##.###.##.######..
.##.....##....##....##.....##.##......
.##.....##....##....##.....##.##......
.########.....##....##.....##.##......
*/
void processingDTMF(String symbol) { 
  sendATCommand("AT+CREC=5", false);  

  waitTimer.start();

  switch (mode)
  {
  case MODE_MENU:

        printFlag = true;
        printSerial("main menu");

    if (symbol.equals("2")) {
      sendATCommand("AT+CREC=4,\"C:\\User\\tnx.amr\",0,100", false);   // играем приветсвие в трубу
      sendATCommand("ATH", false); 
      sendSMS(innerPhone, "Current temp. : " + (String)currentTemp + ";");   
    } 
    
    if (symbol.equals("5")) {
      sendATCommand("AT+CREC=4,\"C:\\User\\tnx.amr\",0,100", false);   // играем приветсвие в трубу
      sendATCommand("ATH", false); 
      sendSMS(innerPhone, "Temp.: " + (String)temp + "; gist.: " + (String)gest + "; phone: " + phone + ";"); 
    } 
    
    if (symbol.equals("0")) {
      printFlag = true;
      printSerial("change optonst");
      sendATCommand("AT+CREC=4,\"C:\\User\\options.amr\",0,100", false);   // играем приветсвие в трубу
      mode = MODE_PARAMS;
    }
    break;
  
  case MODE_PARAMS:
        printFlag = true;
        printSerial("menu params");

    if (symbol.equals("2")) {
      // printFlag = true;
      // printSerial("change temp");
      sendATCommand("AT+CREC=4,\"C:\\User\\temp.amr\",0,100", false);   // играем приветсвие в трубу
      mode = MODE_SET_TEMP;

    } else if (symbol.equals("5")) {
      // printFlag = true;
      // printSerial("change gist"); 
      sendATCommand("AT+CREC=4,\"C:\\User\\gist.amr\",0,100", false);   // играем приветсвие в трубу     
      mode = MODE_SET_GEST;

    } else if (symbol.equals("0")) {
      // printFlag = true;
      // printSerial("change phone");
      sendATCommand("AT+CREC=4,\"C:\\User\\phone.amr\",0,100", false);   // играем приветсвие в трубу
      mode = MODE_SET_AL_PHONE;
    } else if (symbol.equals("#")) {
      // printFlag = true;
      // printSerial("change phone");
      sendATCommand("AT+CREC=4,\"C:\\User\\hello.amr\",0,100", false);   // играем приветсвие в трубу
      mode = MODE_MENU;
    }

    break;

  case MODE_SET_TEMP:
          printFlag = true;
          printSerial("menu set temp");

      if (symbol.equals("*") == false || symbol.equals("#") == false) {
        enterTemp = ("" + enterTemp) + symbol;
      } 
      if (symbol.equals("#")) {
        sendATCommand("AT+CREC=4,\"C:\\User\\options.amr\",0,100", true);

        temp = enterTemp.toInt();

        writeStringToEEPROM(0, (String)temp);

        printFlag = true;
        printSerial("menu back from temp change");
          
        mode = MODE_PARAMS; 
        enterTemp = "";
    break;    

  case MODE_SET_GEST:
          printFlag = true;
          printSerial("menu set gist");

      if (symbol.equals("*") == false || symbol.equals("#") == false) {
        enterGest = ("" + enterGest) + symbol; 
      } 

      if (symbol.equals("#")) {
        sendATCommand("AT+CREC=4,\"C:\\User\\options.amr\",0,100", false);

        gest = enterGest.toInt();

        writeStringToEEPROM(20, (String)gest);

        printFlag = true;
        printSerial("menu back from gest change");
          
        mode = MODE_PARAMS; 
        enterGest = "";
      }

    break;  

    case MODE_SET_AL_PHONE:
          printFlag = true;
          printSerial("menu set phone");

      if (symbol.equals("*") == false || symbol.equals("#") == false) {
        enterPhone = ("" + enterPhone) + symbol;;
      } 
      if (symbol.equals("#")) {

          sendATCommand("AT+CREC=4,\"C:\\User\\options.amr\",0,100", false);

          phone = enterPhone.toInt();   

          writeStringToEEPROM(40, phone);

          printFlag = true;
          printSerial("menu back from phone change"); 

          mode = MODE_PARAMS; 
          enterPhone = "";
      }

    break;
  
    default:
      break;
    }
  }
}

/*
..######..##.....##.##....##.########..########.##.....##.########
.##....##.##.....##.##...##..##.....##....##....###...###.##......
.##.......##.....##.##..##...##.....##....##....####.####.##......
.##.......#########.#####....##.....##....##....##.###.##.######..
.##.......##.....##.##..##...##.....##....##....##.....##.##......
.##....##.##.....##.##...##..##.....##....##....##.....##.##......
..######..##.....##.##....##.########.....##....##.....##.##......
*/

void CheckDTMF() {
  
  if (SIM800.available())   {                         // Если модем, что-то отправил...
    _response = waitResponse();                       // Получаем ответ от модема для анализа
 //   Serial.println(">" + _response);                  // Выводим поученную пачку сообщений
    int index = -1;
    do  {                                             // Перебираем построчно каждый пришедший ответ
      index = _response.indexOf("\r\n");              // Получаем идекс переноса строки
      String submsg = "";
      if (index > -1) {                               // Если перенос строки есть, значит
        submsg = _response.substring(0, index);       // Получаем первую строку
        _response = _response.substring(index + 2);   // И убираем её из пачки
      }
      else {                                          // Если больше переносов нет
        submsg = _response;                           // Последняя строка - это все, что осталось от пачки
        _response = "";                               // Пачку обнуляем
      }
      submsg.trim();                                  // Убираем пробельные символы справа и слева
      if (submsg != "") {                             // Если строка значимая (не пустая), то распознаем уже её
        // Serial.println("submessage: " + submsg);
        if (submsg.startsWith("+DTMF:")) {            // Если ответ начинается с "+DTMF:" тогда:
          symbol = submsg.substring(7, 8);     // Выдергиваем код нажатой кнопки с 7 позиции длиной 1 (по 8)
          processingDTMF(symbol);                     // Логику выносим для удобства в отдельную функцию

        } else if (submsg.startsWith("+CLIP: \"")) {
          phoneindex = 8;                        // Парсим строку и ...
          innerPhone = submsg.substring(phoneindex, submsg.indexOf("\"", phoneindex)); // ...получаем номер

        } else if (submsg.startsWith("RING")) {         // При входящем звонке...
          sendATCommand("ATA", true);                 // ...отвечаем (поднимаем трубку)          
          sendATCommand("AT+CREC=4,\"C:\\User\\hello.amr\",0,100", false);   // играем приветсвие в трубу 
          waitTimer.start();
        } else if (submsg.startsWith("NO CARRIER")) {
          enterTemp = "";
          enterGest = "";
          enterPhone = "";
          mode = MODE_MENU;
          voiceMsgCount = 0;
          waitTimer.stop();          
        }
      }
    } while (index > -1);                             // Пока индекс переноса строки действителен
  }
}

/*
..######..##.....##..######.
.##....##.###...###.##....##
.##.......####.####.##......
..######..##.###.##..######.
.......##.##.....##.......##
.##....##.##.....##.##....##
..######..##.....##..######.
*/
void CheckSMS() {
  if (lastUpdate + updatePeriod < millis() ) {                    // Пора проверить наличие новых сообщений
    do {
      _response = sendATCommand("AT+CMGL=\"REC UNREAD\",1", true);// Отправляем запрос чтения непрочитанных сообщений
      if (_response.indexOf("+CMGL: ") > -1) {                    // Если есть хоть одно, получаем его индекс
        int msgIndex = _response.substring(_response.indexOf("+CMGL: ") + 7, _response.indexOf("\"REC UNREAD\"", _response.indexOf("+CMGL: ")) - 1).toInt();
        char i = 0;                                               // Объявляем счетчик попыток
        do {
          i++;                                                    // Увеличиваем счетчик
          _response = sendATCommand("AT+CMGR=" + (String)msgIndex + ",1", true);  // Пробуем получить текст SMS по индексу
          _response.trim();                                       // Убираем пробелы в начале/конце
          if (_response.endsWith("OK")) {                         // Если ответ заканчивается на "ОК"
            if (!hasmsg) hasmsg = true;                           // Ставим флаг наличия сообщений для удаления
            sendATCommand("AT+CMGR=" + (String)msgIndex, true);   // Делаем сообщение прочитанным
            sendATCommand("\n", true);                            // Перестраховка - вывод новой строки
            parseSMS(_response);                                  // Отправляем текст сообщения на обработку
            break;                                                // Выход из do{}
          }
          else {                                                  // Если сообщение не заканчивается на OK
            // Serial.println ("Error answer");                      // Какая-то ошибка
            sendATCommand("\n", true);                            // Отправляем новую строку и повторяем попытку
          }
        } while (i < 10);
        break;
      }
      else {
        lastUpdate = millis();                                    // Обнуляем таймер
        if (hasmsg) {
          sendATCommand("AT+CMGDA=\"DEL READ\"", true);           // Удаляем все прочитанные сообщения
          hasmsg = false;
        }
        break;
      }
    } while (1);
  }
}

/*
..######..########.##....##.########...######..##.....##..######.
.##....##.##.......###...##.##.....##.##....##.###...###.##....##
.##.......##.......####..##.##.....##.##.......####.####.##......
..######..######...##.##.##.##.....##..######..##.###.##..######.
.......##.##.......##..####.##.....##.......##.##.....##.......##
.##....##.##.......##...###.##.....##.##....##.##.....##.##....##
..######..########.##....##.########...######..##.....##..######.
*/
void CheckSendSMS() {
  if (_response.startsWith("+CMGS:")) {       // Пришло сообщение об отправке SMS
    int index = _response.lastIndexOf("\r\n");// Находим последний перенос строки, перед статусом
    String result = _response.substring(index + 2, _response.length()); // Получаем статус
    result.trim();                            // Убираем пробельные символы в начале/конце

    if (result == "OK") {                     // Если результат ОК - все нормально
      // Serial.println ("Message was sent. OK");
    }
    else {                                    // Если нет, нужно повторить отправку
      // Serial.println ("Message was not sent. Error");
    }
  }

  if (Serial.available())  {                          // Ожидаем команды по Serial...
    SIM800.write(Serial.read());                      // ...и отправляем полученную команду модему
  };

}

/*
.########.....###....########...######..########.....######..##.....##..######.
.##.....##...##.##...##.....##.##....##.##..........##....##.###...###.##....##
.##.....##..##...##..##.....##.##.......##..........##.......####.####.##......
.########..##.....##.########...######..######.......######..##.###.##..######.
.##........#########.##...##.........##.##................##.##.....##.......##
.##........##.....##.##....##..##....##.##..........##....##.##.....##.##....##
.##........##.....##.##.....##..######..########.....######..##.....##..######.
*/
void parseSMS(String msg) {                                   // Парсим SMS
  String msgheader  = "";
  String msgbody    = "";
  String msgphone   = "";

  msg = msg.substring(msg.indexOf("+CMGR: "));
  msgheader = msg.substring(0, msg.indexOf("\r"));            // Выдергиваем телефон

  msgbody = msg.substring(msgheader.length() + 2);
  msgbody = msgbody.substring(0, msgbody.lastIndexOf("OK"));  // Выдергиваем текст SMS
  msgbody.trim();

  int firstIndex = msgheader.indexOf("\",\"") + 3;
  int secondIndex = msgheader.indexOf("\",\"", firstIndex);
  msgphone = msgheader.substring(firstIndex, secondIndex);

  // Serial.println("Phone: " + msgphone);                       // Выводим номер телефона
  // Serial.println("Message: " + msgbody);                      // Выводим текст SMS
}

/*
..######..########.##....##.########........###....########
.##....##.##.......###...##.##.....##......##.##......##...
.##.......##.......####..##.##.....##.....##...##.....##...
..######..######...##.##.##.##.....##....##.....##....##...
.......##.##.......##..####.##.....##....#########....##...
.##....##.##.......##...###.##.....##....##.....##....##...
..######..########.##....##.########.....##.....##....##...
*/
String sendATCommand(String cmd, bool waiting) {
  String _resp = "";                                  // Переменная для хранения результата
  Serial.println(cmd);                                // Дублируем команду в монитор порта
  SIM800.println(cmd);                                // Отправляем команду модулю
  if (waiting) {                                      // Если необходимо дождаться ответа...
    _resp = waitResponse();                           // ... ждем, когда будет передан ответ
    // Если Echo Mode выключен (ATE0), то эти 3 строки можно закомментировать
    if (_resp.startsWith(cmd)) {                      // Убираем из ответа дублирующуюся команду
      _resp = _resp.substring(_resp.indexOf("\r\n", cmd.length()) + 2);
    }
    Serial.println(_resp);                            // Дублируем ответ в монитор порта
  }
  return _resp;                                       // Возвращаем результат. Пусто, если проблема
}

/*
.########..########..######..########...#######..##....##..######..########
.##.....##.##.......##....##.##.....##.##.....##.###...##.##....##.##......
.##.....##.##.......##.......##.....##.##.....##.####..##.##.......##......
.########..######....######..########..##.....##.##.##.##..######..######..
.##...##...##.............##.##........##.....##.##..####.......##.##......
.##....##..##.......##....##.##........##.....##.##...###.##....##.##......
.##.....##.########..######..##.........#######..##....##..######..########
*/
String waitResponse() {                               // Функция ожидания ответа и возврата полученного результата
  String _resp = "";                                  // Переменная для хранения результата
  unsigned long _timeout = millis() + 10000;                   // Переменная для отслеживания таймаута (10 секунд)
  while (!SIM800.available() && millis() < _timeout)  {}; // Ждем ответа 10 секунд, если пришел ответ или наступил таймаут, то...
  if (SIM800.available()) {                           // Если есть, что считывать...
    _resp = SIM800.readString();                      // ... считываем и запоминаем
  }
  else {                                              // Если пришел таймаут, то...
    // Serial.println("Timeout...");                     // ... оповещаем об этом и...
  }
  return _resp;                                       // ... возвращаем результат. Пусто, если проблема
}

/*
..######..########.##....##.########......######..##.....##..######.
.##....##.##.......###...##.##.....##....##....##.###...###.##....##
.##.......##.......####..##.##.....##....##.......####.####.##......
..######..######...##.##.##.##.....##.....######..##.###.##..######.
.......##.##.......##..####.##.....##..........##.##.....##.......##
.##....##.##.......##...###.##.....##....##....##.##.....##.##....##
..######..########.##....##.########......######..##.....##..######.
*/
void sendSMS(String phone, String message) {
  sendATCommand("AT+CMGS=\"" + phone + "\"", true);             // Переходим в режим ввода текстового сообщения
  sendATCommand(message + "\r\n" + (String)((char)26), true);   // После текста отправляем перенос строки и Ctrl+Z
}

/*
.########.##....##.########.....
.##.......###...##.##.....##....
.##.......####..##.##.....##....
.######...##.##.##.##.....##....
.##.......##..####.##.....##....
.##.......##...###.##.....##.###
.########.##....##.########..###
*/