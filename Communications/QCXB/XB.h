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
        struct genericPacket { // A very generic struct for storing packet data
            uint8_t frameType;
            uint16_t length; // length of contents[]
            char contents[MAX_DATA_SIZE];
            bool goodPacket; // true if start delimiter is 7E
            bool goodCheckSum; // true if checksum is correct
        };

        XB(uint8_t RX, uint8_t TX, uint16_t baudrate); // Constructor (specify RX and TX pins and the baudrate)

        bool available(); // Returns true if packets available to read
        void flushSerial(); // Flushes everything in XBee serial

        uint8_t send(XBpacket packet); // Sends XBpacket, and
        uint8_t sendRaw(uint8_t fID, uint16_t destAddr, uint8_t options, uint16_t len, char message[]); // Basically same as send, but not wrapped in an XBpacket
        
        XBpacket receiveMessage(); // Receives and returns next message in the form of an XBpacket. Will reject if it's not a 'received message' packet

        // Temporarily public methods
        void flushUntilStartFrame(); // Same as the equivalent method in QCXB
        void printLeftoverBytes(); // Same as the equivalent method in QCXB

    private:
        SoftwareSerial serial;

        void sendTransmitRequest(uint8_t fID, uint16_t destAddr, uint8_t options, uint16_t len, char message[]); // Send the raw transmit request to the XBee
        genericPacket readNextGenericPacket(); // Reads and returns a generic packet from the XBee serial
        XBpacket parseMessage(genericPacket packet); // Helper function to extract message from genericPacket to put into XBpacket

        // Temporary methods for debugging use
        uint8_t read();
        uint8_t peek();
};

#endif