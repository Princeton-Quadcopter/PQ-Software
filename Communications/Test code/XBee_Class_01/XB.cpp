#include "Arduino.h"
#include "XB.h"
#include <SoftwareSerial.h>
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
    byte magicByte = serial.read();
    byte temp = serial.read();
    unsigned int len = twoBytesToUInt(temp, serial.read());
    byte checksum = 0;
    genericPacket result;

    if (magicByte != FRAME_HEADER_MAGIC_BYTE || len == 0 || len - 1 >= MAX_DATA_SIZE) {
    //if (magicByte != FRAME_HEADER_MAGIC_BYTE || len == 0) {
        result.goodPacket = false;
        return result;
    }
    result.goodPacket = true;
    result.length = len - 1;

    result.frameType = serial.read();
    checksum += result.frameType;
    Serial.println(len - 1);
    for (unsigned int i = 0; i < len - 1; i++) {
        byte readByte = serial.read();
        result.contents[i] = readByte;
        checksum += readByte;
        Serial.print(i);
        Serial.print(": ");
        Serial.println(readByte, HEX);
    }
    byte expectedCheckSum = serial.read();
    result.goodCheckSum = (0xff - checksum == expectedCheckSum);
    Serial.println("Checksums: exp vs reality");
    Serial.println(0xff - checksum, HEX);
    Serial.println(expectedCheckSum, HEX);

    return result;
}

void XB::flushSerial() {
    serial.flush();
}

// Parses a generic packet as if it's a received message frame.
// Returns the packet in XBpacket form; if it's not a receive message frame, the `type' field is -1.
XBpacket XB::parseMessage(genericPacket packet) {
    XBpacket result;

    if (!packet.goodPacket || !packet.goodCheckSum) {
        result.type = -1;
        return result;
    }

    Serial.print("Frame type ");
    Serial.println(packet.frameType, HEX);
    if (packet.frameType != FRAME_RECEIVE_PACKET || packet.length < 4) {
        result.type = -1;
        return result;
    }

    result.type = PACKET_RECEIVE;
    result.srcAddr = twoBytesToUInt(packet.contents[0], packet.contents[1]);
    result.RSSI = packet.contents[2];
    result.options = packet.contents[3];
    result.length = packet.length - 4;
    copyStr(packet.contents, result.message, 0, 4, packet.length - 4);

    return result;
}

XBpacket XB::receiveMessage() {
    // while (serial.peek() != 0x7E) {
    //     serial.read();
    //     delay(10);
    // }
    return parseMessage(readNextGenericPacket());
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
  char checksum = 1 + fID + getMSB(destAddr) + getLSB(destAddr) + options;
  
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
