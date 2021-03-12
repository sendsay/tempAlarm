/*
.########..########.########.####.##....##.########
.##.....##.##.......##........##..###...##.##......
.##.....##.##.......##........##..####..##.##......
.##.....##.######...######....##..##.##.##.######..
.##.....##.##.......##........##..##..####.##......
.##.....##.##.......##........##..##...###.##......
.########..########.##.......####.##....##.########
*/
enum mode_menu {                    //режимы работы голосового меню
    MODE_MENU,
    MODE_PARAMS,
    MODE_SET_TEMP,
    MODE_SET_GEST,
    MODE_SET_AL_PHONE
};

#define ONE_WIRE_BUS 5              //температурный сенсор здесь

/*
.##.....##....###....########...######.
.##.....##...##.##...##.....##.##....##
.##.....##..##...##..##.....##.##......
.##.....##.##.....##.########...######.
..##...##..#########.##...##.........##
...##.##...##.....##.##....##..##....##
....###....##.....##.##.....##..######.
*/
String _response = "";                              // Переменная для хранения ответов модуля
long lastUpdate = millis();                         // Время последнего обновления
unsigned long updatePeriod   = 60000;               // Проверять каждую минут
bool hasmsg = false;                                // Флаг наличия сообщений к удалению
int mode = MODE_MENU;                               // режим работы во время звонка                  
float currentTemp = 555;                            // текущая теспература

float temp = 0.0;
float gest = 0.0;
String phone = "";

String enterTemp = "";
String enterGest = "";
String enterPhone = "";

boolean printFlag = false;      
String symbol = "";                                 // полученный символ по ДТМФ

String innerPhone = "";                             // Переменная для хранения определенного номера
int phoneindex = -1;   

byte voiceMsgCount = 0;

// #define PIN_EDGE_TEMP 6                             //пин пороговой температуры
// #define PIN_UP_TEMP 7                               //пин температуры выше порога
// #define PIN_DOWN_TEMP 5                             //пин температуры ниже порога
// #define USER_GIST 1                                 //гистерезис ТОЛЬКО ДЛЯ ТЕСТА    //TODO: ИСПОЛЬЗОВТЬ ТОЛЬКО ДЛЯ ТЕСТА  
// float gisteresis = 1.0;                             //гистерезис

boolean tempFlag = true;

/*
.########.##.....##.##....##..######.
.##.......##.....##.###...##.##....##
.##.......##.....##.####..##.##......
.######...##.....##.##.##.##.##......
.##.......##.....##.##..####.##......
.##.......##.....##.##...###.##....##
.##........#######..##....##..######.
*/
String sendATCommand(String cmd, bool waiting);
String waitResponse();
void processingDTMF(String symbol);
void CheckDTMF();
void CheckSMS();
void CheckSendSMS();
void parseSMS(String msg);
void sendSMS(String phone, String message);
void printSerial(String str);
String waitResponse();
void writeStringToEEPROM(int addrOffset, const String &strToWrite);
String readStringFromEEPROM(int addrOffset);
void voiceMsg();
void CheckTemp();
// void pinModeFast(uint8_t pin, uint8_t mode);
bool digitalReadFast(uint8_t pin);

/*
..#######..########........##
.##.....##.##.....##.......##
.##.....##.##.....##.......##
.##.....##.########........##
.##.....##.##.....##.##....##
.##.....##.##.....##.##....##
..#######..########...######.
*/
SoftwareSerial SIM800(2, 3);                          // RX, TX
Ticker waitTimer(voiceMsg, 20000, 0, MILLIS);
Ticker checkTempTimer(CheckTemp, 5000, 0, MILLIS);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

/*
.########.##....##.########.....
.##.......###...##.##.....##....
.##.......####..##.##.....##....
.######...##.##.##.##.....##....
.##.......##..####.##.....##....
.##.......##...###.##.....##.###
.########.##....##.########..###
*/