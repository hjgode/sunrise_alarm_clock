//needed by MP3SERIAL
#include <SoftwareSerial.h>
#include <MP3SERIAL.h>


/* YourDuino SoftwareSerialExample1
   - Connect to another Arduino running "YD_SoftwareSerialExample1Remote"
   - Connect this unit Pins 10, 11, Gnd
   - To other unit Pins 11,10, Gnd  (Cross over)
   - Open Serial Monitor, type in top window. 
   - Should see same characters echoed back from remote Arduino

   Questions: terry@yourduino.com 
*/

/*-----( Import needed libraries )-----*/
#include "MP3SERIAL.h"

/*-----( Declare Constants and Pin Numbers )-----*/
#define SSerialRX 2
#define SSerialTX 3
#define Pin13LED  13
/*-----( Declare objects )-----*/
MP3SERIAL myMP3(SSerialRX, SSerialTX); // RX, TX
/*-----( Declare Variables )-----*/
int byteReceived;
int byteSend;
byte bVolume=15;
byte bState=0;

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(57600);
  Serial.println("YourDuino.com SoftwareSerial remote loop example");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");
  pinMode(Pin13LED, OUTPUT);    
    myMP3.setVolume(bVolume);
  bState=myMP3.getState();  
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  digitalWrite(Pin13LED, HIGH);  // Show activity
  if (Serial.available())
  {
    byteReceived = Serial.read();

    Serial.println(byteReceived);
      switch(byteReceived){
        case 13:
          bVolume=myMP3.getVolume();
          Serial.print("Volume is: "); Serial.println(bVolume);
          break;
        case 0x2B: // +
          bVolume=myMP3.getVolume();
          if(bVolume<31)
            myMP3.setVolume(++bVolume);
          break;
        case 0x2D: // -
          bVolume=myMP3.getVolume();
          if(bVolume>0)
            myMP3.setVolume(--bVolume);
          break;
        case 0x6E: // n
          myMP3.next();
          break;
        case 0x70: // p
          myMP3.prev();
          break;
        case 0x73: // h halt
          myMP3.stop();
          break;
        case 0x69: // i
          bState=myMP3.getState();
          dumpState(bState);
          break;
        case 0x31: // 1
        case 0x32: // 1
        case 0x33: // 1
        case 0x34: // 1
        case 0x35: // 1
        case 0x36: // 1
        case 0x37: // 1
        case 0x38: // 1
        case 0x39: // 1
        case 0x40: // 1
          play(byteReceived);
          break;
        default:
        break;
      }// switch
      dumpState();
      digitalWrite(Pin13LED, LOW);  // Show activity    
      delay(25);
      Serial.flush();
    } // if available
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/
void dumpState(){
  bState=myMP3.getState();
 dumpState(bState); 
}

void dumpState(byte state){
  Serial.print("State: ");
  switch (state){
    case 0x01:
      Serial.println("PLAY");;
      break;
    case 0x02:
      Serial.println("STOP");;
      break;
    case 0x03:
      Serial.println("PAUSE");;
      break;
    default:
      Serial.println("n/a");;
      break;
  }
  Serial.print("num files: "); Serial.println(myMP3.getSDNumFiles());
  
}

void play(byte idx){
  byte maxIdx=myMP3.getSDNumFiles();
  maxIdx=10; //test
  if(idx<maxIdx)
    myMP3.playByIndex(idx);
}
//NONE
//*********( THE END )***********


