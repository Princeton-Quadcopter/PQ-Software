#include "Arduino.h"
#include "QCXB.h"
#include "QCutil.h" // Do we still need this included, since it's included in XB.cpp?

// Constructor
QCXB::QCXB(uint8_t RX, uint8_t TX, uint16_t baudrate)
        : xb(RX, TX, baudrate) {
    lastRSSI = 0;
}

// Returns true if there are packets available to read
bool QCXB::available() {
    return xb.available();
}

// Flushes the XBee serial
void QCXB::flushSerial() {
    xb.flushSerial();
}

// Send a QCpacket with specified ID (pID) to the specified destAddr
uint8_t QCXB::sendPacket(uint16_t destAddr, uint8_t pID, QCpacket packet) {
    // Build XBpacket from QCpacket
    XBpacket xbpkt;
    xbpkt.type = PACKET_SEND;
    xbpkt.options = 0x00;
    xbpkt.destAddr = destAddr;
    xbpkt.ID = pID;
    xbpkt.length = 9 + packet.length;
    
    xbpkt.message[0] = packet.command; // Byte 0: command
    xbpkt.message[1] = getMSB(packet.ID); // Bytes 1 and 2: ID
    xbpkt.message[2] = getLSB(packet.ID);
    xbpkt.message[3] = getMSB(packet.length); // Bytes 3 and 4: length of data
    xbpkt.message[4] = getLSB(packet.length);
    copyStr(packet.data, xbpkt.message, 0, 5, packet.length);
    uint32_t checksum = 0x12345678; // TODO: calculate the actual checksum
    xbpkt.message[5 + packet.length] = checksum >> 24; // Last 4 bytes: checksum
    xbpkt.message[6 + packet.length] = (checksum >> 16) % (1 << 8);
    xbpkt.message[7 + packet.length] = (checksum >> 8)  % (1 << 8);
    xbpkt.message[8 + packet.length] = checksum % (1 << 8);

    // Transmit XBpacket
    return xb.send(xbpkt);
}

// Reads the next packet as though it were a QCpacket
QCpacket QCXB::readNextPacket() {
    // Read next packet
    XBpacket received = xb.receiveMessage();

    // Build QCpacket from received information
    QCpacket result;
    result.command = -1;
    if (received.type != PACKET_RECEIVE) // If it's not a valid packet, reject
        return result;

    // Note: this will freak out if we didn't actually receive a QCpacket!
    result.command = (uint8_t)received.message[0];
    result.ID = twoBytesToUInt(received.message[1], received.message[2]);
    result.length = twoBytesToUInt(received.message[3], received.message[4]);
    if (result.length > QCPACKET_MAX_DATA_SIZE)
        result.length = QCPACKET_MAX_DATA_SIZE;
    copyStr(received.message, result.data, 5, 0, result.length);

    // TODO: calculate the checksum for the received bytes and compare with result.hash.
    // Perhaps output a byte for good checksum, and take QCpacket* as an argument that can be edited? Callback functions??
    // FUNDAMENTAL PROBLEM: how should we output multiple values from a function? Pairs? Tuples?
    result.hash = fourBytesToULong(received.message[5 + result.length],
                                   received.message[6 + result.length],
                                   received.message[7 + result.length],
                                   received.message[8 + result.length]);

    // Note: srcAddr and RSSI are going to waste. Any way to output these? Perhaps maintain an instance variable called "last srcAddr" and "last RSSI"? Maybe we should do this for XB too and restructure XBpacket? I.e. no address fields and no RSSI field
    lastRSSI = received.RSSI;
    return result;
}

uint8_t QCXB::getLastRSSI() {
    return lastRSSI;
}

// Debug use: prints all bytes left over in XBee serial
void QCXB::printLeftoverBytes() {
    xb.printLeftoverBytes();
}