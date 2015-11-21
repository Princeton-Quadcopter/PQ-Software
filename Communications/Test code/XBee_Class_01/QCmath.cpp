#include "Arduino.h"
#include "QCMath.h"

// returns the MSB of 2 bytes
byte getMSB(int a) {
    return a >> 8;
}

// returns the LSB of sequence of bytes
byte getLSB(int a) {
    return a % 256;
}

// converts two bytes into an unsigned int
unsigned int twoBytesToUInt(byte msb, byte lsb) {
    return (msb << 8) | lsb;
}