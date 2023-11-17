#ifndef _PS2Keboard_h
#define _PS2Keboard_h

class KeyboardListener {
public:

    virtual void onKey() = 0;
};

void keyboard_init(uint8_t clockPin, uint8_t dataPin, KeyboardListener*);
void keyboard_clock_callback(uint gpio, uint32_t event_mask);

#endif

