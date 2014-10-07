//=======================================================
// TODO: save program RAM by using PROGMEM for strings etc!
// see http://arduino.cc/en/Reference/PROGMEM
//=======================================================

// ***************************************************************
// Build uploaded to Arduino: 01-02-2012
// ***************************************************************
// ** Alarm Clock Radio with wake-up light.
// ** Control with 6 key analog keypad or serial.
// Key pad:
// Mode      Menu/Snooze  Enter/Accept  Left         Right        Up           Down
// ---------+------------+------------+------------+------------+------------+------------+
// Running  : Setup      | Info       | Sl. light  | Radio On   | Light On++ | Quick Set  |
// Alarm On : Snooze	 | Alarm Off  |            |            |            |            |
// Light On : Toggle 1-3 |            |            |            | Int. Up    | Int. Down  |
// Radio On : Sleep 1-6  | Radio Off  | Ch. Down   | Ch. Up     | Volume Up  | Volume Dn  |
// ---------+------------+------------+------------+------------+------------+------------+
// Set up   |   Exit     |  Select    |            |            | Scroll up  | Scroll Dn  |
// ---------+------------+------------+------------+------------+------------+------------+
// Set Date : Cancel     |  Store     | Next digit |Prev. Digit | Value Up   | Value Dn   |
// Set Time : ---        |  ---       | ---        | ---        | ---        | ---        |
// Set Alarm: ---        |  ---       | ---        | ---        | ---        | ---        |
// Wake up  : ---        |  ---       |            |            | ---        | ---        |
// Snooze   : ---        |  ---       |            |            | ---        | ---        |
// ---------+------------+------------+------------+------------+------------+------------+
// NOTES/IDEAS/TODO:
//  Software:
//  Use a photo resistor to determine if Wake-Up light should be enabled. 
//		Implement radio.				Ok
//  TODO: Test keypad, settings. Sometimes jumps back.
//		Back light On/Off.				!Ok
//		Moved to quick setup.			Ok
//		Back light on by any key press. Ok
//  Radio Sleep function (Right button) 
//		Radio auto shut off after 0 - 60 (120) minutes. 
//		15 min intervals.				Testing
//		Radio fade out.
//  Check accuracy of RTC and build auto correct function.
//		Soft on for radio.				Ok
//  Sleep light timeout (Left button), fade out. 
//		Use current setting of light (Up button).
//  Quick setup.
//		Back light not 100% ready jet.
//  Radio frequency range: 87.5 - 107.9 MHz
//  ---------------------------------------------------------------------------------------
//  Hardware:
//  [Ok] Implement radio on bread board. 
//  [Ok] Check old PC speakers. Power supply, amplifier. [Using external speakers] 
//  [Ok] Check 'experimental' from schematics. 
//  [Ok] Find proper power supply.
//  [Ok] Test power supply.
//  [Ok] Build all on bread board.
//  [Ok] Calculate and get resistors for LED panel. Depending on power supply and LEDs.
//	Implement photo resistor for wake up light.
// ****************************************************************************************

#define DEBUGMODE 0 // If false, no data is written to eeprom and display is default in details mode.
#define TEST 1      // if 1 show test LED steps

#include "initialize.h"
#include "SpechialChars.h"
#include "cie1931.h"

#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <EEPROM.h>
#include <RTClib.h>

#undef USE_FM_RADIO
#ifdef USE_FM_RADIO
  //fm radio
  #include "Si4735.h"
#endif

#include <SoftwareSerial.h>
SoftwareSerial mp3(2, 3);//modify this with the connector you are using.

#include "LCDoutput.h"
#include "keyboard.h"
#include "helpers.h"

//init for YwRobot LCM1602 IIC V1
// set the LCD address to 0x27 for a 20 chars 4 line display
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Set the LCD I2C address
//LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 RTC;

#ifdef USE_FM_RADIO
  //Create an instance of the Si4735 named radio.
  Si4735 radio;
  //int freq = 10800; 
#endif

// ***************************
// **** Alarm/Light/Radio ****
// ***************************
// Section read from eprom.
int alarmHour =  -1;            // Alarm hour.
int alarmMinute = -1;           // Alarm minute.
int snoozeTime = -1;            // The snooze time.
int fadeInTime = -1;  // The fade in time for wake-up light.

  int radioFrequency = 8890;//9390;      // Specifies the FM frequency/sender.

