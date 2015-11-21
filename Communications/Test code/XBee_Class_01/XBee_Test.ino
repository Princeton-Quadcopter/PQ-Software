#include <SoftwareSerial.h>
#include <XB.h>
#include "QCutil.h"

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
    while (!ourXB.available()) {
        delay(10);
    }
    delay(100);

    // Serial.print(" --> ");
    // Serial.println(ourXB.peek(), HEX);
    //XBpacket pck = ourXB.receiveMessage();
    ourXB.readNextGenericPacket();
    // if (pck.type == PACKET_RECEIVE) {
    //     Serial.println(pck.message);
    // } else {
    //     Serial.print("Bad message... Packet type ");
    //     Serial.println(pck.type);
    // }
    // ourXB.flushSerial();
    
    // while (ourXB.available()) {
    //     digitalWrite(LED, HIGH);
    //     byte received = ourXB.read();
    //     Serial.println(received, HEX);
    //     //Serial.print(received);
    // }

    Serial.println();

    // Prepare to send a message to the computer
    digitalWrite(LED, LOW);
    delay(10);
    
    // build message to send
    XBpacket packet;
    packet.type = PACKET_SEND;
    packet.destAddr = 0x0000;
    packet.ID = 0x01;
    packet.options = 0x00;
    packet.length = 0x05;
    copyStr("hello", packet.message, 0, 0, 5);

    byte result = 0;//ourXB.send(packet);
    //byte result = ourXB.sendRaw(1, 0x0000, 0x00, 5, "hello");
    //byte result = ourXB.sendRaw(1, 0x0000, 0x00, 70, "1234567812345678123456781234567812345678123456781234567812345678hiiiii");
    if (result == 0) {
        Serial.println("Writing successful");
    }
    delay(10);
    //ourXB.flushSerial();
    while (ourXB.available()) {
        digitalWrite(LED, HIGH);
        byte received = ourXB.read();
        Serial.println(received, HEX);
        //Serial.print(received);
    }
}
