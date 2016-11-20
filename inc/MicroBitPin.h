/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef MICROBIT_PIN_H
#define MICROBIT_PIN_H

#include "mbed.h"
#include "MicroBitComponent.h"
                                                        // Status Field flags...
#define IO_STATUS_DIGITAL_IN                0x01        // Pin is configured as a digital input, with no pull up.
#define IO_STATUS_DIGITAL_OUT               0x02        // Pin is configured as a digital output
#define IO_STATUS_ANALOG_IN                 0x04        // Pin is Analog in
#define IO_STATUS_ANALOG_OUT                0x08        // Pin is Analog out
#define IO_STATUS_TOUCH_IN                  0x10        // Pin is a makey-makey style touch sensor
#define IO_STATUS_EVENTBUS_ENABLED          0x80        // Pin is will generate events on change

//#defines for each edge connector pin
#define MICROBIT_PIN_P0                     P0_3        //P0 is the left most pad (ANALOG/DIGITAL) used to be P0_3 on green board
#define MICROBIT_PIN_P1                     P0_2        //P1 is the middle pad (ANALOG/DIGITAL)
#define MICROBIT_PIN_P2                     P0_1        //P2 is the right most pad (ANALOG/DIGITAL) used to be P0_1 on green board
#define MICROBIT_PIN_P3                     P0_4        //COL1 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P4                     P0_5        //COL2 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P5                     P0_17       //BTN_A
#define MICROBIT_PIN_P6                     P0_12       //COL9
#define MICROBIT_PIN_P7                     P0_11       //COL8
#define MICROBIT_PIN_P8                     P0_18       //PIN 18
#define MICROBIT_PIN_P9                     P0_10       //COL7
#define MICROBIT_PIN_P10                    P0_6        //COL3 (ANALOG/DIGITAL)
#define MICROBIT_PIN_P11                    P0_26       //BTN_B
#define MICROBIT_PIN_P12                    P0_20       //PIN 20
#define MICROBIT_PIN_P13                    P0_23       //SCK
#define MICROBIT_PIN_P14                    P0_22       //MISO
#define MICROBIT_PIN_P15                    P0_21       //MOSI
#define MICROBIT_PIN_P16                    P0_16       //PIN 16
#define MICROBIT_PIN_P19                    P0_0        //SCL
#define MICROBIT_PIN_P20                    P0_30       //SDA

#define MICROBIT_PIN_MAX_OUTPUT             1023

#define MICROBIT_PIN_MAX_SERVO_RANGE        180
#define MICROBIT_PIN_DEFAULT_SERVO_RANGE    2000
#define MICROBIT_PIN_DEFAULT_SERVO_CENTER   1500


/**
  * Pin capabilities enum.
  * Used to determine the capabilities of each Pin as some can only be digital, or can be both digital and analogue.
  */
enum PinCapability{
    PIN_CAPABILITY_DIGITAL = 0x01,
    PIN_CAPABILITY_ANALOG = 0x02,
    PIN_CAPABILITY_TOUCH = 0x04,
    PIN_CAPABILITY_AD = PIN_CAPABILITY_DIGITAL | PIN_CAPABILITY_ANALOG,
    PIN_CAPABILITY_ALL = PIN_CAPABILITY_DIGITAL | PIN_CAPABILITY_ANALOG | PIN_CAPABILITY_TOUCH

};

#endif