float intensity = 0;
float stepSize = 0;
bool backLightOn = true; // Default to always on. If 'Off' turn on at any key. Auto off at display timeout.
bool alarmWait = false;	 // Delays the alarm 1 minute after turning off alarm. Avoids retriggering.
int sleepTime = 0; // minutes
int sleepHour = 0; 
int sleepMinute = 0;

// *************************
// **** Status *************
// *************************
//bool defaultDisplayMode = true;  // True when default time display is shown.
bool wakeUpLightOn = false;      // The wake-up light is on.
bool alarmOn = false;            // The alarm is sounding.
bool radioOn = false;            // The radio is on.
bool manualLight = false;        // The light have been set mannually.
float manualLightIntensity = 0;	 // Experimental.
bool displayDateTime = false; 
int lastMinute = -1;
int quickMenuIndex = -1;		 // Counter for QuickMenu index

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// for debug
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
void printFreeRam(){
  Serial.print("Free RAM="); Serial.println(freeRam());
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// setLED
// param int 0-255
// set LEDs with CIE luminance correction
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void setLED(int i){
  //Serial.print("i=");Serial.println(i);
  byte b=(byte) i;
  byte bR=pgm_read_byte_near(redRamp   + i/8) / 4; //the ramps have only 32 values
  byte bG=pgm_read_byte_near(greenRamp + i/8) / 4; //the ramps have only 32 values
  byte bB=pgm_read_byte_near(blueRamp  + i/8) / 4; //the ramps have only 32 values
#if TEST
  //Serial.print("R G B from rampXYZ"); 
  //Serial.print(bR); //Serial.print("/");//Serial.print(bG); //Serial.print("/");Serial.println(bB);
#endif
  analogWrite(lightPin, bR);
  analogWrite(lightPin2, bG);
  analogWrite(lightPin3, bB);
  return;
  /*
  byte bCIE = pgm_read_byte_near(cie+b);
  //Serial.print("b=");Serial.println(b);
  //Serial.print("CIE=");
  Serial.println(bCIE);
  */
 }

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// testLEDsteps 
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void testLEDsteps(){
  return;
  for(int x=0; x<256; x++){
    //Serial.print("x->");Serial.println(cie[x]);
  }
  for(int x=0; x<256; x++){
    setLED(x);
    delay(500);
  }
}

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// Setup 
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void setup () {
  Serial.begin(57600);
  Serial.println("sunRise Alarm Clock v0.01");
  printFreeRam();
  Wire.begin();
  RTC.begin();
  printFreeRam();

  pinMode(lightPin, OUTPUT); 
  pinMode(lightPin2, OUTPUT); 
  pinMode(lightPin3, OUTPUT); 
  pinMode(buzzerPin, OUTPUT); 
  pinMode(A0, INPUT);
  digitalWrite(buzzerPin, LOW);

  printFreeRam();
  setLED(0);
//  analogWrite(lightPin, 0);
//  analogWrite(lightPin2, 0);
//  analogWrite(lightPin3, 0);
  printFreeRam();
  

  if (! RTC.isrunning()) {
	  Serial.println("RTC is NOT running!");
	  // Error Beep
	  tone(buzzerPin, beepFrequency, 500);
	  delay(300); 
	  tone(buzzerPin, beepFrequency, 500);
	  delay(300); 
	  tone(buzzerPin, beepFrequency, 500);
	  delay(300); 
  }

  // --------------------------
  // Initialize LCD Display.
  // --------------------------   
  lcd.backlight();
  lcd.begin(16,2);
//  lcd.init();

  // Set up special characters.
  byte bArray[8];
  //read bytes from PROGMEM to PROG RAM
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar0+k);
  lcd.createChar(0, bArray);//newChar0);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar1+k);
  lcd.createChar(1, bArray);//newChar1);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar2+k);
  lcd.createChar(2, bArray);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar3+k);
  lcd.createChar(3, bArray);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar4+k);
  lcd.createChar(4, bArray);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar5+k);
  lcd.createChar(5, bArray);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar6+k);
  lcd.createChar(6, bArray);
  for(int k=0; k<8; k++)
    bArray[k]=pgm_read_byte_near(newChar7+k);
  lcd.createChar(7, bArray); 
  printFreeRam();

