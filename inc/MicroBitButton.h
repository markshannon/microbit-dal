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

#ifndef MICROBIT_BUTTON_H
#define MICROBIT_BUTTON_H

#include "mbed.h"
#include "MicroBitComponent.h"
#include "MicroBitEvent.h"

//TODO: When platform is built for MB2 - pins will be defined by default, these will change...
#define MICROBIT_PIN_BUTTON_A                   P0_17
#define MICROBIT_PIN_BUTTON_B                   P0_26
#define MICROBIT_PIN_BUTTON_RESET               P0_19

#define MICROBIT_BUTTON_EVT_DOWN                1
#define MICROBIT_BUTTON_EVT_UP                  2
#define MICROBIT_BUTTON_EVT_CLICK               3
#define MICROBIT_BUTTON_EVT_LONG_CLICK          4
#define MICROBIT_BUTTON_EVT_HOLD                5
#define MICROBIT_BUTTON_EVT_DOUBLE_CLICK        6

#define MICROBIT_BUTTON_LONG_CLICK_TIME         1000
#define MICROBIT_BUTTON_HOLD_TIME               1500

#define MICROBIT_BUTTON_STATE                   1
#define MICROBIT_BUTTON_STATE_HOLD_TRIGGERED    2
#define MICROBIT_BUTTON_STATE_CLICK             4
#define MICROBIT_BUTTON_STATE_LONG_CLICK        8

#define MICROBIT_BUTTON_SIGMA_MIN               0
#define MICROBIT_BUTTON_SIGMA_MAX               12
#define MICROBIT_BUTTON_SIGMA_THRESH_HI         8
#define MICROBIT_BUTTON_SIGMA_THRESH_LO         2
#define MICROBIT_BUTTON_DOUBLE_CLICK_THRESH     50

#endif
