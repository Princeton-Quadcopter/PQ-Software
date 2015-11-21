#include "Arduino.h"
#include "XB.h"
#include "QCutil.h"

// Constructor
QCXB(uint8_t RX, uint8_t TX, int baudrate)
        : xb(RX, TX, baudrate) {
    // do stuff here
}

