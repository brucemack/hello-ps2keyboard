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
#include "PS2Keyboard.h"

#define LED_PIN (25)
const uint KBD_DATA_PIN = 2;
const uint KBD_CLOCK_PIN = 3;

class Listener : public KeyboardListener {
public:


    virtual void onKeyDown(uint8_t scanCode, bool isExtended, 
        bool isShift, bool isCtl, bool isAlt) { 
    printf("KBD: %02x %d %d %d %d\n", (int)scanCode, 
        (int)isExtended,
        (int)isShift, (int)isCtl, (int)isAlt);
    }
};

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

    Listener listener;

    keyboard_init(KBD_CLOCK_PIN, KBD_DATA_PIN, &listener);

    gpio_set_irq_enabled_with_callback(KBD_CLOCK_PIN, GPIO_IRQ_EDGE_FALL, 
        true, keyboard_clock_callback);

    // Prevent the main fom exiting
    while(true) { }

    return 0;
}
