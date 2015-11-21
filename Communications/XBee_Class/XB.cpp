#include "Arduino.h" // Do we need this included, since it's included in XB.h?
#include "XB.h"
#include <SoftwareSerial.h> // Do we need this included, since it's included in XB.h?
#include "QCutil.h"

const byte FRAME_HEADER_MAGIC_BYTE = 0x7E;
const byte FRAME_TRANSMIT_REQUEST = 0x01;
const byte FRAME_TRANSMIT_STATUS = 0x89;
const byte FRAME_RECEIVE_PACKET = 0x81;

XB::XB(uint8_t RX, uint8_t TX, int baudrate)
        : serial(RX, TX, false) {
    // Instantiate serial and begin with given baudrate
    serial.begin(baudrate);
}

bool XB::available() {
    return serial.available();
}

byte XB::read() {
    return serial.read();
}

byte XB::peek() {
    return serial.peek();
}

XB::genericPacket XB::readNextGenericPacket() {
    // Get and check frame header magic byte
    byte magicByte = serial.read();
    genericPacket result;

    if (magicByte != FRAME_HEADER_MAGIC_BYTE) {
        result.goodPacket = false;
        return result;
    }
    result.goodPacket = true;

    // Get and check length
    byte temp = serial.read();
    unsigned int len = twoBytesToUInt(temp, serial.read());
    result.length = len - 1;
    byte checksum = 0;

    //if (len == 0 || len - 1 >= MAX_DATA_SIZE) {
    if (len == 0) {
        result.goodPacket = false;
        return result;
    }

    // Get frame type and all following bytes until the checksum
    result.frameType = serial.read();
    checksum += result.frameType;
    for (unsigned int i = 0; i < len - 1; i++) {
        if (!serial.available()) {
            result.length = i; // trying to make output of parseMessage more meaningful
            break;
        }
        byte readByte = serial.read();
        result.contents[i] = readByte;
        checksum += readByte;
        delay(10);
    }

    // Check the checksum
    byte expectedCheckSum = serial.read();
    result.goodCheckSum = (0xff - checksum == expectedCheckSum);

    return result;
}

void XB::flushSerial() {
    serial.flush();
}

// Parses a generic packet as if it's a received message frame.
// Returns the packet in XBpacket form; `type' field is:
//   -1 (255) if not good packet
//   -2 (254) if not good checksum
//   -3 (253) if not receive packet
//   -4 (252) if packet length too short
XBpacket XB::parseMessage(genericPacket packet) {
    XBpacket result;

    // All the possible things that could go wrong with this packet
    result.type = PACKET_RECEIVE;
    if (!packet.goodPacket) { result.type = -1; }
    if (!packet.goodCheckSum) { result.type = -2; }
    if (packet.frameType != FRAME_RECEIVE_PACKET) { result.type = -2; }
    if (packet.length < 4) { result.type = -4; }
    // If any of them were bad, stop now and return an error in the form of an XBpacket
    if (result.type != PACKET_RECEIVE) {
        return result;
    }

    // If everything is alright, construct the XBpacket from the generic packet
    result.srcAddr = twoBytesToUInt(packet.contents[0], packet.contents[1]);
    result.RSSI = packet.contents[2];
    result.options = packet.contents[3];
    result.length = packet.length - 4;
    copyStr(packet.contents, result.message, 4, 0, packet.length - 4);

    return result;
}

// Flush everything until the first magic byte, then read the next generic packet
// and parse it as though it were a message packet.
XBpacket XB::receiveMessage() {
    flushUntilStartFrame();
    return parseMessage(readNextGenericPacket());
}

// Helper: flush read buffer until first magic byte
void XB::flushUntilStartFrame() {
    while (serial.peek() != 0x7E && serial.available()) {
        serial.read();
        delay(10);
    }
}

// Helper: flush the read buffer, but print the bytes in hex in the process
void XB::printLeftoverBytes() {
    while (serial.available()) {
        byte received = serial.read();
        Serial.print(received, HEX);
        Serial.print(" ");
    }
    Serial.println();
}

// See XB::sendRaw
byte XB::send(XBpacket packet) {
    return sendRaw(packet.ID, packet.destAddr, packet.options, packet.length, packet.message);
}

// Returns 0x00 if successful; Returns -1 if bad packet or not transmit status or not expected fID;
// Returns other codes if other errors occurred (see Frames Generator on XCTU).
byte XB::sendRaw(byte fID, unsigned int destAddr, byte options, unsigned int len, char* message) {
    sendTransmitRequest(fID, destAddr, options, len, message);
    // Wait for transmit status to be received
    while (!serial.available()) {
        delay(10);
    }

    // Parse transmit status
    genericPacket result = readNextGenericPacket();
    // TODO: if necessary, make an array of received packets so that various frame IDs can be accessed non-chronologically
    if (result.length < 2 || !result.goodPacket || result.frameType != FRAME_TRANSMIT_STATUS)
        return -1;
    if (result.contents[0] != fID)
        return -1;
    return result.contents[1];
}

// Sends a transmit request to the XBee to send a message to destAddr: frame ID = fID, len is length of message[];
void XB::sendTransmitRequest(byte fID, unsigned int destAddr, byte options, unsigned int len, char* message) {
    // constants
    const int lenToData = 5;

    // len is the length of the message
    char payload[len + 9];
    byte checksum = 1 + fID + getMSB(destAddr) + getLSB(destAddr) + options;

    // beginning of frame
    payload[0] = FRAME_HEADER_MAGIC_BYTE;
    payload[1] = getMSB(len + lenToData);
    payload[2] = getLSB(len + lenToData);
    payload[3] = FRAME_TRANSMIT_REQUEST; // transmit request 16-bit addr
    payload[4] = fID;
    payload[5] = getMSB(destAddr);
    payload[6] = getLSB(destAddr);
    payload[7] = options;
    // data
    for (int i = 0; i < len; i++) {
        payload[i+8] = message[i];
        checksum += message[i];
    }
    // checksum
    payload[len + 8] = 0xff - checksum;

    // send the message
    for (int i = 0; i < len + 9; i++) {
    serial.print(payload[i]);
    //Serial.println(payload[i], HEX);
    }
}
