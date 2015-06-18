/**
  * Class definition for MicroBit IO.
  *
  * Represents a single IO pin on the edge connector.
  */

#include "inc/MicroBitIO.h"

/**
  * Constructor. 
  * Create a representation of all given I/O pins on the edge connector
  */
MicroBitIO::MicroBitIO(int MICROBIT_ID_IO_P0, int MICROBIT_ID_IO_P1, int MICROBIT_ID_IO_P2,
                       int MICROBIT_ID_IO_P3, int MICROBIT_ID_IO_P4, int MICROBIT_ID_IO_P5,
                       int MICROBIT_ID_IO_P6, int MICROBIT_ID_IO_P7, int MICROBIT_ID_IO_P8,
                       int MICROBIT_ID_IO_P9, int MICROBIT_ID_IO_P10,int MICROBIT_ID_IO_P11,
                       int MICROBIT_ID_IO_P12,int MICROBIT_ID_IO_P13,int MICROBIT_ID_IO_P14,
                       int MICROBIT_ID_IO_P15,int MICROBIT_ID_IO_P16,int MICROBIT_ID_IO_P19,
                       int MICROBIT_ID_IO_P20) :
    P0 (MICROBIT_ID_IO_P0, MICROBIT_PIN_P0, PIN_CAPABILITY_BOTH),           //P0 is the left most pad (ANALOG/DIGITAL) 
    P1 (MICROBIT_ID_IO_P1, MICROBIT_PIN_P1, PIN_CAPABILITY_BOTH),           //P1 is the middle pad (ANALOG/DIGITAL)
    P2 (MICROBIT_ID_IO_P2, MICROBIT_PIN_P2, PIN_CAPABILITY_BOTH),           //P2 is the right most pad (ANALOG/DIGITAL)
    P3 (MICROBIT_ID_IO_P3, MICROBIT_PIN_P3, PIN_CAPABILITY_BOTH),           //COL1 (ANALOG/DIGITAL) 
    P4 (MICROBIT_ID_IO_P4, MICROBIT_PIN_P4, PIN_CAPABILITY_DIGITAL),        //BTN_A 
    P5 (MICROBIT_ID_IO_P5, MICROBIT_PIN_P5, PIN_CAPABILITY_BOTH),           //COL2 (ANALOG/DIGITAL) 
    P6 (MICROBIT_ID_IO_P6, MICROBIT_PIN_P6, PIN_CAPABILITY_DIGITAL),        //ROW2
    P7 (MICROBIT_ID_IO_P7, MICROBIT_PIN_P7, PIN_CAPABILITY_DIGITAL),        //ROW1 
    P8 (MICROBIT_ID_IO_P8, MICROBIT_PIN_P8, PIN_CAPABILITY_DIGITAL),        //PIN 18
    P9 (MICROBIT_ID_IO_P9, MICROBIT_PIN_P9, PIN_CAPABILITY_DIGITAL),        //ROW3
    P10(MICROBIT_ID_IO_P10,MICROBIT_PIN_P10,PIN_CAPABILITY_BOTH),           //COL3 (ANALOG/DIGITAL) 
    P11(MICROBIT_ID_IO_P11,MICROBIT_PIN_P11,PIN_CAPABILITY_DIGITAL),        //BTN_B
    P12(MICROBIT_ID_IO_P12,MICROBIT_PIN_P12,PIN_CAPABILITY_DIGITAL),        //PIN 20
    P13(MICROBIT_ID_IO_P13,MICROBIT_PIN_P13,PIN_CAPABILITY_DIGITAL),        //SCK
    P14(MICROBIT_ID_IO_P14,MICROBIT_PIN_P14,PIN_CAPABILITY_DIGITAL),        //MISO
    P15(MICROBIT_ID_IO_P15,MICROBIT_PIN_P15,PIN_CAPABILITY_DIGITAL),        //MOSI
    P16(MICROBIT_ID_IO_P16,MICROBIT_PIN_P16,PIN_CAPABILITY_DIGITAL),        //PIN 16
    P19(MICROBIT_ID_IO_P19,MICROBIT_PIN_P19,PIN_CAPABILITY_DIGITAL),        //SCL
    P20(MICROBIT_ID_IO_P20,MICROBIT_PIN_P20,PIN_CAPABILITY_DIGITAL)         //SDA                 
{   
}