/*
PI PICO Demonstration of interface to a PS/2 style keyboard.

# Make sure that the PICO_SDK_PATH is set properly

cd /home/bruce/pico/hello-pico
# You need main.c and CMakeLists.txt
cp ../pico-sdk/external/pico_sdk_import.cmake .
mkdir build
cd build
cmake ..
make

# Connect the PI PICO to USB while pressing the BOOTSEL button.
# And then:
cp main.uf2 /media/bruce/RPI-RP2

# Make sure the SWD is connected properly:
# GPIO24 (Pin 18) to SWDIO
# GPIO25 (Pin 22) to SWCLK
# GND (Pin 20) to SWGND

# Use this command to flash:
openocd -f interface/raspberrypi-swd.cfg -f target/rp2040.cfg -c "program main.elf verify reset exit"

# Looking at the serial port:
minicom -b 115200 -o -D /dev/ttyACM0
*/
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define KEY_UP_CODE (0xf0)
#define EXTENDED_CODE (0xe0)
#define LEFT_SHIFT_CODE (0x12)
#define RIGHT_SHIFT_CODE (0x59)
#define LEFT_CTL_CODE (0x14)
#define LEFT_ALT_CODE (0x11)

const uint LED_PIN = 25;
const uint KBD_DATA_PIN = 2;
const uint KBD_CLOCK_PIN = 3;

static uint bitCount = 0;
static uint8_t bitAccumulator = 0;
static bool lastKeyUp = false;
static bool inExtended = false;
static uint keyCount = 0;

static bool shiftState = false;
static bool ctlState = false;
static bool altState = false;

/*
Fires on the falling edge of the keyboard clock signal.
*/
static void clock_callback(uint gpio, uint32_t event_mask) {

    const uint dataBit = gpio_get(KBD_DATA_PIN); 
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
                    printf("KBD: %d %02x %d %d %d %d\n", keyCount, bitAccumulator, 
                        (int)inExtended,
                        (int)shiftState, (int)ctlState, (int)altState);
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

int main() {
 
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    gpio_init(KBD_DATA_PIN);
    gpio_set_dir(KBD_DATA_PIN, GPIO_IN);
    
    gpio_init(KBD_CLOCK_PIN);
    gpio_set_dir(KBD_CLOCK_PIN, GPIO_IN);

    // Startup ID
    for (uint i = 0; i < 4; i++) {
        gpio_put(LED_PIN, 1);
        sleep_ms(250);
        gpio_put(LED_PIN, 0);
        sleep_ms(250);
    }    
    puts("PS2 Keyboard Demo");

    uint32_t usSinceBoot = time_us_32();
    printf("  Time ms=%ld\n", usSinceBoot / 1000);

    gpio_set_irq_enabled_with_callback(KBD_CLOCK_PIN, GPIO_IRQ_EDGE_FALL, 
        true, clock_callback);

    // Prevent the main fom exiting
    while(true) { }

    return 0;
}
