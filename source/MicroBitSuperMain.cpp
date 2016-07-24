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

#include "MicroBit.h"

MicroBit        uBit;
InterruptIn     resetButton(MICROBIT_PIN_BUTTON_RESET);

extern char* MICROBIT_BLE_DEVICE_NAME;

int main()
{    
    // Bring up soft reset button.
    resetButton.mode(PullUp);
    resetButton.fall(microbit_reset);

#if CONFIG_ENABLED(MICROBIT_DBG)

    // For diagnostics. Gives time to open the console window. :-) 
    for (int i=3; i>0; i--)
    {
        uBit.serial.printf("=== SUPERMAIN: Starting in %d ===\n", i);
        wait(1.0);
    }

    uBit.serial.printf("micro:bit runtime DAL version %s\n", MICROBIT_DAL_VERSION);

#endif    

    // Bring up random number generator, BLE, display and system timers.    
    uBit.init();

    // Provide time for all threaded initialisers to complete.
    uBit.sleep(100);

    app_main();

    // We should never get here, but just in case.
    while(1);
}
