#include "Arduino.h"
#include "QCutil.h"

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

// copies string src to string dest, from ind onwards, for length len
void copyStr(char* src, char* dest, int indsrc, int inddest, int len) {
    for (int i = 0; i < len; i++) {
        dest[inddest + i] = src[indsrc + i];
    }
}