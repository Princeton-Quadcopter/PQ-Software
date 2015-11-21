#ifndef QCMath_h
#define QCMath_h

#include "Arduino.h"

// returns the MSB of 2 bytes
byte getMSB(int a);

// returns the LSB of sequence of bytes
byte getLSB(int a);

// converts two bytes into an unsigned int
unsigned int twoBytesToUInt(byte msb, byte lsb);

#endif