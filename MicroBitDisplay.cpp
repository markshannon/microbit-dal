/**
  * Class definition for a MicroBitDisplay.
  *
  * A MicroBitDisplay represents the LED matrix array on the MicroBit device.
  */
#include "MicroBit.h"
#include "MicroBitMatrixMaps.h"
#include <new>
#include "nrf_gpio.h"
#include "mbed.h"

/**
  * Constructor.
  * Create a Point representation of an LED on a matrix
  * Used to handle non-linear matrix layouts.
  */
MatrixPoint::MatrixPoint(int x, int y)
{
    this->x = x;
    this->y = y;
}

/**
  * Constructor.
  * Create a representation of a display of a given size.
  * The display is initially blank.
  *
  * @param x the width of the display in pixels.
  * @param y the height of the display in pixels.
  * 
  * Example:
  * @code 
  * MicroBitDisplay display(MICROBIT_ID_DISPLAY, 5, 5),
  * @endcode
  */
MicroBitDisplay::MicroBitDisplay(int id, int x, int y) : 
    columnPins(MICROBIT_DISPLAY_COLUMN_PINS), 
    font(),
    image(x*2,y)
{
    this->rowDrive = DynamicPwm::allocate(rowPins[0],PWM_PERSISTENCE_PERSISTENT);
    this->id = id;
    this->width = x;
    this->height = y;
    this->strobeRow = 0;
    this->strobeCount = 0;
    this->rowDrive->period_ms(1);
    
    this->rotation = MICROBIT_DISPLAY_ROTATION_0;
    this->setBrightness(MICROBIT_DEFAULT_BRIGHTNESS);
    
    animationMode = ANIMATION_MODE_NONE;
    
    uBit.flags |= MICROBIT_FLAG_DISPLAY_RUNNING;
}

/**
  * Internal frame update method, used to strobe the display.
  *
  * TODO: Write a more efficient, complementary variation of this method for the case where 
  * MICROBIT_DISPLAY_ROW_COUNT > MICROBIT_DISPLAY_COLUMN_COUNT.
  */   
void MicroBitDisplay::strobeUpdate()
{   
    // TODO: Cache coldata for future use, so we don't recompute so often?
    int coldata;

    // Move on to the next row.    
    strobeRow = (strobeRow+1) % MICROBIT_DISPLAY_ROW_COUNT;
        
    // Calculate the bitpattern to write.
    coldata = 0;
    for (int i = 0; i<MICROBIT_DISPLAY_COLUMN_COUNT; i++)
    {
        int x = matrixMap[i][strobeRow].x;
        int y = matrixMap[i][strobeRow].y;
        int t = x;        
        
        switch (rotation)
        {
            case MICROBIT_DISPLAY_ROTATION_90:
                x = width - 1 - y;
                y = t;
                break;
                
            case MICROBIT_DISPLAY_ROTATION_180:
                x = width - 1 - x;
                y = height - 1 - y;
                break;
                
            case MICROBIT_DISPLAY_ROTATION_270:
                x = y;
                y = height - 1 - t;
                break;
        }
        
        if(image.getPixelValue(x, y))
            coldata |= (1 << i);
    }

    // Write to the matrix display.
    columnPins.write(0xffff);    

    rowDrive->redirect(rowPins[strobeRow]);

    columnPins.write(~coldata);

    // Update text and image animations if we need to.
    this->animationUpdate();
}

/**
  * Periodic callback, that we use to perform any animations we have running.
  */
void
MicroBitDisplay::animationUpdate()
{   
    if (animationMode == ANIMATION_MODE_NONE)
        return;
    
    animationTick += FIBER_TICK_PERIOD_MS; 
    
    if(animationTick >= animationDelay)
    {
        animationTick = 0;
        
        if (animationMode == ANIMATION_MODE_SCROLL_TEXT)
            this->updateScrollText();
        
        if (animationMode == ANIMATION_MODE_PRINT_TEXT)
            this->updatePrintText();

        if (animationMode == ANIMATION_MODE_SCROLL_IMAGE)
            this->updateScrollImage();
            
    }
}

/**
  * Broadcasts an event onto the shared MessageBus
  * @param eventCode The ID of the event that has occurred.
  */
void MicroBitDisplay::sendEvent(int eventCode)
{
    MicroBitEvent evt;
    
    evt.source = id;
    evt.context = NULL;
    evt.timestamp = ticks;
    evt.value = eventCode;
    
    uBit.MessageBus.send(evt);
}

