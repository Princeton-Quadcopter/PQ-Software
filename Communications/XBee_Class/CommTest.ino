#include <SoftwareSerial.h>
#include <XB.h>
#include <QCXB.h>
#include "QCutil.h"

const int LED = 13; // High when receiving packets
const int RX = 2;   // RX pin connected to TX of XBee
const int TX = 3;   // TX pin connected to RX of XBee

QCXB ourQCXB = QCXB(RX, TX, 9600);

void setup()
{
    // Set up pins
    pinMode(LED, OUTPUT);
    pinMode(RX, INPUT);
    pinMode(TX, OUTPUT);

    digitalWrite(LED, LOW); // Turn off receiving indicator

    // Set up hardware serial
    Serial.begin(9600);
}

void loop()
{
    // Wait for read buffer to become nonempty (i.e. wait for message)
    while (!ourQCXB.available()) {
        delay(10);
    }

    // As long as something is available to read, read packet by packet
    int nTotal = 0;
    int nSuccessful = 0;
    while (ourQCXB.available()) {
        digitalWrite(LED, HIGH); // Turn on receiving indicator

        // Read the packet and increment count
        QCpacket pkt = ourQCXB.readNextPacket(); // can stuff screw up here if it tries to read a transmit status as a message?
        Serial.println("Packet received:");
        nTotal++;
        if (pkt.command != -1) { // If it's not a bad packet, display message and increment successful count
            nSuccessful++;
            Serial.print("    Message: ");
            for (uint16_t i = 0; i < pkt.length; i++) {
                Serial.print(pkt.data[i]);
                if (i > QCPACKET_MAX_DATA_SIZE) // In the case that it's actually a bad packet, buffer overflow may happen. This halts overflow (hopefully)
                    break;
            }
            Serial.println();
        } else {
            // If we have a bad message, report it, flush the serial and try again
            Serial.print("Bad message... Type ");
            Serial.print(pkt.command);
            ourQCXB.flushSerial();
        }
        delay(10); // Allow new messages to arrive
    }
    digitalWrite(LED, LOW); // Turn off receiving indicator light
    
    // Build message to send
    QCpacket packet;
    packet.command = 0x01;
    packet.ID = 0x1234;
    packet.length = sprintf(packet.data, "%i received; %i successful.", nTotal, nSuccessful);
    //Serial.println(packet.data);

    // Send packet
    Serial.print("Transmitting QCpacket... ");
    uint8_t result = ourQCXB.sendPacket(0x0000, 0x01, packet); // Make second argument anything but 0x00!!

    if (result == 0) { // Report if successful or not
        Serial.println("Successful");
    } else {
        Serial.println("Failed!");
    }

    //ourQCXB.printLeftoverBytes();
    Serial.println(); // Leave an empty line between receive/transmit cycles
    delay(10);
}

// note: using XCTU to transmit a short message at 200 ms intervals can cause some transmissions to "fail" because the new message arrives before transmit status
// However, it can receive messages arbitrarily fast (when it doesn't need to transmit between each one)
//  --> the read buffer could overflow sometimes, causing packets to be missed, though. It's reliable around 20 ms.