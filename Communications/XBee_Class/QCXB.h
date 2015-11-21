#ifndef QCXB_h
#define QCXB_h

#include "Arduino.h"
#include "XB.h"

// struct QCpacket stuff
struct QCpacket {
    byte command;
    unsigned int ID;

    unsigned int length; // length of data[]
    char data[MAX_DATA_SIZE];
    
    long hash;
};

// class QCXB stuff
class QCXB {
    public:
        QCXB(uint8_t RX, uint8_t TX, int baudrate);

    private:
        XB xb;
};

#endif