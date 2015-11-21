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