/*
Copyright (C) 2023 - Bruce MacKinnon KC1FSZ

This program is free software: you can redistribute it and/or modify it under 
the terms of the GNU General Public License as published by the Free 
Software Foundation, either version 3 of the License, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but WITHOUT 
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with 
this program. If not, see <https://www.gnu.org/licenses/>.

PRIOR ART
=========
This borrows heavily from code by Hunter Adams (vha3@cornell.edu), 
Tom Roberts, and Malcolm Slaney (malcolm@interval.com).
*/
#ifndef _PS2Keyboard_h
#define _PS2Keyboard_h

#include <cstdint>
#include "KeyboardListener.h"

void keyboard_init(uint8_t clockPin, uint8_t dataPin, KeyboardListener*);
void keyboard_clock_callback(uint gpio, uint32_t event_mask);

/**
 * Takes the scan code and returns the ASCII character, or zero if not possible.
 */
char keyboard_code_to_ascii(uint16_t scanCode, bool isShift);

// Some keycodes
#define PS2_SCAN_ENTER (0x005a)
#define PS2_SCAN_ESC (0x0076)
#define PS2_SCAN_F1 (0x0005)
#define PS2_SCAN_F2 (0x0006)
#define PS2_SCAN_F3 (0x0004)
#define PS2_SCAN_F4 (0x000c)
#define PS2_SCAN_F5 (0x0003)
#define PS2_SCAN_F6 (0x000d)
#define PS2_SCAN_F7 (0x0083)
#define PS2_SCAN_F8 (0x000a)
#define PS2_SCAN_F9 (0x0001)
#define PS2_SCAN_F10 (0x0009)
#define PS2_SCAN_F11 (0x0078)
#define PS2_SCAN_F12 (0x0007)
#define PS2_SCAN_BSP (0x0066)
#define PS2_SCAN_DEL (0xe076)
#define PS2_SCAN_UP (0xe075)
#define PS2_SCAN_DOWN (0xe072)
#define PS2_SCAN_LEFT (0xe06b)
#define PS2_SCAN_RIGHT (0xe074)
#define PS2_SCAN_HOME (0xe06c)
#define PS2_SCAN_END (0xe069)
#define PS2_SCAN_PGUP (0xe07d)
#define PS2_SCAN_PGDN (0xe07a)

#endif
