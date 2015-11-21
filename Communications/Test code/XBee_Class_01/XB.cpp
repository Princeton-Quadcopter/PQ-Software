#include "Arduino.h"
#include "XB.h"
#include <SoftwareSerial.h>
#include "QCMath.h"

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

XB::genericPacket XB::readNextGenericPacket() {
    byte magicByte = serial.read();
    byte temp = serial.read();
    unsigned int len = (temp << 8) + (serial.read());
    byte checksum = 0;
    genericPacket result;

    if (magicByte != FRAME_HEADER_MAGIC_BYTE || len == 0 || len - 1 >= MAX_DATA_SIZE) {
        result.goodPacket = false;
        return result;
    }
    result.goodPacket = true;
    result.length = len - 1;

    result.frameType = serial.read();
    checksum += result.frameType;
    for (int i = 0; i < len - 1; i++) {
        result.contents[i] = serial.read();
        checksum += result.contents[i];
    }
    result.goodCheckSum = (0xff - checksum == serial.read());

    return result;
}

void XB::flushSerial() {
    serial.flush();
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
