#ifndef XB_h
#define XB_h

#include "Arduino.h"
#include <SoftwareSerial.h>

const unsigned int MAX_DATA_SIZE = 64;

// struct XBpacket stuff
const byte PACKET_SEND = 0;
const byte PACKET_RECEIVE = 1;

struct XBpacket {
    byte type;
    
    unsigned int length; // length of message[]
    char message[MAX_DATA_SIZE];
    
    byte ID;
    byte options;

    // relevant only for PACKET_SEND
    unsigned int destAddr;

    // relevant only for PACKET_RECEIVE
    unsigned int srcAddr;
    byte RSSI;
};

// class XB stuff
class XB {
    public:
        //template <int N>
        struct genericPacket { // A very generic struct for storing packet data
            byte frameType;
            unsigned int length; // length of contents[]
            char contents[MAX_DATA_SIZE];
            bool goodPacket; // true if start delimiter is 7E
            bool goodCheckSum; // true if checksum is correct
        };

        XB(uint8_t RX, uint8_t TX, int baudrate);
        byte send(XBpacket packet);
        byte sendRaw(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]);
        // XBpacket receiveMessage();
        bool available();
        void flushSerial();

    private:
        SoftwareSerial serial;

        void sendTransmitRequest(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]);
        genericPacket readNextGenericPacket();
};

#endif