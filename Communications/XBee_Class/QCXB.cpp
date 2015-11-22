#include "Arduino.h"
#include "QCXB.h"
#include "QCutil.h" // Do we still need this included, since it's included in XB.cpp?

// Constructor
QCXB::QCXB(uint8_t RX, uint8_t TX, uint16_t baudrate)
        : xb(RX, TX, baudrate) {
    // do stuff here
}

bool QCXB::available() {
    return xb.available();
}

void QCXB::flushSerial() {
    xb.flushSerial();
}

void QCXB::printLeftoverBytes() {
    xb.printLeftoverBytes();
}

QCpacket QCXB::readNextPacket() {
    XBpacket received = xb.receiveMessage();
    delay(100);
    QCpacket result;
    result.command = -1;
    if (received.type != PACKET_RECEIVE)
        return result;

    result.command = (uint8_t)received.message[0];
    result.ID = twoBytesToUInt(received.message[1], received.message[2]);
    if (result.length > QCPACKET_MAX_DATA_SIZE)
        result.length = QCPACKET_MAX_DATA_SIZE;
    copyStr(received.message, result.data, 5, 0, result.length);

    // TODO: calculate the checksum for the received bytes and compare with result.hash.
    // Perhaps output a byte for good checksum, and take QCpacket* as an argument that can be edited? Callback functions??
    // FUNDAMENTAL PROBLEM: how should we output multiple values from a function? Pairs? Tuples?
    result.hash = (received.message[5 + result.length] << 24) |
                  (received.message[6 + result.length] << 16) |
                  (received.message[7 + result.length] << 8) |
                  (received.message[8 + result.length]);

    // Note: srcAddr and RSSI are going to waste. Any way to output these? Perhaps maintain an instance variable called "last srcAddr" and "last RSSI"? Maybe we should do this for XB too and restructure XBpacket? I.e. no address fields and no RSSI field
    return result;
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
    
    xbpkt.message[0] = packet.command;
    xbpkt.message[1] = getMSB(packet.ID);
    xbpkt.message[2] = getLSB(packet.ID);
    xbpkt.message[3] = getMSB(packet.length);
    xbpkt.message[4] = getLSB(packet.length);
    copyStr(packet.data, xbpkt.message, 0, 5, packet.length);
    uint32_t checksum = 0x12345678; // TODO: calculate the actual checksum
    xbpkt.message[5 + packet.length] = checksum >> 24;
    xbpkt.message[6 + packet.length] = (checksum >> 16) % (1 << 8);
    xbpkt.message[7 + packet.length] = (checksum >> 8)  % (1 << 8);
    xbpkt.message[8 + packet.length] = checksum % (1 << 8);

    // Transmit XBpacket
    return xb.send(xbpkt);
}