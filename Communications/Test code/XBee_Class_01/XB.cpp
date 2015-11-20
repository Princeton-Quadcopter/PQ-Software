#include "Arduino.h"
#include "XB.h"
#include <SoftwareSerial.h>

XB::XB(uint8_t RX, uint8_t TX, int baudrate) :
    serial(RX, TX, false)
{
    //serial = SoftwareSerial(RX, TX, false);
    serial.begin(baudrate);
}

bool XB::anythingtoread() {
    return serial.available();
}

// returns the MSB of 2 bytes
char getMSB(int a) {
  return a >> 8;
}

// returns the LSB of sequence of bytes
char getLSB(int a) {
  return a % 256;
}

void XB::sendTransmitRequest(char fID, unsigned int destAddr, char options, char len, char message[])
{
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

  delay(10);
  serial.flush();
}