/**
  * Internal scrollText update method. 
  * Shift the screen image by one pixel to the left. If necessary, paste in the next char.
  */   
void MicroBitDisplay::updateScrollText()
{    
    image.shiftLeft(1);
    scrollingPosition++;
    
    if (scrollingPosition == width + SPACING)
    {        
        scrollingPosition = 0;
        
        image.print(scrollingChar < scrollingText.length() ? scrollingText.charAt(scrollingChar) : ' ',width,0);

        if (scrollingChar > scrollingText.length())
        {
            animationMode = ANIMATION_MODE_NONE;
            this->sendEvent(MICROBIT_DISPLAY_EVT_SCROLLTEXT_COMPLETE);
            return;
        }
        scrollingChar++;
   }
}

/**
  * Internal printText update method. 
  * Paste in the next char in the string.
  */   
void MicroBitDisplay::updatePrintText()
{        
    image.print(printingChar < printingText.length() ? printingText.charAt(printingChar) : ' ',0,0);

    if (printingChar > printingText.length())
    {
        animationMode = ANIMATION_MODE_NONE;   
        this->sendEvent(MICROBIT_DISPLAY_EVT_PRINTTEXT_COMPLETE);
        return;
    }
    
    printingChar++;
}

/**
  * Internal scrollImage update method. 
  * Paste the stored bitmap at the appropriate point.
  */   
void MicroBitDisplay::updateScrollImage()
{   
    image.clear();     

    if ((image.paste(scrollingImage, scrollingImagePosition, 0, 0) == 0) && scrollingImageRendered)
    {
        animationMode = ANIMATION_MODE_NONE;  
        this->sendEvent(MICROBIT_DISPLAY_EVT_SCROLLIMAGE_COMPLETE);     
        return;
    }

    scrollingImagePosition += scrollingImageStride;
    scrollingImageRendered = true;
}


/**
  * Resets the current given animation.
  * @param delay the delay after which the animation is reset.
  */
void MicroBitDisplay::resetAnimation(int delay)
{
    //sanitise this value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
        
    // Reset any ongoing animation.
    // Clear the display and setup the animation timers.
    animationMode = ANIMATION_MODE_NONE;

    this->image.clear();

    this->animationDelay = delay;
    this->animationTick = delay-1;
}

/**
  * Prints the given character to the display.
  *
  * @param c The character to display.
  * 
  * Example:
  * @code 
  * uBit.display.print('p');
  * @endcode
  */
void MicroBitDisplay::print(char c)
{
    image.print(c, 0, 0);
}

/**
  * Prints the given string to the display, one character at a time.
  * Uses the given delay between characters.
  * Returns immediately, and executes the animation asynchronously.
  *
  * @param s The string to display.
  * @param delay The time to delay between characters, in timer ticks.
  * 
  * Example:
  * @code 
  * uBit.display.printStringAsync("abc123",400);
  * @endcode
  */
void MicroBitDisplay::printStringAsync(ManagedString s, int delay)
{
    //sanitise this value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    this->resetAnimation(delay);
    
    this->printingChar = 0;
    this->printingText = s;
    
    animationMode = ANIMATION_MODE_PRINT_TEXT;
}

/**
  * Prints the given string to the display, one character at a time.
  * Uses the given delay between characters.
  * Blocks the calling thread until all the text has been displayed.
  *
  * @param s The string to display.
  * @param delay The time to delay between characters, in timer ticks.
  * 
  * Example:
  * @code 
  * uBit.display.printString("abc123",400);
  * @endcode
  */
void MicroBitDisplay::printString(ManagedString s, int delay)
{
    //sanitise this value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    // Start the effect.
    this->printStringAsync(s, delay);
    
    // Wait for completion.
    // TODO: We're polling here for now, but should really block on an event here.
    while (animationMode == ANIMATION_MODE_PRINT_TEXT)
        uBit.sleep(100);
}


/**
  * Scrolls the given string to the display, from right to left.
  * Uses the given delay between characters.
  * Returns immediately, and executes the animation asynchronously.
  *
  * @param s The string to display.
  * @param delay The time to delay between characters, in timer ticks.
  * 
  * Example:
  * @code 
  * uBit.display.scrollStringAsync("abc123",100);
  * @endcode
  */
