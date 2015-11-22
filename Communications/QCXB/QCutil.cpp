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

// converts four bytes into an unsigned long
uint32_t fourBytesToULong(uint8_t b3, uint8_t b2, uint8_t b1, uint8_t b0) {
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

// copies string src to string dest, from ind onwards, for length len
void copyStr(char src[], char dest[], uint16_t indsrc, uint16_t inddest, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        dest[inddest + i] = src[indsrc + i];
    }
}