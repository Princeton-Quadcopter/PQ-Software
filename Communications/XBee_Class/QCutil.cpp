#include "Arduino.h"
#include "QCutil.h"

// returns the MSB of 2 bytes
uint8_t getMSB(uint16_t a) {
    return a >> 8;
}

// returns the LSB of sequence of bytes
uint8_t getLSB(uint16_t a) {
    return a % 256;
}

// converts two bytes into an unsigned int
uint16_t twoBytesToUInt(uint8_t msb, uint8_t lsb) {
    return (msb << 8) | lsb;
}

// copies string src to string dest, from ind onwards, for length len
void copyStr(char src[], char dest[], uint16_t indsrc, uint16_t inddest, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        dest[inddest + i] = src[indsrc + i];
    }
}