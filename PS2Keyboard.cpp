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
                    uint16_t scanCode = 0;
                    if (inExtended) {
                        scanCode = 0xe000;
                    }
                    scanCode |= bitAccumulator;
                    _listener->onKeyDown(scanCode, shiftState, ctlState, altState);
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

struct KeyMap {    
    uint16_t scanCode;
    char asciiLower;
    char asciiUpper;
};

static KeyMap Map[] = {
    { 0x001c, 'a', 'A' },
    { 0x0032, 'b', 'B' },
    { 0x0021, 'c', 'C' },
    { 0x0023, 'd', 'D' },
    { 0x0024, 'e', 'E' },
    { 0x002b, 'f', 'F' },
    { 0x0034, 'g', 'G' },
    { 0x0033, 'h', 'H' },
    { 0x0043, 'i', 'I' },
    { 0x003b, 'j', 'J' },
    { 0x0042, 'k', 'K' },
    { 0x004b, 'l', 'L' },
    { 0x003a, 'm', 'M' },
    { 0x0031, 'n', 'N' },
    { 0x0044, 'o', 'O' },
    { 0x004d, 'p', 'P' },
    { 0x0015, 'q', 'Q' },
    { 0x002d, 'r', 'R' },
    { 0x001b, 's', 'S' },
    { 0x002c, 't', 'T' },
    { 0x003c, 'u', 'U' },
    { 0x002a, 'v', 'V' },
    { 0x001d, 'w', 'W' },
    { 0x0022, 'x', 'X' },
    { 0x0035, 'y', 'Y' },
    { 0x001a, 'z', 'Z' },
    { 0x0016, '1', '!' },
    { 0x001e, '2', '@' },
    { 0x0026, '3', '#' },
    { 0x0025, '4', '$' },
    { 0x002e, '5', '%' },
    { 0x0036, '6', '^' },
    { 0x003d, '7', '&' },
    { 0x003e, '8', '*' },
    { 0x0046, '9', '(' },
    { 0x0045, '0', ')' },
    { 0x000e, '`', '~' },
    { 0x004e, '-', '_' },
    { 0x0055, '=', '+' },
    { 0x004c, ';', ':' },
    { 0x0052, '\'', '\"' },
    { 0x0054, '[', '{' },
    { 0x005b, ']', '}' },
    { 0x0041, ',', '<' },
    { 0x0049, '.', '>' },
    { 0x004a, '/', '?' },
};

static uint16_t MapSize = 46;

char keyboard_code_to_ascii(uint16_t scanCode, bool isShift) {
    for (uint16_t i = 0; i < MapSize; i++) {
        if (scanCode == Map[i].scanCode) {
            if (isShift) {
                return Map[i].asciiUpper;
            } else {
                return Map[i].asciiLower;
            }
        }
    }
    return 0;
}
