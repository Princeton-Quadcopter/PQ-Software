#ifndef QCXB_h
#define QCXB_h

#include "Arduino.h" // Do we still need this included, since it's included in XB.h?
#include "XB.h"

// struct QCpacket stuff
const uint16_t QCPACKET_MAX_DATA_SIZE = 32;

struct QCpacket {
    uint8_t command; // 1 byte command
    uint16_t ID; // 2 byte ID

    uint16_t length; // 2 byte length of data[]
    char data[QCPACKET_MAX_DATA_SIZE]; // 32(could change) byte data field
    
    uint32_t hash; // 4 byte hash
};

// class QCXB stuff
class QCXB {
    public:
        QCXB(uint8_t RX, uint8_t TX, uint16_t baudrate);
        
        bool available();
        void flushSerial();

        uint8_t sendPacket(uint16_t destAddr, uint8_t pID, QCpacket packet);
        QCpacket readNextPacket();

        // Temporarily public
        void printLeftoverBytes();

    private:
        XB xb;
};

#endif