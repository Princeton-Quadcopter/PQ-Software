#include <SoftwareSerial.h>
#include <XB.h>
#include <QCXB.h>
#include "QCutil.h"

const int LED = 13; // High when reading
const int RX = 2;   // RX pin connected to TX of XBee
const int TX = 3;   // TX pin connected to RX of XBee

QCXB ourQCXB = QCXB(RX, TX, 9600);

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
    while (!ourQCXB.available()) {
        delay(10);
    }

    // As long as something is available to read, read packet by packet
    while (ourQCXB.available()) {
        digitalWrite(LED, HIGH);
        QCpacket pkt = ourQCXB.readNextPacket();
        Serial.println("Packet received");
        if (pkt.command != -1) {
            Serial.print("Message: ");
            for (uint16_t i = 0; i < pkt.length; i++) {
                Serial.print(pkt.data[i]);
            }
            Serial.println();
        } else {
            // If we have a bad message, flush the serial and try again
            Serial.print("Bad message... Type ");
            Serial.print(pkt.command);
            ourQCXB.flushSerial();
        }
    }

    // Prepare to send a message to the computer
    digitalWrite(LED, LOW);
    
    // Build message to send
    QCpacket packet;
    packet.command = 0x01;
    packet.ID = 0x1234;
    packet.length = 0x05;
    Serial.println("Transmitting QCpacket...");
    copyStr("abcde", packet.data, 0x0000, 0x01, 5);

    // Send packet
    uint8_t result = ourQCXB.sendPacket(0x0000, 0x01, packet);

    if (result == 0) {
        Serial.println("Successful");
    } else {
        Serial.println("Failed!");
    }

    //ourQCXB.printLeftoverBytes();
    Serial.println();
    delay(100);
}

// note: using XCTU to transmit an 8-byte message at 100 ms intervals (even 500 ms) occasionally causes bad packets
