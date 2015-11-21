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
            Serial.print("Bad message... Type ");
            Serial.print(pck.type);
            Serial.print(" length ");
            Serial.println(pck.length);
        }

        while (ourXB.available()) {
            byte received = ourXB.read();
            Serial.print(received, HEX);
            Serial.print(" ");
            //Serial.print(received);
        }
        Serial.println();
    }

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

    byte result = ourXB.send(packet);
    if (result == 0) {
        Serial.println("Writing successful");
    }
    delay(10);
    //ourXB.flushSerial();
    if (ourXB.available()) {
        Serial.println("Miscellaneous bytes left in read buffer:");
        while (ourXB.available()) {
            byte received = ourXB.read();
            Serial.print(received, HEX);
            Serial.print(" ");
            //Serial.print(received);
        }
        Serial.println();
    }
}
