#ifndef MP3SERIAL_H
#define MP3SERIAL_H
     
#include <Arduino.h> //It is very important to remember this! note that if you are using Arduino 1.0 IDE, change "WProgram.h" to "Arduino.h"

class MP3SERIAL {
    private:
//        boolean isOKcode();
        void writeCommand(uint8_t* arrayBytes);
    public:
        MP3SERIAL(int iRX, int iTX);
        ~MP3SERIAL();
        void play();
        void stop();
        void next();
        boolean setVolume(int iVol);
        void setRepeatAll();
        void setRepeatSingle();
        boolean playByIndex(uint8_t hbyte,uint8_t lbyte);
};
     
#endif

