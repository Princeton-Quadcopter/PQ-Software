#include <XBee.h>

XBee xbee = XBee();

void setup()
{
    pinMode(13, OUTPUT);
    // XBee serial connection
    xbee.begin(9600);

    // Computer serial connection
    Serial.begin(9600);
}

void loop()                     // run over and over again
{
    digitalWrite(13, LOW);
    while (mySerial.available()) {
        digitalWrite(13, HIGH);
        Serial.print((char)mySerial.read());
        //delay(10);
    }
    digitalWrite(13, LOW);
    delay(1000);
    sendTransmitRequest(mySerial, 1, 0x0000, 0x00, 5, "hello");
    delay(1000);
}

// Various methods

void sendTransmitRequest(SoftwareSerial serial, char fID, unsigned int destAddr, char options, char len, char message[])
{
//  char checksum = 1 + fID + (destAddr >> 8) + (destAddr % 256) + options;
//  
//  serial.print(0x7E);
//  serial.print(len + 5);
//  serial.print(1);
//  serial.print(fID);
//  serial.print(destAddr);
//  serial.print(options);
for (int i = 0; i < len; i++)
{
    serial.print(message[i]);
    //checksum += message[i];
}
  //Serial.print(0xff - checksum, HEX);
//  serial.print(0xff - checksum);
}
