#ifndef MP3SERIAL_H
#define MP3SERIAL_H
     
#include <Arduino.h> //It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h"

enum repeatMode{
	repeatNone=0,
	repeatSingle=0x01,
	repeatAll=0x02,
	repeatRandom=0x03,
};

enum playState{
	playStatePlay=0x01,
	playStateStop=0x02,
	playStatePause=0x03,
};

class MP3SERIAL {
    private:
//        boolean isOKcode();
        void writeCommand(uint8_t* arrayBytes);
    public:
        MP3SERIAL(int iRX, int iTX);
        ~MP3SERIAL();
        bool play();
        bool stop();
        bool next();
        bool prev();
        bool pauseToggle();
        bool setVolume(byte iVol);
        byte getVolume();
        byte getState();
        int getSDNumFiles();
        int getSDcurrentFile();
        bool setRepeatMode(repeatMode bMode);
        bool setRepeatAll();
        bool setRepeatSingle();
        bool setRepeatNone();
        bool playByIndex(uint8_t lbyte);
        bool playByIndex(uint8_t hbyte,uint8_t lbyte);
};
     
#endif

