#include "MP3SERIAL.h"
#include <SoftwareSerial.h>

/*
class to control grove _mp3serial serial player
*/

SoftwareSerial _mp3serial;

int _iTX=2;
int _iRX=3;

const uint8_t END=0xFF;
uint8_t aPauseToggle[]={0x7e,0x02,0xa3,0x7e, END};

//private func to check for OK code 
boolean isOKcode(){
	while(_mp3serial.available())
	{
	    if (0xA0==_mp3serial.read())
	        return true;
	    else 
	        return false;
    }
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
void MP3SERIAL::play(){
}

//stop
void MP3SERIAL::stop(){
}

//next
void MP3SERIAL::next(){
}

//setVolume
boolean MP3SERIAL::setVolume(int iVolume){
    _mp3serial.write(0x7E);
    _mp3serial.write(0x03);
    _mp3serial.write(0xA7);
    _mp3serial.write(iVolume);
    _mp3serial.write(0x7E);
    delay(10);
    return ::isOKcode();
}

//setRepeatAll
void MP3SERIAL::setRepeatAll(){
}

//setRepeatSingle
void MP3SERIAL::setRepeatSingle(){
}

//play mp3 by index
//Set the music index to play, the index is decided by the input sequence
//of the music;
//hbyte: the high byte of the index;
//lbyte: the low byte of the index;
boolean MP3SERIAL::playByIndex(uint8_t hbyte,uint8_t lbyte)
{
	_mp3serial.write(0x7E);
	_mp3serial.write(0x04);
	_mp3serial.write(0xA0);
	_mp3serial.write(hbyte);
	_mp3serial.write(lbyte);
	_mp3serial.write(0x7E);
    delay(10);
    /*
	while(_mp3serial.available())
	{
	    if (0xA0==_mp3serial.read())
	        return true;
	    else 
	        return false;
    }
    */
    return isOKcode();
}


void MP3SERIAL::writeCommand(uint8_t* arrayBytes){
    
}

