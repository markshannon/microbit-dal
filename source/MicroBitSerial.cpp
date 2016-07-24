#include "mbed.h"
#include "MicroBit.h"

/**
  * Constructor. 
  * Create an instance of MicroBitSerial
  *
  * @param tx the Pin to be used for transmission
  * @param rx the Pin to be used for receiving data
  *
  * Example:
  * @code 
  * MicroBitSerial serial(USBTX, USBRX);
  * @endcode
  * @note the default baud rate is 115200
  */
MicroBitSerial::MicroBitSerial(PinName tx, PinName rx) : Serial(tx,rx)
{
    this->baud(MICROBIT_SERIAL_DEFAULT_BAUD_RATE);
}
