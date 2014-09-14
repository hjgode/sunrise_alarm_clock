#include "Arduino.h"
#include "../SoftwareSerial/SoftwareSerial.h"

#include "MP3SERIAL.h"

/*
class to control grove _mp3serial serial player
*/

int _iTX=2;
int _iRX=3;

SoftwareSerial _mp3serial(_iRX,_iTX);

const uint8_t END=0xFF;
uint8_t aPauseToggle[]={0x7e,0x02,0xa3,0x7e, END};

//private func to print hex
void printHex8(uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
        Serial.print("0x"); 
        for (int i=0; i<length; i++) { 
          if (data[i]<0x10) {Serial.print("0");} 
          Serial.print(data[i],HEX); 
          Serial.print(" "); 
        }
}

void printHex8(uint8_t data) // prints 8-bit data in hex with leading zeroes
{
  byte buf[1]; buf[0]=data;
  printHex8(buf, 1);
}

//private func to check for OK code 
boolean isOKcode(byte checkCode){
	int countDown=10;
	byte bRead=0;
	while(_mp3serial.available() && countDown>0)
	{
		bRead = _mp3serial.read();
	    if (checkCode==bRead){
	        return true;
	    }
	    else {
	    #ifdef DEBUG
	    	Serial.print("isOKcode="); printHex8(bRead);
	    #endif
	        return false;
	    }
	    delay(10);
	    countDown--;
    }
	return false;
}

//<<constructor>> setup the comm PINs
MP3SERIAL::MP3SERIAL(int iRX, int iTX){
    _iTX=iTX;
    _iRX=iRX;
    _mp3serial=SoftwareSerial(iRX, iTX);
    _mp3serial.begin(9600);
}

//<<destructor>>
MP3SERIAL::~MP3SERIAL(){
    /*nothing to destruct*/
}

//play
bool MP3SERIAL::play(){
	return next();
}

//stop
bool MP3SERIAL::stop(){
	//next
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xA4);
	_mp3serial.write(0x7E);
	 delay(10);
	 return isOKcode(0xA4);
}

//next
bool MP3SERIAL::next(){
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xA5);
	_mp3serial.write(0x7E);
	 delay(10);
	 return isOKcode(0xA5);
}

//next
bool MP3SERIAL::prev(){
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xA6);
	_mp3serial.write(0x7E);
	 delay(10);
	 return isOKcode(0xA6);
}

bool MP3SERIAL::pauseToggle(){
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xA3);
	_mp3serial.write(0x7E);
	 delay(10);
	 return isOKcode(0xA3);
}

//setVolume
//There are 32 volume levels from 00 to 31. 00 is mute, and 31 for the maximum volume.
bool MP3SERIAL::setVolume(byte iVolume){
	if(iVolume>31)
		iVolume=31;
    _mp3serial.write(0x7E);
    _mp3serial.write(0x03);
    _mp3serial.write(0xA7);
    _mp3serial.write(iVolume);
    _mp3serial.write(0x7E);
    delay(10);
    return isOKcode(0xA7);
}


bool MP3SERIAL::setRepeatMode(repeatMode bMode){
	if (((bMode==0x00)|(bMode==0x01)|(bMode==0x02)|(bMode==0x03))==false)
	{
		Serial.println("PlayMode Parameter Error! ");
		return false;
	}
	_mp3serial.write(0x7E);
	_mp3serial.write(0x03);
	_mp3serial.write(0xA9);
	_mp3serial.write((byte)bMode);
	_mp3serial.write(0x7E);
    delay(10);
    return ::isOKcode(0xA9);
}
//setRepeatAll
bool MP3SERIAL::setRepeatAll(){
	return setRepeatMode(repeatAll);
}

//setRepeatSingle
bool MP3SERIAL::setRepeatSingle(){
	return setRepeatMode(repeatSingle);
}

//setRepeatSingle
bool MP3SERIAL::setRepeatNone(){
	return setRepeatMode(repeatNone);
}

