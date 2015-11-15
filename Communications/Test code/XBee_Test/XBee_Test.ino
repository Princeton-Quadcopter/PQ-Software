#include <SoftwareSerial.h>

SoftwareSerial mySerial = SoftwareSerial(2, 3); // RX, TX

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
}

void loop()                     // run over and over again
{
  digitalWrite(13, LOW);
  while (mySerial.available()) {
      digitalWrite(13, HIGH);
      Serial.print((char)mySerial.read());
  }
  digitalWrite(13, LOW);
  delay(100);
  sendTransmitRequest(mySerial, 1, 0x0000, 0x00, 5, "hello");
  delay(900);
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
