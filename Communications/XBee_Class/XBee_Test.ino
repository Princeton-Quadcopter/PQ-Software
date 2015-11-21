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
}

void loop()
{
    digitalWrite(LED, LOW);

    // Wait for read buffer to become nonempty (i.e. wait for message)
    while (!ourXB.available()) {
        delay(10);
    }

    // As long as something is available to read, read packet by packet
    while (ourXB.available()) {
        digitalWrite(LED, HIGH);
        XBpacket pck = ourXB.receiveMessage();
        if (pck.type == PACKET_RECEIVE) {
            Serial.print("Message: ");
            for (int i = 0; i < pck.length; i++) {
                Serial.print(pck.message[i]);
            }
            Serial.println();
        } else {
            // If we have a bad message, flush the serial and try again
            Serial.print("Bad message... Type ");
            Serial.print(pck.type);
            Serial.print(" length ");
            Serial.println(pck.length);
            ourXB.flushSerial();
        }
    }

    // Prepare to send a message to the computer
    digitalWrite(LED, LOW);
    delay(100);
    
    // build message to send
    XBpacket packet;
    packet.type = PACKET_SEND;
    packet.destAddr = 0x0000;
    packet.ID = 0x01;
    packet.options = 0x00;
    packet.length = 0x05;
    copyStr("hello", packet.message, 0, 0, 5);

    byte result = -1;
    for (int i = 0; i < 10; i++) {
        result = ourXB.send(packet);
        delay(20);
    }
    if (result == 0) {
        Serial.println("Transmission successful");
    }
    delay(10);
}

// note: using XCTU to transmit an 8-byte message at 100 ms intervals (even 500 ms) occasionally causes bad packets
