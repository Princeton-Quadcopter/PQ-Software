#ifndef XB_h
#define XB_h

#include "Arduino.h"
#include <SoftwareSerial.h>

enum FrameType {
    TRANSMIT_REQUEST    = 0x01;
    TRANSMIT_STATUS     = 0x89;
    RECEIVE_PACKET      = 0x81;
};

class XBPacket { // A very generic class for storing packet data
    public:
        XBPacket(SoftwareSerial* sSerial); // Construct Generic Packet from Serial
        XBPacket(uint16_t destAddr, uint8_t* message, uint16_t len); // Construct a Transmit Request for Data

        ~XBPacket();

        bool isGoodPacket; // true if checksum and magic byte (0x7E) are correct

        uint8_t getRSSI(); // Return RSSI, or 127 if FrameType != RECEIVE_PACKET
        uint16_t getAddr(); // Return Address, or 2^16-1 if FrameType == TRANSMIT_STATUS

        uint16_t sendPacket(SoftwareSerial *sSerial);

    private:
        uint8_t mFrameType;
        uint16_t mLength; // Length of mContents
        uint8_t* mContents;
        uint8_t mChecksum;
};

// class XB stuff
class XB {
    public:
        XB(uint8_t RX, uint8_t TX, uint16_t baudrate); // Constructor (specify RX and TX pins and the baudrate)

        ~XB();

        bool available(); // Returns true if packets available to read
        void flushSerial(); // Flushes everything in XBee serial

        uint16_t send(XBPacket* packet); // Sends genericPacket      
        XBPacket* receive(); // Receives and returns next message in the form of an XBpacket. Will reject if it's not a 'received message' packet

    private:
        SoftwareSerial *serial;

        // Temporary methods for debugging use
        uint8_t read();
        uint8_t peek();

        // Temporarily public methods
        void flushUntilStartFrame(); // Read until we hit delmiter 0x7E
};

#endif