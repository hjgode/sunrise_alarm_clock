#include <arduino.h>

// see also http://arduino.cc/en/Reference/PROGMEM

// Char 0
prog_uchar newChar0[8] PROGMEM = {
 	B11011, 
	B11011, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000
};

// Char 1
prog_uchar newChar1[8] PROGMEM= {
	B11011, 
	B11011, 
	B00000, 
	B11011, 
	B11011, 
	B00000, 
	B11011, 
	B11011
};

// Char 2
prog_uchar newChar2[8] PROGMEM= {
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B11011, 
	B11011
};

// Char 3
prog_uchar newChar3[8] PROGMEM= {
	B11011, 
	B11011, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B11011, 
	B11011
};

// Char 4
prog_uchar newChar4[8] PROGMEM= { // Dot
	B00000, 
	B00000, 
	B11100, 
	B11100, 
	B11100, 
	B00000, 
	B00000, 
	B00000
};

// Char 5
prog_uchar newChar5[8] PROGMEM= { // Bell
	B00100, 
	B01010, 
	B01010, 
	B01010, 
	B10001, 
	B11111, 
	B00000, 
	B00000
};

// Char 6
prog_uchar newChar6[8] PROGMEM= { // Enter
	B00000, 
	B00001, 
	B00001, 
	B00101, 
	B01101, 
	B11111, 
	B01100, 
	B00100
};

// Char 7
prog_uchar newChar7[8] PROGMEM= { // Blank
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000, 
	B00000
};