void MicroBitDisplay::scrollStringAsync(ManagedString s, int delay)
{
    //sanitise this value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    this->resetAnimation(delay);
    
    this->scrollingPosition = width-1;
    this->scrollingChar = 0;
    this->scrollingText = s;
    
    animationMode = ANIMATION_MODE_SCROLL_TEXT;
}

/**
  * Scrolls the given string to the display, from right to left.
  * Uses the given delay between characters.
  * Blocks the calling thread until all the text has been displayed.
  *
  * @param s The string to display.
  * @param delay The time to delay between characters, in timer ticks.
  * 
  * Example:
  * @code 
  * uBit.display.scrollString("abc123",100);
  * @endcode
  */
void MicroBitDisplay::scrollString(ManagedString s, int delay)
{
    //sanitise this value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    // Start the effect.
    this->scrollStringAsync(s, delay);
    
    // Wait for completion.
    // TODO: We're polling here for now, but should really block on an event here.
    while (animationMode == ANIMATION_MODE_SCROLL_TEXT)
        uBit.sleep(100);
}


/**
  * Scrolls the given image across the display, from right to left.
  * Returns immediately, and executes the animation asynchronously.
  * @param image The image to display.
  * @param delay The time to delay between each scroll update, in timer ticks. Has a default.
  * @param stride The number of pixels to move in each quantum. Has a default.
  * 
  * Example:
  * @code 
  * MicrobitImage i("1,1,1,1,1\n1,1,1,1,1\n"); 
  * uBit.display.scrollImageAsync(i,100,1);
  * @endcode
  */
void MicroBitDisplay::scrollImageAsync(MicroBitImage image, int delay, int stride)
{
    // Assume right to left functionality, to align with scrollString()
    stride = -stride;
    
    //sanitise the delay value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
            
    this->resetAnimation(delay);

    this->scrollingImagePosition = stride < 0 ? width : -image.getWidth();
    this->scrollingImageStride = stride;
    this->scrollingImage = image;
    this->scrollingImageRendered = false;
        
    animationMode = ANIMATION_MODE_SCROLL_IMAGE;
}

/**
  * Scrolls the given image across the display, from right to left.
  * Blocks the calling thread until all the text has been displayed.
  *
  * @param image The image to display.
  * @param delay The time to delay between each scroll update, in timer ticks. Has a default.
  * @param stride The number of pixels to move in each quantum. Has a default. 
  * 
  * Example:
  * @code 
  * MicrobitImage i("1,1,1,1,1\n1,1,1,1,1\n"); 
  * uBit.display.scrollImage(i,100,1);
  * @endcode
  */
void MicroBitDisplay::scrollImage(MicroBitImage image, int delay, int stride)
{
    //sanitise the delay value
    if(delay <= 0 )
        delay = MICROBIT_DEFAULT_SCROLL_SPEED;
    
    // Start the effect.
    this->scrollImageAsync(image, delay, stride);
    
    // Wait for completion.
    // TODO: We're polling here for now, but should really block on an event here.
    while (animationMode == ANIMATION_MODE_SCROLL_IMAGE)
        uBit.sleep(100);
}


/**
  * Sets the display brightness to the specified level.
  * @param b The brightness to set the brightness to, in the range 0..255.
  * 
  * Example:
  * @code 
  * uBit.display.setBrightness(255); //max brightness
  * @endcode
  */  
void MicroBitDisplay::setBrightness(int b)
{
    //sanitise the brightness level
    if(b < 0 || b > 255)
        return;
    
    float level = (float)b / float(MICROBIT_DISPLAY_MAX_BRIGHTNESS);
    
    this->brightness = b;
    this->rowDrive->write(level);
}

/**
  * Fetches the current brightness of this display.
  * @return the brightness of this display, in the range 0..255.
  * 
  * Example:
  * @code 
  * uBit.display.getBrightness(); //the current brightness
  * @endcode
  */  
int MicroBitDisplay::getBrightness()
{
    return this->brightness;
}

/**
  * Rotates the display to the given position.
  * Axis aligned values only.
  *
  * Example:
  * @code 
  * uBit.display.rotateTo(MICROBIT_DISPLAY_ROTATION_180); //rotates 180 degrees from original orientation
  * @endcode
  */   
