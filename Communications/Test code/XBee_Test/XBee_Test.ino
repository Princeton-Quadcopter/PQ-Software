#include <SoftwareSerial.h>

SoftwareSerial mySerial = SoftwareSerial(2, 3); // RX, TX

void setup()
{
  pinMode(13, OUTPUT);
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.listen();
}

void loop()                     // run over and over again
{
  // digitalWrite(13, HIGH);
  // while (!mySerial.available()) {
  //   Serial.print("Stuck...\n");
  //   if (mySerial.isListening()) {
  //     Serial.print("but listening\n");
  //   }
    
  //   delay(500);
  // }
  // while (mySerial.available()) {
  //     Serial.print((char)mySerial.read());
  //     //Serial.print(" ");
  //     //delay(10);
  // }
  // mySerial.flush();
  // digitalWrite(13, LOW);
  // delay(1000);
  Serial.print("About to write\n");
  sendTransmitRequest(mySerial, 1, 0x0001, 0x00, 5, "hello");
  //mySerial.flush();
  while (!mySerial.available()) {
    Serial.print("Stuck...\n");
    delay(500);
  }
  while (mySerial.available()) {
      Serial.print((char)mySerial.read());
      //Serial.print(" ");
      //delay(10);
  }
  delay(10000);
}

// Various methods

void sendTransmitRequest(SoftwareSerial serial, char fID, unsigned int destAddr, char options, char len, char message[])
{
 char checksum = 1 + fID + (destAddr >> 8) + (destAddr % 256) + options;
 
 serial.print(0x7E);
 serial.print(len + 5);
 serial.print(1);
 serial.print(fID);
 serial.print(destAddr);
 serial.print(options);
  for (int i = 0; i < len; i++)
  {
    serial.print(message[i]);
    checksum += message[i];
  }
  //Serial.print(0xff - checksum, HEX);
  serial.println(0xff - checksum);
}
