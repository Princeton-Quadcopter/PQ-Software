#ifndef XB_h
#define XB_h

#include "Arduino.h"
#include <SoftwareSerial.h>

// struct XBpacket stuff
const uint8_t PACKET_SEND = 0;
const uint8_t PACKET_RECEIVE = 1;
const unsigned int MAX_DATA_SIZE = 64;

struct XBpacket {
    uint8_t type;
    uint16_t length; // length of message[]
    char message[MAX_DATA_SIZE];
    uint8_t options;
    
    // relevant only for PACKET_SEND
    uint16_t destAddr;
    uint8_t ID;

    // relevant only for PACKET_RECEIVE
    uint16_t srcAddr;
    uint8_t RSSI;
};

// class XB stuff
class XB {
    public:
        //template <int N>
        struct genericPacket { // A very generic struct for storing packet data
            uint8_t frameType;
            uint16_t length; // length of contents[]
            char contents[MAX_DATA_SIZE];
            bool goodPacket; // true if start delimiter is 7E
            bool goodCheckSum; // true if checksum is correct
        };

        XB(uint8_t RX, uint8_t TX, uint16_t baudrate);

        bool available();
        void flushSerial();

        uint8_t send(XBpacket packet);
        uint8_t sendRaw(uint8_t fID, uint16_t destAddr, uint8_t options, uint16_t len, char message[]);
        
        XBpacket receiveMessage();
        XBpacket parseMessage(genericPacket packet); // a helper function: should this be private?

        // TODO: Write various methods and structure this architecture to deal with serial only on the level of packets, and not on the level of bytes. A user of this class should not need to worry about the byte-level workings of this class at all.
        // TODO: e.g. read <--> receiveMessage; available <--> [flushUntilStartFrame and then available]; etc.

        // Temporarily public methods
        uint8_t read();
        uint8_t peek();
        void flushUntilStartFrame();
        void printLeftoverBytes();

    private:
        SoftwareSerial serial;
        genericPacket readNextGenericPacket();
        void sendTransmitRequest(uint8_t fID, uint16_t destAddr, uint8_t options, uint16_t len, char message[]);
        
};

#endif