#ifndef MICROBIT_SERIAL_H
#define MICROBIT_SERIAL_H

#include "mbed.h"

#define MICROBIT_SERIAL_DEFAULT_BAUD_RATE 115200
#define MICROBIT_SERIAL_BUFFER_SIZE 20

#define MICROBIT_SERIAL_DEFAULT_EOF '\n'

/**
  * Class definition for MicroBitSerial.
  *
  * Represents an instance of Serial which accepts micro:bit specific data types.
  */
class MicroBitSerial : public Serial
{

    public:
    
    /**
      * Constructor. 
      * Create an instance of MicroBitSerial
      * @param sda the Pin to be used for SDA
      * @param scl the Pin to be used for SCL
      * Example:
      * @code 
      * MicroBitSerial serial(USBTX, USBRX);
      * @endcode
      */
    MicroBitSerial(PinName tx, PinName rx);

};

#endif