#ifdef USE_FM_RADIO
  // Initialize Radio.
  radio.begin(FM);
  // Needed for 'waking up' the radio.
  delay(200);
  radio.tuneFrequency(radioFrequency);//8890); // 88.9 MHz Radio Nordzee. Is set again after e-eprom read.
  delay(100);
  radio.unmute();
  radio.setVolume(0);
#endif

  lcd.clear();
  lcd.print("Initializing...");
  delay(800);

  // --------------------------
  // Read values from Eeprom.
  // -------------------------- 
  if (DEBUGMODE)
  {
    alarmHour = 20; 
    alarmMinute = 45; 
    alarmActive = true;
    snoozeTime = 9;
    fadeInTime = 15;
    useWakeUpLight = true;
#ifdef USE_FM_RADIO
  radioFrequency = 8890;//10800;
#endif
  }//DEBUGMODE
  else
  {
    lcd.print("EERPOM read...");
    alarmHour = EEPROM.read(alarmHourAddr);
    alarmMinute = EEPROM.read(alarmMinuteAddr);
    alarmActive = (boolean)EEPROM.read(alarmActiveAddr); // Specified if the Alarm in turned on.
    snoozeTime = EEPROM.read(snoozeTimeAddr);
    fadeInTime = EEPROM.read(fadeInTimeAddr);
    useWakeUpLight = (boolean)EEPROM.read(useWakeUpLightAddr); // Specified if the Wake-Up should be used
#ifdef USE_FM_RADIO
    radioFrequency = EEPROMReadInt(radioFrequencyAddr);
#endif
    alarmIsRadio = (bool)EEPROM.read(alarmIsRadioAddr);
  }//DEBUGMODE

  lcd.print("EERPOM verify...");
  // Adjust for illegal values.
  if ((alarmHour < 0) || (alarmHour > 23))
    alarmHour = 0;
  if ((alarmHour < 0) || (alarmMinute > 59))
    alarmMinute = 0;
  if ((snoozeTime < 0) || (snoozeTime > 59))
    snoozeTime = 9; // Default to 9 minutes
  if ((fadeInTime < 0) || (fadeInTime > 59))
    fadeInTime = 30; // Default to 30 minutes
#ifdef USE_FM_RADIO
  if ((radioFrequency < 8750) || (radioFrequency > 10790))
    radioFrequency = 9390;

  radio.tuneFrequency(radioFrequency);
#endif

  stepSize = 255.0f / (float)(fadeInTime * 60.0f); // May be use a 'log' curve.
//Serial.print("stepSize=");Serial.println(stepSize);
  // Dump eeprom to serial
  Serial.println("\n-- Dump eeprom -------------------------------");
  //Serial.print("alarmHour: ");  
  //Serial.print((int)alarmHour);
  //Serial.print(":");
  if (alarmMinute < 10)
    //Serial.print("0");
  Serial.println((int)alarmMinute);

  //Serial.print("alarmActive: ");
  Serial.println(alarmActive?"ON":"OFF");

  //Serial.print("snoozeTime: ");
  Serial.println((int)snoozeTime);

  //Serial.print("fadeInTime: ");
  Serial.println((int)fadeInTime);

  //Serial.print("useWakeUpLight: ");
  Serial.println(useWakeUpLight?"ON":"OFF");

  //Serial.print("alarmIsRadio: ");
  Serial.println(alarmIsRadio?"ON":"OFF");

#ifdef USE_FM_RADIO
  //Serial.print("radioFrequency: ");
  //Serial.print((float)radioFrequency/100.0f);
  Serial.println(" MHz");
#else
  //Serial.print("radio undefined!");
