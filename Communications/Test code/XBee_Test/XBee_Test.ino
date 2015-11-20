#include <SoftwareSerial.h>

SoftwareSerial globalSerial = SoftwareSerial(2, 3); // RX, TX

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  globalSerial.begin(9600);
}

void loop()                     // run over and over again
{
  digitalWrite(13, LOW);
  while (!globalSerial.available()) {
    delay(100);
  }
  while (globalSerial.available()) {
      digitalWrite(13, HIGH);
      Serial.print((char)globalSerial.read());
  }
  digitalWrite(13, LOW);
  delay(100);
  //sendTransmitRequest(globalSerial, 1, 0x0000, 0x00, 5, "hello");
  Serial.println("About to write");
  char payload[] = {0x7E, 0x00, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xE9};
  for (int i = 0; i < 14; i++) {
    Serial.println((int)payload[i]);
    globalSerial.print(payload[i]);
  }
  delay(900);
}

// Various methods

void sendTransmitRequest(SoftwareSerial& serial, char fID, unsigned int destAddr, char options, char len, char message[])
{
 char checksum = 1 + fID + (destAddr >> 8) + (destAddr % 256) + options;
 
 serial.write(0x7E);
 serial.print(len + 5);
 serial.write(1);
 serial.write(fID);
 serial.print(destAddr);
 serial.write(options);
  for (int i = 0; i < len; i++)
  {
    serial.write(message[i]);
    checksum += message[i];
  }
  //Serial.write(0xff - checksum, HEX);
 serial.write(0xff - checksum);
}
