/**
  * Class definition for a MicroBit Device Firmware Update loader.
  *
  * This is actually just a frontend to a memory resident nordic DFU loader.
  * Here we deal with the MicroBit 'pairing' functionality with BLE devices, and
  * very basic authentication and authorization. 
  *
  * This implementation is not intended to be fully secure, but rather intends to:
  *
  * 1. Provide a simple mechanism to identify an individual MicroBit amongst a classroom of others
  * 2. Allow BLE devices to discover and cache a passcode that can be used to flash the device over BLE.
  * 3. Provide a BLE escape route for programs that 'brick' the MicroBit. 
  *
  * Represents the device as a whole, and includes member variables to that reflect the components of the system.
  */
  
#include "MicroBit.h"
#include "UUID.h"

#include "test/MicroBitTest.h"
/**
  * Constructor. 
  * Create a representation of a MicroBit device.
  * @param messageBus callback function to receive MicroBitMessageBus events.
  */
MicroBitDFUService::MicroBitDFUService(BLEDevice &_ble) : 
        ble(_ble), 
        microBitDFUServiceControlCharacteristic(MicroBitDFUServiceControlCharacteristicUUID, &controlByte),
        microBitDFUServiceFlashCodeCharacteristic(MicroBitDFUServiceFlashCodeCharacteristicUUID, (uint8_t *)&flashCode, 0, sizeof(uint32_t),
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE),
        microBitScrollTextCharacteristic(MicroBitDFUServiceScrollTextCharacteristicUUID, scrollBytes, 20, 20,
        GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE)
{
    authenticated = false;
    flashCodeRequested = false;

    controlByte = 0x00;
    flashCode = 0x00;
    
    GattCharacteristic *characteristics[] = {&microBitDFUServiceControlCharacteristic, &microBitDFUServiceFlashCodeCharacteristic, &microBitScrollTextCharacteristic};
    GattService         service(MicroBitDFUServiceUUID, characteristics, sizeof(characteristics) / sizeof(GattCharacteristic *));

    ble.addService(service);

    ble.onDataWritten(this, &MicroBitDFUService::onDataWritten);
}


/**
  * Returns the friendly name for this device, autogenerated from our Device ID.
  *
  * @param name Pointer to a string where the data will be written.
  * @return The number of bytes written.
  */
int MicroBitDFUService::getName(char *name)
{
    const uint8_t codebook[5][5] = 
    {
        {'z', 'v', 'g', 'p', 't'},  
        {'u', 'o', 'i', 'e', 'a'},
        {'z', 'v', 'g', 'p', 't'},  
        {'u', 'o', 'i', 'e', 'a'},
        {'z', 'v', 'g', 'p', 't'}
    };

    // We count right to left, so fast forward the pointer.
    name += MICROBIT_DFU_HISTOGRAM_WIDTH;

    //int n = NRF_FICR->DEVICEID[1];
    int n = 0xbabe;
    
    int ld = 1;
    int d = MICROBIT_DFU_HISTOGRAM_HEIGHT;
    int h;

#ifdef MICROBIT_DEBUG
    pc.printf("MicroBitDFUService::getName: Called [%.8x]\n",n);
#endif    

    for (int i=0; i<MICROBIT_DFU_HISTOGRAM_WIDTH;i++)
    {
        h = (n % d) / ld;
        n -= h;
        d *= MICROBIT_DFU_HISTOGRAM_HEIGHT;
        ld *= MICROBIT_DFU_HISTOGRAM_HEIGHT;
        *--name = codebook[i][h];
    }

    return MICROBIT_DFU_HISTOGRAM_WIDTH;
}

/**
  * Begin the pairing process. Typically called when device is powered up with buttons held down.
  * Scroll a description on the display, then displays the device ID code as a histogram on the matrix display.
  */
void MicroBitDFUService::pair()
{
#ifdef MICROBIT_DEBUG
    pc.printf("MicroBitDFUService::pair: Called\n");
#endif
    
    ManagedString blueZoneString("BLUE ZONE...");
    ManagedString pairString("PAIR?");
    
    uBit.display.scrollString(blueZoneString);
    wait(8.0);
    showNameHistogram();
    
    while(1)
    {    
        for (int i=0; i<100; i++)
        {
            if (flashCodeRequested)
            {
                uBit.display.scrollString(pairString);
                for (int j=0; j<40; j++)
                {          
                    if (uBit.leftButton.isPressed())
                    {
                        i=100;
                        releaseFlashCode();
                        showTick();
                        flashCodeRequested = false;
                        authenticated = true;
                        break;
                    }
                             
                    wait(0.1);
                }
            }
            wait (0.1);
            
            // If our peer disconnects, drop all state.
            if ((authenticated || flashCodeRequested) && !ble.getGapState().connected)
            {
                authenticated = false;
                flashCodeRequested = false;
                flashCode = 0x00;
            }
        }
    }
}

/**
  * Callback. Invoked when any of our attributes are written via BLE.
  */
