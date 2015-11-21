#include "Arduino.h"
#include "XB.h"
#include <SoftwareSerial.h>

XB::XB(uint8_t RX, uint8_t TX, int baudrate)
        : serial(RX, TX, false) {
    // Instantiate serial and begin with given baudrate
    serial.begin(baudrate);
}

bool XB::available() {
    return serial.available();
}

// returns the MSB of 2 bytes
byte getMSB(int a) {
    return a >> 8;
}

// returns the LSB of sequence of bytes
byte getLSB(int a) {
    return a % 256;
}

XB::genericPacket XB::readNextGenericPacket() {
    byte magicByte = serial.read();
    byte temp = serial.read();
    unsigned int length = (temp << 8) + (serial.read());
    byte checksum = 0;
    genericPacket result;

    if (magicByte != 0x7E || length == 0) {
        result.goodPacket = false;
        return result;
    }
    result.goodPacket = true;
    result.len = length - 1;

    result.frameType = serial.read();
    checksum += result.frameType;
    for (int i = 0; i < length - 1; i++) { // TODO: throw error if length is too big
        result.contents[i] = serial.read();
        checksum += result.contents[i];
    }
    result.goodCheckSum = (0xff - checksum == serial.read());

    return result;
}

void XB::flushSerial() {
    serial.flush();
}

// Returns 0x00 if successful
byte XB::send(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]) {
    sendTransmitRequest(fID, destAddr, options, len, message);
    // Wait for transmit status to be received
    while (!serial.available()) {
        delay(10);
    }

    // Parse transmit status
    // TODO: check correct frame type, and correct fID, etc.
    genericPacket result = readNextGenericPacket();
    if (result.len < 2 || !result.goodPacket)
        return -1;
    return result.contents[1];

    //serial.flush();
}

// Sends a transmit request to the XBee to send a message to destAddr: frame ID = fID, len is length of message[];
void XB::sendTransmitRequest(byte fID, unsigned int destAddr, byte options, unsigned int len, char message[]) {
  // constants
  const char magicByte = 0x7E;
  const int lenToData = 5;
  const char transmitRequest = 0x01;

  // len is the length of the message
  char payload[len + 9];
  char checksum = 1 + fID + getMSB(destAddr) + getLSB(destAddr) + options;
  
  // beginning of frame
  payload[0] = magicByte;
  payload[1] = getMSB(len + lenToData);
  payload[2] = getLSB(len + lenToData);
  payload[3] = transmitRequest; // transmit request 16-bit addr
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