bool MP3SERIAL::playByIndex(uint8_t lbyte){
	return playByIndex(0x00, lbyte);
}

//play mp3 by index
//Set the music index to play, the index is decided by the input sequence
//of the music;
//hbyte: the high byte of the index;
//lbyte: the low byte of the index;
bool MP3SERIAL::playByIndex(uint8_t hbyte,uint8_t lbyte)
{
	_mp3serial.write(0x7E);
	_mp3serial.write(0x04);
	_mp3serial.write(0xA0);
	_mp3serial.write(hbyte);
	_mp3serial.write(lbyte);
	_mp3serial.write(0x7E);
    delay(10);
    return isOKcode(0xA0);
}

byte MP3SERIAL::getVolume(){
	byte bVolume=0;
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xC1);
	_mp3serial.write(0x7E);
    delay(10);
    //any data avilable?
	if(_mp3serial.available())
	{
		byte bRead=_mp3serial.read();
		if(bRead==0xC1)
	    {
	    	if(_mp3serial.available()){
	    		bVolume=_mp3serial.read();
	    		return bVolume;
	    	}
		    else{
		        return 0;
		    }
	    }
	    else
	        return 0;
    }
	return 0;
}
// states:
// 0x01=Play
// 0x02=Stop
// 0x03=Pause
byte MP3SERIAL::getState(){
	byte bState=0;
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xC2);
	_mp3serial.write(0x7E);
    delay(10);
    int maxCount=5;
    //any data avilable?
	while(_mp3serial.available() && maxCount>0)
	{
		//answer code?
	    if (0xC2==_mp3serial.read())
	    {
	    	if(_mp3serial.available()){
	    		bState=_mp3serial.read();
	    		return bState;
	    	}
		    else
		        return 0;
	    }
	    else
	        return 0;
	    maxCount--;
    }
	return 0;
}

int MP3SERIAL::getSDNumFiles(){
	byte bAnswer=0;
	byte bAnswer2=0;
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xC4);
	_mp3serial.write(0x7E);
    delay(10);
    int maxCount=5;
    //any data avilable?
	byte bRead=_mp3serial.read();
	Serial.print("mp3serial read=0x");Serial.println(bRead, HEX);
	if (0xC4==bRead)
	{
		if(_mp3serial.available()){
			//read high byte of answer 
			bAnswer=_mp3serial.read();
			Serial.print("mp3serial read answer=0x");Serial.println(bAnswer, HEX);
			//read low byte of answer
			if(_mp3serial.available()){
				bAnswer2=_mp3serial.read();
				  Serial.print("mp3serial read answer2=0x");Serial.println(bAnswer2, HEX);
				return bAnswer*0xff+bAnswer2;
			}
			return 0;
		}
	}
	else{
		return 0;
	}
	return 0;
}

int MP3SERIAL::getSDcurrentFile(){
	byte bAnswerLow=0;
	byte bAnswerHigh=0;
	int iReturn=-1;
	_mp3serial.write(0x7E);
	_mp3serial.write(0x02);
	_mp3serial.write(0xC6);
	_mp3serial.write(0x7E);
    delay(10);
    int maxCount=5;
    //any data avilable?
	while(_mp3serial.available() && maxCount>0)
	{
		//answer code?
	    if (0xC6==_mp3serial.read())
	    {
	    	if(_mp3serial.available()){
	    		bAnswerHigh=_mp3serial.read();
		    	if(_mp3serial.available()){
		    		bAnswerLow=_mp3serial.read();
		    		iReturn=(0xFF*bAnswerHigh)+bAnswerLow;
		    		return iReturn;
		    	}
		    	else
		    		return -1;
	    	}
		    else
		        return -1;
	    }
	    else
	        return -1;
	    maxCount--;
    }
	return 0;
}

void MP3SERIAL::writeCommand(uint8_t* arrayBytes){
    
}

