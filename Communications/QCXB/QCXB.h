#ifndef QCXB_h
#define QCXB_h

#include "Arduino.h" // Do we still need this included, since it's included in XB.h?
#include "XB.h"

// struct QCpacket stuff
const uint16_t QCPACKET_MAX_DATA_SIZE = 32;

class QCpacket {
    public:
        QCpacket(genericPacket* pkt);

        uint8_t command; // 1 byte command
        uint16_t ID; // 2 byte ID

        uint16_t length; // 2 byte length of data[]
        char* data; // dynamic size data field
        
        uint32_t hash; // 4 byte hash
};

// class QCXB stuff
class QCXB {
    public:
        QCXB(uint8_t RX, uint8_t TX, uint16_t baudrate); // Constructor (specify RX and TX pins and the baudrate)
        
        bool available(); // Returns true if packets available to read
        void flushSerial(); // Flushes everything in XBee serial

        uint8_t sendPacket(QCpacket* packet); // Send QCpacket
        uint8_t readNextPacket(QCpacket* ret); // Reads and returns next QCpacket

        uint8_t getLastRSSI(); // Returns RSSI of last received packet

        // Temporarily public
        void printLeftoverBytes();

    private:
        XB xb; // Instance of XB for internal use within QCXB
        uint8_t lastRSSI; // Signal strength of last received packet
};

#endif