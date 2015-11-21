#include "Arduino.h"
#include "QCXB.h"
#include "QCutil.h" // Do we still need this included, since it's included in XB.cpp?

// Constructor
QCXB::QCXB(uint8_t RX, uint8_t TX, int baudrate)
        : xb(RX, TX, baudrate) {
    // do stuff here
}

byte QCXB::sendPacket(QCpacket packet) {
    // Build XBpacket from QCpacket
    XBpacket xbpkt;
    xbpkt.type = PACKET_SEND;
    xbpkt.options = 0x00;
    xbpkt.destAddr = 0x0000; // IMPORTANT: we need something to do about the destination address. Will QCpacket have this field?
    xbpkt.ID = 0x00; // What should we do with this ID? Note: it's only 1 byte wide
    xbpkt.length = 9 + packet.length;
    
    xbpkt.message[0] = packet.command;
    xbpkt.message[1] = getMSB(packet.ID);
    xbpkt.message[2] = getLSB(packet.ID);
    xbpkt.message[3] = getMSB(packet.length);
    xbpkt.message[4] = getLSB(packet.length);
    copyStr(packet.data, xbpkt.message, 0, 5, packet.length);
    long checksum = 0x12345678;
    xbpkt.message[5 + packet.length] = checksum >> 24;
    xbpkt.message[6 + packet.length] = (checksum >> 16) % (1 << 8);
    xbpkt.message[7 + packet.length] = (checksum >> 8)  % (1 << 8);
    xbpkt.message[8 + packet.length] = checksum % (1 << 8);

    // Transmit XBpacket
    return xb.send(xbpkt);
}