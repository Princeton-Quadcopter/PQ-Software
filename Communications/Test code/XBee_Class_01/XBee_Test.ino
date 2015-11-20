#include <SoftwareSerial.h>
#include <XB.h>

const int LED = 13; // High when reading
const int RX = 2;   // RX pin connected to TX of XBee
const int TX = 3;   // TX pin connected to RX of XBee

XB ourXB = XB(RX, TX, 9600);

void setup()
{
  // set up pins
  pinMode(LED, OUTPUT);
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);

  // set up serials
  Serial.begin(9600);

  //ourXB = XB(RX, TX, 9600);
}

void loop()
{
  digitalWrite(LED, LOW);

  // Wait for read buffer to become nonempty (i.e. wait for message)
  while (!ourXB.anythingtoread()) {
    delay(10);
  }

  // Print everything in the read buffer
  // while (globalSerial.available()) {
  //     digitalWrite(LED, HIGH);
  //     char received = globalSerial.read();
  //     //Serial.println(received, HEX);
  //     Serial.print(received);
  // }

  Serial.println();

  // Prepare to send a message to the computer
  digitalWrite(LED, LOW);
  delay(10);
  Serial.println("About to write");
  ourXB.sendTransmitRequest(1, 0x0000, 0x00, 5, "hello");
  delay(10);
}