#endif
  Serial.println("----------------------------------------------");

  // --------------------------
  // Create menu items
  // --------------------------
  menuItems[0] = "SET DATE";
  menuItems[1] = "SET TIME";
  menuItems[2] = "SET ALARM";
  menuItems[3] = "FADE IN TIME";
  menuItems[4] = "SNOOZE TIME";
  menuItems[5] = "EXIT MENU"; 

  // --------------------------
  // Create quick menu items
  // --------------------------
  quickItems[0] = "ALARM"; // Enable/Disable alarm.
  quickItems[1] = "ALARM IS";  // Select alarm type; Buzzer or Radio (May be move to Setup, hardly ever used)
  quickItems[2] = "WAKE UP LIGHT"; // Enable/Disable wake up light. (May be move to Setup, only used twice a year)
  quickItems[3] = "BACK LIGHT";  // On/Off. 
  quickItems[4] = "RADIO FREQ";  // FM min -> FM max. 

  //quickItems[] = "";  // 


  // Init Beep
  if (DEBUGMODE) {    
    tone(buzzerPin, beepFrequency, 300);
    delay(500); 
    tone(buzzerPin, beepFrequency, 300);
    delay(500); 
  }

  //digitalWrite(lightPin, manualLight ? HIGH : LOW); // Light off
  if(manualLight)
    setLED(manualLightIntensity);//analogWrite(lightPin, manualLightIntensity);
  else
    setLED(0);// analogWrite(lightPin, 0);
    
  //defaultDisplayMode = false; // Show info display.

  lcd.clear();
  // Rady Beep
  tone(buzzerPin, beepFrequency, 200);
  #if TEST
    Serial.println("testLEDsteps()...");
    testLEDsteps();
  #endif
}  // End setup()

// Currently not used: int lastMenuIndex = -1;

// Currently not used: bool outputEnabled = true; // Makes sure that display is only updated once every second.

// Testing Soft on for radio.
byte volume = 0;
byte maxVolume = 62;