void MicroBitDFUService::onDataWritten(const GattCharacteristicWriteCBParams *params)
{
#ifdef MICROBIT_DEBUG
    pc.printf("MicroBitDFUService::onDataWritten: Called... ");       
#endif
    
    if (params->charHandle == microBitDFUServiceControlCharacteristic.getValueHandle()) {

#ifdef MICROBIT_DEBUG        
    pc.printf("Control Point:\n   ");
#endif    
        if (params->len < 1)
        {
#ifdef MICROBIT_DEBUG
            pc.printf("  invalid. Ignoring.\n");
#endif
            return;
        }
        
#ifdef MICROBIT_DEBUG
        for (int i=0; i<params->len; i++)
            pc.printf("%.2x ", params->data[i]);            

        pc.printf("\n");
#endif
        
        switch(params->data[0])
        {
            case MICROBIT_DFU_OPCODE_START_DFU:
            
                if (authenticated)
                {
#ifdef MICROBIT_DEBUG
                    pc.printf("  ACTIVATING BOOTLOADER.\n");
#endif
                    bootloader_start();    
                }   
            
                break;
            
            case MICROBIT_DFU_OPCODE_START_PAIR:
#ifdef MICROBIT_DEBUG
                pc.printf("  START_PAIR:  ");                
#endif                
                flashCodeRequested = true;
                break;
                                
        }
    }
    
    if (params->charHandle == microBitDFUServiceFlashCodeCharacteristic.getValueHandle()) {

#ifdef MICROBIT_DEBUG
        pc.printf("FlashCode\n\n");    
#endif        

        if (params->len >= 4)
        {            
            uint32_t lockCode=0;
            uint32_t t = 0xcafe;
            memcpy(&lockCode, params->data, 4);
            //if (lockCode == NRF_FICR->DEVICEID[0])
            if (lockCode == 0xcafe)
            {
#ifdef MICROBIT_DEBUG
                pc.printf("AUTHENTICATED\n");                
#endif
                authenticated = true;
            }else{
#ifdef MICROBIT_DEBUG                
                pc.printf("NOT AUTHENTICATED: %8x : %8x\n", lockCode, t);                
#endif
                authenticated = false;
            }
        }      
    }

#ifdef MAIN_FOTA_TEST
    if (params->charHandle == microBitScrollTextCharacteristic.getValueHandle()) {
#ifdef MICROBIT_DEBUG
        pc.printf("ScrollText\n\n");    
#endif        
        updateScroll((char *)params->data, params->len);
    }
#endif
}

/**
  * Displays the device's ID code as a histogram on the LED matrix display.
  */
void MicroBitDFUService::showTick()
{
    uBit.display.scrollString(ManagedString::EmptyString);
    uBit.display.image.setPixelValue(0,3, 255);
    uBit.display.image.setPixelValue(1,4, 255);
    uBit.display.image.setPixelValue(2,3, 255);
    uBit.display.image.setPixelValue(3,2, 255);
    uBit.display.image.setPixelValue(4,1, 255);
}


/**
  * Displays the device's ID code as a histogram on the LED matrix display.
  */
void MicroBitDFUService::showNameHistogram()
{
#ifdef MICROBIT_DEBUG
    pc.printf("MicroBitDFUService::showNameHistogram: Called\n");
#endif
    
    uBit.display.scrollString(ManagedString::EmptyString);

    //int n = NRF_FICR->DEVICEID[1];
    int n = 0xbabe;
    int ld = 1;
    int d = MICROBIT_DFU_HISTOGRAM_HEIGHT;
    int h;

    for (int i=0; i<MICROBIT_DFU_HISTOGRAM_WIDTH;i++)
    {
        h = (n % d) / ld;

        n -= h;
        d *= MICROBIT_DFU_HISTOGRAM_HEIGHT;
        ld *= MICROBIT_DFU_HISTOGRAM_HEIGHT;

        for (int j=0; j<h+1; j++)
            uBit.display.image.setPixelValue(MICROBIT_DFU_HISTOGRAM_WIDTH-i-1, MICROBIT_DFU_HISTOGRAM_HEIGHT-j-1, 255);
    }       
}

/**
  * Displays the device's ID code as a histogram on the LED matrix display.
  */
void MicroBitDFUService::releaseFlashCode()
{
#ifdef MICROBIT_DEBUG
    pc.printf("MicroBitDFUService::releaseFlashCode: Called\n");       
#endif    
    //flashCode = NRF_FICR->DEVICEID[0];
    flashCode = 0xcafe;
    ble.updateCharacteristicValue(microBitDFUServiceFlashCodeCharacteristic.getValueHandle(), (uint8_t *)&flashCode, sizeof(uint32_t));
}

/**
  * UUID definitions for BLE Services and Characteristics.
  */

const uint8_t              MicroBitDFUServiceUUID[] = {
    0xd8,0xaf,0x99,0x1c,0x71,0x44,0x43,0xd7,0x95,0x4b,0x99,0x51,0x2f,0x95,0xf9,0x9c
};

const uint8_t              MicroBitDFUServiceControlCharacteristicUUID[] = {
    0x97,0x10,0x95,0x47,0xe6,0x3a,0x44,0x2a,0xbf,0x89,0x9d,0x73,0x04,0x13,0xdc,0x2f
};

const uint8_t              MicroBitDFUServiceFlashCodeCharacteristicUUID[] = {
    0x94,0x7b,0x69,0x34,0x64,0xd1,0x4f,0xad,0x9b,0xd0,0xcc,0x9d,0x6e,0x9f,0x3e,0xa3
};

const uint8_t              MicroBitDFUServiceScrollTextCharacteristicUUID[] = {
    0x94,0x7b,0x69,0x34,0x64,0xd1,0x4f,0xad,0x9b,0xd0,0xcc,0x9d,0x6e,0x9f,0x3e,0xa4
};