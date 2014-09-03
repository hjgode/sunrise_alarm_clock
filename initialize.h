// -----------------------------------------------------------------------
// Global definitions.
// -----------------------------------------------------------------------

// ***********************************
// **** Eeprom Adress ****************
// ***********************************
const int alarmHourAddr = 0;
const int alarmMinuteAddr = 1;
const int alarmActiveAddr = 2;
const int snoozeTimeAddr = 3;           
const int fadeInTimeAddr = 4;         
const int useWakeUpLightAddr = 5;
const int alarmIsRadioAddr = 6;
const int radioFrequencyAddr = 7; // 2 bytes

// ***********************************
// **** PIN ASSIGNMENTS **************
// ***********************************
const int buzzerPin = 6; // Tone out

const int lightPin = 9;  // was 5 PWM, for RGB we need two more: 10 and 11
const int lightPin2 = 10;  // was 5 PWM, for RGB we need two more: 10 and 11
const int lightPin3 = 11;  // was 5 PWM, for RGB we need two more: 10 and 11

// ***********************************
// **** Strings/Messages *************
// ***********************************
const char* _Ok = { "OK"};
const char* _Cancel = { "Cancel"};
const char* _On = { "ON "};  
const char* _Off = { "OFF"};   
const char* _Alarm = { "ALARM: "};
const char* _Radio = { "RADIO: "};
const char* _Light = { "LIGHT: "};  

// TODO: Add other reusable strings.

int pauseTime = 1500; // Pause when showing status. TODO: Maybe use other way for delay.
int beepFrequency = 100;

bool alarmActive = false;    // Specifies that the alarm is active.
bool alarmIsRadio = false;   // Specifies if the alarm is sounr or radio.
bool useWakeUpLight = false; // Specifies if the wake-up light is active.

int menuIndex = -1;
long displayTimeOut = 0;		// Counter for auto return to big time.
const int menuItemCount = 6;
const int quickMenuCount = 5;

int lastSecond = -1;

enum Button {
  btnRight, // 0
  btnLeft,  // 1
  btnDown,  // 2
  btnUp,    // 3
  btnEnter, // 4
  btnMenu,  // 5
  noKey = -1,
};

char *menuItems[menuItemCount];
char *quickItems[quickMenuCount];