void MicroBitDisplay::rotateTo(int position)
{
    //perform a switch on position to restrict range to distinct values
    switch(position){
        case MICROBIT_DISPLAY_ROTATION_0:
            this->rotation = MICROBIT_DISPLAY_ROTATION_0;
            break;   
        case MICROBIT_DISPLAY_ROTATION_90:
            this->rotation = MICROBIT_DISPLAY_ROTATION_90;
            break;
        case MICROBIT_DISPLAY_ROTATION_180:
            this->rotation = MICROBIT_DISPLAY_ROTATION_180;
            break;
        case MICROBIT_DISPLAY_ROTATION_270:
            this->rotation = MICROBIT_DISPLAY_ROTATION_270;
            break;
    }
}

/**
  * Enables the display, should only be called if the display is disabled.
  *
  * Example:
  * @code 
  * uBit.display.enable(); //reenables the display mechanics
  * @endcode
  */
void MicroBitDisplay::enable()
{
    new(&columnPins) BusOut(MICROBIT_DISPLAY_COLUMN_PINS);  //bring columnPins back up
    columnPins.write(0xFFFF);                               //write 0xFFFF to reset all column pins 
    rowDrive = DynamicPwm::allocate(rowPins[0],PWM_PERSISTENCE_PERSISTENT); //bring rowDrive back up
    rowDrive->period_ms(1);                                  
    setBrightness(brightness);
    uBit.flags |= MICROBIT_FLAG_DISPLAY_RUNNING;            //set the display running flag
}
    
/**
  * Disables the display, should only be called if the display is enabled.
  * Display must be disabled to avoid MUXing of edge connector pins.
  *
  * Example:
  * @code 
  * uBit.display.disable(); //disables the display
  * @endcode
  */
void MicroBitDisplay::disable()
{
    uBit.flags &= ~MICROBIT_FLAG_DISPLAY_RUNNING;           //unset the display running flag
    columnPins.~BusOut();
    rowDrive->free();
    
}

/**
  * Clears the current image on the display.
  * Simplifies the process, you can also use uBit.display.image.clear
  *
  * Example:
  * @code 
  * uBit.display.clear(); //clears the display
  * @endcode
  */ 
void MicroBitDisplay::clear()
{
    image.clear();  
}

/**
  * Displays "=(" and an accompanying status code infinitely.
  * @param statusCode the appropriate status code - 0 means no code will be displayed.
  *
  * Example:
  * @code 
  * uBit.display.error(20);
  * @endcode
  */
void MicroBitDisplay::error(int statusCode)
{   
    __disable_irq(); //stop ALL interrupts

    if(statusCode < 0)
        statusCode = 0;

    disable(); //relinquish PWMOut's control
    
    uint8_t strobeRow = 0;
    uint8_t strobeBitMsk = 0x20;
    
    //enter infinite loop.
    while(1)
    {
        
        int coldata = 0;

        int i = 0;

        //if we have hit the row limit - reset both the bit mask and the row variable
        if(strobeRow == 3)
        {
            strobeRow = 0; 
            strobeBitMsk = 0x20;
        }    

        // Calculate the bitpattern to write.
        for (i = 0; i<MICROBIT_DISPLAY_COLUMN_COUNT; i++)
        {
            
            int bitMsk = 0x10 >> matrixMap[i][strobeRow].x; //chars are right aligned but read left to right
            int y = matrixMap[i][strobeRow].y;
                 
            if(panicFace[y] & bitMsk)
                coldata |= (1 << i);
        }
        
        nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT0, 0xF0); //clear port 0 4-7
        nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, strobeBitMsk | 0x1F); // clear port 1 8-12
        
        //write the new bit pattern
        nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT0, ~coldata<<4 & 0xF0); //set port 0 4-7
        nrf_gpio_port_write(NRF_GPIO_PORT_SELECT_PORT1, strobeBitMsk | (~coldata>>4 & 0x1F)); //set port 1 8-12
    
        //set i to an obscene number.
        i = 100000;
        
        //burn cycles
        while(i>0)
            i--;
        
        //update the bit mask and row count
        strobeBitMsk <<= 1;    
        strobeRow++;
    }
}

/**
  * Updates the font property of this object with the new font.
  * @param font the new font that will be used to render characters..
  */
void MicroBitDisplay::setFont(MicroBitFont font)
{
    this->font = font;
}

/**
  * Retreives the font object used for rendering characters on the display.
  */
MicroBitFont MicroBitDisplay::getFont()
{
    return this->font;
}