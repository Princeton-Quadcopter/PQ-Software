#ifndef ProtoQC_h
#define ProtoQC_h

#include "Arduino.h" // Do we still need this included, since it's included in XB.h?
#include "QCXB.h"

// class ProtoQC stuff
class ProtoQC {
    public:
        ProtoQC(uint8_t RX, uint8_t TX, uint16_t baudrate); // Constructor (specify RX and TX pins and the baudrate)
        
        int connect(addr);
        int reconnect(addr); // nonblocking equiv of connect
        bool isConnected();

        QCpacket* getPackets(); // checks if QCXB queue is empty
        int acknowledge(QCpacket*);

        void doHeartbeat(sensory information); // this is a really big function: update heartbeat timer, update state, read everything into read queue, etc.; AND IT HAS TO BE NONBLOCKING. 
        // sensory info either as serialized array of char, or a new class

    private:
        state // (enum type), including connected
};

#endif

// lol this is a full network stack