//::::::::::::::::::::::::::::::::::::::::::::::::::::::
// loop 
//::::::::::::::::::::::::::::::::::::::::::::::::::::::
void loop () {

  // -------------------------------------------
  // Read keypad. Handled below display section.
  // ------------------------------------------- 
  //lcd.setCursor (0,0);
  //lcd.print(analogRead(A0));
  //Button key = (Button)getKey(200); // Moved to place of use, see "Keypad, menu and set-up" 2012-01-30
  //Serial.println(key);

  // Get current time.
  DateTime now = RTC.now();
  int second = now.second();
  int minute = now.minute();	

  // Define alarm times
  DateTime alarmTime = DateTime(now.year(), now.month(), now.day(), alarmHour, alarmMinute, 0); // Define alarm time.
  DateTime lightOnTime = alarmTime.unixtime() - (fadeInTime * 60); // Define light on time.					
  DateTime alarmTimeOut = alarmTime.unixtime() + 3600;	// Define alarm timeout time. Sound the alarm for max one hour.

  //===================================================================================================================
  // Display Time and check alarm time. 
  //===================================================================================================================

  if (lastSecond != second)
  {

    if (!DEBUGMODE)
    {
      lcd.noCursor(); // Should move this to appropriate place...
      if (!displayDateTime)
      {
        if (quickMenuIndex < 0)
        {
	   WriteBigTime(lcd, now, alarmActive);
        }
        //else // Show Quick Set up.
        //{
        //  QuickSetUp(lcd, quickMenuIndex);
        //}
        //Serial.println(quickMenuIndex);
      }
      else
      {
        printTime(lcd, 0, 0, now);
        lcd.print(" ");
        lcd.write(alarmActive ? 5 : 7);
        printAlarmTime(lcd, 11,0, alarmTime);
        printDate(lcd, 0, 1, now);
      }
    }
    else
    {
      printTime(lcd, 0, 0, now);
      printAlarmTime(lcd, 0, 1, alarmTime);
      lcd.print(alarmActive ? " On " : " Off");
      printAlarmTime(lcd, 11, 0, lightOnTime);
      printAlarmTime(lcd, 11, 1, alarmTimeOut);
    }

    if (!wakeUpLightOn)
      wakeUpLightOn = ((now.hour() == lightOnTime.hour()) && (now.minute() ==  lightOnTime.minute()) && useWakeUpLight && alarmActive); // NB! TEST

    if (!alarmOn)
    {
      alarmOn = ((now.hour() == alarmTime.hour()) && (now.minute() == alarmTime.minute()) && !alarmWait && alarmActive); // NB! TEST 
    }

    // NB! Not tested
    if (sleepTime > 0)
    {
      bool muteRadio = ((now.hour() == sleepHour) && (now.minute() == sleepMinute));
      if (muteRadio)
      {
        volume = 0;
#ifdef USE_FM_RADIO
        radio.setVolume(volume);       
#endif
        sleepTime = 0;
      }
    }

    if (lastMinute != minute )
    {
      alarmWait = false;
    }
    // Not the most efficient way to do this.
    //if (alarmOn)
    //  alarmOn = !((now.hour() == alarmTimeOut.hour()) && (now.minute() == alarmTimeOut.minute())); // But turn it off if timed out.

    debugPrintTime(now);
    //debugPrintTime(lightOnTime);
    //debugPrintTime(alarmTime);

    // --------------------------
    // Wake up light On 
    // --------------------------
    if (wakeUpLightOn)
    {
      Serial.println("wakeUpLightOn is ON");
      //      
      //      if (intensity > 255)
      //        intensity = 255;
      setLED((int)intensity);
      Serial.println("Intensity=");
      Serial.println(intensity);
      //analogWrite(lightPin, (int)intensity); // Light on. 
      if (intensity < 255){
        intensity += stepSize; // May be use a 'log' curve for intensity
        //setLED uses a CIE table!
      }
    }
    else{
      Serial.println("wakeUpLightOn is off");
    }

    // --------------------------
    // Alarm On
    // --------------------------
    if (alarmOn)
    {
      if (alarmIsRadio)
      {
        //radio.unmute();
        // Testing soft on.
        if (volume <= maxVolume)
        {
#ifdef USE_FM_RADIO
          radio.setVolume(volume);
#endif
          volume++;
          //Serial.println(volume, DEC);
        }
      }
      else{
        if (second % 2 == 0) 
        {
          tone(buzzerPin, beepFrequency, 400);
        }
      }
    }

    //SendStatusToSerial(now);
    lastMinute = minute;
    lastSecond = second;

  } // End -> (lastSecond != now.second())

  //===================================================================================================================
  // Keypad, menu and set-up. 
  //===================================================================================================================
  Button key = (Button)getKey(200);
  if (key > noKey)
  {
    Serial.println(key);

	if (!backLightOn)
		lcd.backlight(); // Turn off again after X seconds. Use display timeout.
	
	displayTimeOut = 0;

    switch (key)
    {
    case btnMenu: // Snooze / Setup menu.
      {
        if (alarmOn) // Snooze
        {
          alarmOn = false;
          // Next alarm time is current time + snoozeTime. 
          // Alarm time is reset on 'Alarm Off' and 'Auto Alarm Off'.
          DateTime tempTime = (now.unixtime() + (snoozeTime * 60)); 
          alarmHour = tempTime.hour();
          alarmMinute = tempTime.minute();

          lcd.clear();
          lcd.print("SNOOZE FOR ");
          lcd.print(snoozeTime);
          lcd.print(" min");

          digitalWrite(buzzerPin, 0);
          if (alarmIsRadio)
          {
            volume = 0;
#ifdef USE_FM_RADIO
            radio.setVolume(volume);
#endif
          }
          delay(pauseTime);
          lcd.clear();
        }
        else // Setup menu
        {
          displayTimeOut = millis();

          ShowSetupMenu(lcd, RTC, now, alarmTime, fadeInTime, snoozeTime);
          alarmHour = alarmTime.hour();
          alarmMinute = alarmTime.minute();
          delay(200);
          lcd.clear();
        }
        break;
      }

    case btnEnter: // Alarm Off / Radio Off / Show Info.
      {
        if (alarmOn) // Alarm Off
        {
          alarmOn = false;
          alarmWait = true; // Disables the alarm for a minute, to avoid retrigger.
          alarmHour = EEPROM.read(alarmHourAddr);
          alarmMinute = EEPROM.read(alarmMinuteAddr);

          lcd.clear();
          lcd.print("ALARM OFF -> 24h");

          digitalWrite(buzzerPin, 0);

          if (alarmIsRadio)
          {
            volume = 0;
#ifdef USE_FM_RADIO
            radio.setVolume(volume);
#endif
          }

          delay(pauseTime);
          lcd.clear();
        }
        else if (radioOn) // Radio Off
        {
          radioOn = false;
          volume = 0;
#ifdef USE_FM_RADIO
          radio.setVolume(volume);
#endif
          sleepTime = 0;
        }
        else // Show Info
        {
          displayTimeOut = millis();
          displayDateTime = !displayDateTime;
          lcd.clear();
        }
        break;
      }

    case btnLeft: // Back light On/Off. TODO: Move this to Quick Setup.
      {
        //backLightOn = !backLightOn;
        //backLightOn?lcd.backlight():lcd.noBacklight();
        break;
      }

    case btnRight: // Radio On / Set sleep time.
      {
        if (radioOn) // Go through sleep times. // NB! Not tested.
        {
          sleepTime += 15;
          if (sleepTime > 90)
            sleepTime = 0;

          DateTime sleepTimeLeft = now.unixtime() + (sleepTime * 60); // Define radio off time.			
          sleepHour = sleepTimeLeft.hour();
          sleepMinute = sleepTimeLeft.minute();
        }
        else
        {
          radioOn = true;
        }        

        lcd.clear();
        lcd.print("RADIO ");
        radioOn?lcd.print("ON"):lcd.print("OFF");
        lcd.setCursor(0, 1);
        lcd.print("AUTO OFF ");
        if (sleepTime > 60)
        {
          lcd.print(sleepTime / 60);
          lcd.print("h");
          lcd.print(sleepTime - 60);
          lcd.print("m");
        }
        else if (sleepTime == 0)
        {
          lcd.print("Disabled");
        }
        else
        {
          lcd.print(sleepTime);
          lcd.print(" min");
        }

#ifdef USE_FM_RADIO
        // Mute/Unmute radio
        radioOn ? radio.setVolume(maxVolume) : radio.setVolume(0);
#endif
        delay(pauseTime);
        lcd.clear();

        break;
      }//btnRight

	case btnUp: // Turn off wake-up light.
		{
  Serial.println("btnUP");
			if (wakeUpLightOn)
			{
  Serial.println("### wakeUpLightOn manual switch off");
				manualLightIntensity = 0;
				wakeUpLightOn = false; 
			}
			else
			{
  Serial.println("### wakeUpLightOn manual light switch ON");
				manualLightIntensity += 1; //2.1f;
				if (manualLightIntensity > 4) //6.34f)
					manualLightIntensity = 0;
  Serial.println("+++ manualLightIntensity = ");
  Serial.println(manualLightIntensity);
			}
                        setLED((int)manualLightIntensity*60); //0-4 => 0-240
			//analogWrite(lightPin, (int)pow(manualLightIntensity, 3)); // Divide steps to 'low', 'mid', 'high'
			break;
		}

	case btnDown: // Quick Set-up
		{
			// NOTE: Should save to e-eprom.
#ifdef USE_FM_RADIO
			QuickSetUp(lcd, alarmActive, useWakeUpLight, alarmIsRadio, backLightOn, radio, radioFrequency);
#else
			QuickSetUp(lcd, alarmActive, useWakeUpLight, alarmIsRadio, backLightOn, radioFrequency);
#endif
			backLightOn?lcd.backlight():lcd.noBacklight();

			break;
		}
    }
  }

  // ====================================
  // Return to default display mode         
  // ====================================  
  if (((millis() - displayTimeOut) > 5000) && (displayDateTime || (quickMenuIndex >= 0)))
  {
    displayDateTime = false;
    menuIndex = -1;
    quickMenuIndex = -1;
    lcd.clear();

	if (!backLightOn) // Turn off back light 
		lcd.noBacklight();
	//backLightOn ? lcd.backlight() : lcd.noBacklight(); // Turn off back light 

    //Serial.println("XXX");
  }

  //delay(50);
/*
// Serial control
  int freq;
  bool valid;
  if(Serial.available()>0){

    //Decide what to do based on the character received.
    switch(Serial.read()){
      //If we get the number 8, turn the volume up.
    case '8':
      //Serial.print("Volume: ");
#ifdef USE_FM_RADIO
      radio.volumeUp();
      //lcd.print(radio.getVolume());
      Serial.println(map(radio.getVolume(), 0, 62, 0, 100));
#else
      Serial.println("00");
#endif      
      break;
      //If we get the number 2, turn the volume down.
    case '2':
      //lcd.print(radio.getVolume());
      //Serial.print("Volume: ");
#ifdef USE_FM_RADIO
      radio.volumeDown();
      Serial.println(map(radio.getVolume(), 0, 62, 0, 100));
#else      
      Serial.println("00");
#endif      
      break;
      //If we get the number 4, seek down to the next channel in the current bandwidth (wrap to the top when the bottom is reached).
    case '4':
      //Serial.print("Frequency: ");
#ifdef USE_FM_RADIO
      radio.seekDown();
      delay(10);
      freq = radio.getFrequency(valid);
      Serial.println((float)(freq/100.0f));
#else
      Serial.println("99.99");
#endif
      //printFrequency(freq);
      break;
      //If we get the number 6, seek up to the next channel in the current bandwidth (wrap to the bottom when the top is reached).
    case '6':
      //Serial.print("Frequency: ");
#ifdef USE_FM_RADIO
      radio.seekUp();
      delay(10);
      freq = radio.getFrequency(valid);
      Serial.println((float)(freq/100.0f));
#else      
      Serial.println("-99.99");
      //printFrequency(freq);
#endif
      break;
      //If we get the letter m, mute the radio.
    case 'M':
    case 'm':
      volume = 0;
#ifdef USE_FM_RADIO
      radio.setVolume(volume);
#endif      
      break;
      //If we get the letter u, unmute the radio.
    case 'U':
    case 'u':
#ifdef USE_FM_RADIO
      //radio.unmute();
      radio.setVolume(maxVolume);
#endif
      break;
      //If we get the letter s, print the current status of the radio.
    case 'S':
    case 's':
#ifdef USE_FM_RADIO
      //Serial.print((unsigned char)radio.getStatus());
#else 
      Serial.println("no radio");
#endif      
      break;
    default:
      break;
    }
    //
  }   
*/

} // End -> Loop()

