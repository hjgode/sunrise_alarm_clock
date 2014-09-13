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

void setup()   /****** SETUP: RUNS ONCE ******/
{
  // Start the built-in serial port, probably to Serial Monitor
  Serial.begin(57600);
  Serial.println("YourDuino.com SoftwareSerial remote loop example");
  Serial.println("Use Serial Monitor, type in upper window, ENTER");
  pinMode(Pin13LED, OUTPUT);    
    myMP3.setVolume(10);
  
}//--(end setup )---


void loop()   /****** LOOP: RUNS CONSTANTLY ******/
{
  digitalWrite(Pin13LED, HIGH);  // Show activity
  if (Serial.available())
  {
    byteReceived = Serial.read();

    Serial.println(byteReceived);

    
    byte bVolume=myMP3.getVolume();
    Serial.print("Volume is: "); Serial.print(bVolume);
    if(bVolume<31)
      bVolume++;
    myMP3.setVolume(bVolume);
    Serial.print("Volume is: "); Serial.print(bVolume);

    myMP3.play(); //play next
    
    byteReceived=myMP3.getState();  // Send byte to Remote Arduino
    Serial.println(byteReceived);

    digitalWrite(Pin13LED, LOW);  // Show activity    
    delay(25);
  }
  
}//--(end main loop )---

/*-----( Declare User-written Functions )-----*/

//NONE
//*********( THE END )***********


