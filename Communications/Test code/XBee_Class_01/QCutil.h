#ifndef QCutil_h
#define QCutil_h

#include "Arduino.h"

// returns the MSB of 2 bytes
byte getMSB(int a);

// returns the LSB of sequence of bytes
byte getLSB(int a);

// converts two bytes into an unsigned int
unsigned int twoBytesToUInt(byte msb, byte lsb);

// copies string src to string dest, from ind onwards, for length len
void copyStr(char* src, char* dest, int indsrc, int inddest, int len);

#endif