void SendStatusToSerial(DateTime &dateTime)
{
  //byte radioFrequencyLow = ;
  //byte radioFrequencyHigh = ;
  Serial.write(120);
  byte data[15];// = {
  data[0] = (byte)(dateTime.year()-2000); 
  data[1] = (byte)dateTime.month();  
  data[2] = (byte)dateTime.day(); 
  data[3] = (byte)dateTime.hour(); 
  data[4] = (byte)dateTime.minute(); 
  data[5] = (byte)dateTime.second();
  data[6] = (byte)alarmHour;  
  data[7] = (byte)alarmMinute;  
  data[8] = (byte)snoozeTime; 
  data[9] = (byte)fadeInTime;  
  data[10] = (byte)alarmActive; 
  data[11] = (byte)alarmIsRadio;  
  data[12] = (byte)useWakeUpLight;
  data[13] = (byte)((radioFrequency >> 0) & 0xFF);
  data[14] = (byte)((radioFrequency >> 8) & 0xFF);
  //};

  Serial.write(data, 15);

  //  //Serial.print(dateTime.year(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(dateTime.month(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(dateTime.day(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(dateTime.hour(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(dateTime.minute(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(dateTime.second(), DEC);  
  //  //Serial.print(",");
  //  //Serial.print(alarmHour, DEC);  
  //  //Serial.print(",");
  //  //Serial.print(alarmMinute, DEC);  
  //  //Serial.print(",");
  //  //Serial.print(snoozeTime, DEC);  
  //  //Serial.print(",");
  //  //Serial.print(fadeInTime, DEC);  
  //  //Serial.print(",");
  //  //Serial.print((int)alarmActive, DEC);  
  //  //Serial.print(",");
  //  //Serial.print((int)alarmIsRadio, DEC);  
  //  //Serial.print(",");
  //  //Serial.print((int)useWakeUpLight, DEC);  
  //  //Serial.print(",");
  //  Serial.println(radioFrequency, DEC);
}

// Beep.
//void beep(int length)
//{
//  analogWrite(buzzerPin, 80);
//  delay(length);
//  analogWrite(buzzerPin, 0);
//}

void debugPrintTime(DateTime now)
{
  //Serial.print("Time: ");
  //Serial.print(now.year(), DEC);
  //Serial.print("/");
  //Serial.print(now.month(), DEC);
  //Serial.print("/");
  //Serial.print(now.day(), DEC);
  //Serial.print(" ");
  if (now.hour() < 10)
    //Serial.print("0");
  //Serial.print(now.hour(), DEC);
  //Serial.print(":");
  if (now.minute() < 10)
    //Serial.print("0");
  //Serial.print(now.minute(), DEC);
  //Serial.print(":");
  if (now.second() < 10)
    //Serial.print("0");
  //Serial.print(now.second(), DEC);
  //Serial.print(", alarmOn: ");
  //Serial.print(alarmOn, DEC);

  Serial.println();
}




















