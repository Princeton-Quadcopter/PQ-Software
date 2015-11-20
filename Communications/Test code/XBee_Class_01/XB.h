#ifndef XB_h
#define XB_h

#include "Arduino.h"
#include <SoftwareSerial.h>

const unsigned int MAXDATASIZE = 64;

class XB {
    public:
        //template <int N>
        struct genericPacket { // A very generic struct for storing packet data
            byte frameType;
            unsigned int len; // length of contents[]
            char contents[MAXDATASIZE];
            bool goodPacket; // true if start delimiter is 7E
            bool goodCheckSum; // true if checksum is correct
        };

        XB(uint8_t RX, uint8_t TX, int baudrate);
        //int send(XBpacket packet);
        byte send(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]);
        bool anythingtoread();
        void flushSerial();

    private:
        SoftwareSerial serial;

        void sendTransmitRequest(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]);
        genericPacket readNextGenericPacket();
};

#endif