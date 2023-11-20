#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "PS2Keyboard.h"

#define KEY_UP_CODE (0xf0)
#define EXTENDED_CODE (0xe0)
#define LEFT_SHIFT_CODE (0x12)
#define RIGHT_SHIFT_CODE (0x59)
#define LEFT_CTL_CODE (0x14)
#define LEFT_ALT_CODE (0x11)

static uint bitCount = 0;
static uint8_t bitAccumulator = 0;
static bool lastKeyUp = false;
static bool inExtended = false;
static uint keyCount = 0;

static bool shiftState = false;
static bool ctlState = false;
static bool altState = false;

static uint8_t ClockPin;
static uint8_t DataPin;
static KeyboardListener* _listener;

void keyboard_init(uint8_t clockPin, uint8_t dataPin, KeyboardListener* list) {
    ClockPin = clockPin;
    DataPin = dataPin;
    _listener = list;
}

/*
Fires on the falling edge of the keyboard clock signal.
*/
void keyboard_clock_callback(uint gpio, uint32_t event_mask) {

    const uint dataBit = gpio_get(DataPin); 
    bitCount++;

    if (bitCount == 1) {
        // Start bit
    } else if (bitCount <= 9) {
        bitAccumulator >>= 1;
		if (dataBit) {
			bitAccumulator |= 0x80;        
        }
    } else if (bitCount == 10) {
        // Pariy bit
    } else if (bitCount == 11) {
        // Stop bit
        if (bitAccumulator == KEY_UP_CODE) {
            lastKeyUp = true;
        } else if (bitAccumulator == EXTENDED_CODE) {
            inExtended = true;
        } else {
            // We only look at the key up events for the shift/ctl/alt keys
            if (lastKeyUp) {
                if (bitAccumulator == LEFT_SHIFT_CODE || bitAccumulator == RIGHT_SHIFT_CODE) {
                    shiftState = false;
                } else if (bitAccumulator == LEFT_CTL_CODE) {
                    ctlState = false;
                } else if (bitAccumulator == LEFT_ALT_CODE) {
                    altState = false;
                }
            }
            else {
                // Look for special state keys
                if (bitAccumulator == LEFT_SHIFT_CODE || bitAccumulator == RIGHT_SHIFT_CODE) {
                    shiftState = true;
                } else if (bitAccumulator == LEFT_CTL_CODE) {
                    ctlState = true;
                } else if (bitAccumulator == LEFT_ALT_CODE) {
                    altState = true;
                }
                // Regular key
                else {
                    _listener->onKeyDown(bitAccumulator, inExtended, 
                        shiftState, ctlState, altState);
                }
            }
            lastKeyUp = false;
            inExtended = false;
        }
        bitCount = 0;
        bitAccumulator = 0;
        keyCount++;
    }
}   
