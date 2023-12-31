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
#ifndef _KeyboardListener_h
#define _KeyboardListener_h

class KeyboardListener {
public:

    virtual void onKeyDown(uint16_t scanCode, 
        bool isShift, bool isCtl, bool isAlt);
};

#endif

