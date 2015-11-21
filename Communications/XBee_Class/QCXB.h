#ifndef QCXB_h
#define QCXB_h

#include "Arduino.h" // Do we still need this included, since it's included in XB.h?
#include "XB.h"

// struct QCpacket stuff
const int QCPACKET_MAX_DATA_SIZE = 32;

struct QCpacket {
    byte command; // 1 byte command
    unsigned int ID; // 2 byte ID

    unsigned int length; // 2 byte length of data[]
    char data[QCPACKET_MAX_DATA_SIZE]; // 32(could change) byte data field
    
    long hash; // 4 byte hash
};

// class QCXB stuff
class QCXB {
    public:
        QCXB(uint8_t RX, uint8_t TX, int baudrate);
        byte sendPacket(QCpacket packet);
        QCpacket readNextPacket();

    private:
        XB xb;
};

#endif