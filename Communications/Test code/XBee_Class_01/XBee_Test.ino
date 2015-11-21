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
    while (!ourXB.available()) {
        delay(10);
    }
    delay(100);
    ourXB.flushSerial();

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
    
    // build message to send
    XBpacket packet;
    packet.type = PACKET_SEND;
    packet.destAddr = 0x0000;
    packet.ID = 0x01;
    packet.options = 0x00;
    packet.length = 0x05;
    char msg[6] = "hello";
    packet.message = msg;

    byte result = ourXB.send(packet);
    if (result == 0) {
        Serial.println("Writing successful");
    }
    delay(10);
}
