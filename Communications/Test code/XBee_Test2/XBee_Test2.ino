#include "Printers.h"
#include "XBee.h"

XBee xbee = XBee();

uint8_t payload[] = { 'H', 'i' };

XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x40b3d56e);

Tx16Request tx16 = Tx16Request(addr64, payload, sizeof(payload));

void setup()
{
    pinMode(13, OUTPUT);
    // Start the serial port
    Serial.begin(9600);
    // Tell XBee to use Hardware Serial. It's also possible to use SoftwareSerial
    xbee.setSerial(Serial);
}

void loop()
{
    digitalWrite(13, 1);
    xbee.send( tx16 );
    delay(500);
    digitalWrite(13, 0);
    delay(500);
    xbee.receive();
}
