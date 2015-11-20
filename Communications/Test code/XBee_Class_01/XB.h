#ifndef XB_h
#define XB_h

#include "Arduino.h"
#include <SoftwareSerial.h>

class XB {
  public:
    XB(uint8_t RX, uint8_t TX, int baudrate);
    void sendTransmitRequest(char fID, unsigned int destAddr, char options, char len, char message[]);
    bool anythingtoread();
  private:
    SoftwareSerial serial;
};

#endif