#include <SoftwareSerial.h>
//#include <XB.h>

const int LED = 13; // High when reading
const int RX = 2;   // RX pin connected to TX of XBee
const int TX = 3;   // TX pin connected to RX of XBee

//establishes serial connection
SoftwareSerial globalSerial = SoftwareSerial(RX, TX);

void setup()
{
  // set up pins
  pinMode(LED, OUTPUT);
  pinMode(RX, INPUT);
  pinMode(TX, OUTPUT);

  // set up serials
  Serial.begin(9600);
  // start serial connection
  globalSerial.begin(9600);
}

void loop()
{
  digitalWrite(LED, LOW);

  // Wait for read buffer to become nonempty (i.e. wait for message)
  while (!globalSerial.available()) {
    delay(100);
  }

  // Print everything in the read buffer
  while (globalSerial.available()) {
      digitalWrite(LED, HIGH);
      char received = globalSerial.read();
      //Serial.println(received, HEX);
      Serial.print(received);
  }

  Serial.println();

  // Prepare to send a message to the computer
  digitalWrite(LED, LOW);
  delay(100);
  Serial.println("About to write");
  sendTransmitRequest(globalSerial, 1, 0x0000, 0x00, 5, "hello");

  // char payload[] = {0x7E, 0x00, 0x0A, 0x01, 0x01, 0x00, 0x00, 0x00, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0xE9};
  // for (int i = 0; i < 14; i++) {
  //   globalSerial.print(payload[i]);
  // }

  delay(900);
  globalSerial.flush(); // flush the read buffer VERY IMPORTANT!
}

// Sends a transmit request to the XBee to send a message to destAddr: frame ID = fID, len is length of message[];
void sendTransmitRequest(SoftwareSerial& serial, char fID, unsigned int destAddr, char options, char len, char message[])
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
}

// returns the MSB of 2 bytes
char getMSB(int a) {
  return a >> 8;
}

// returns the LSB of sequence of bytes
char getLSB(int a) {
  return a % 256;
}
