#include "emulator.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "duo_travel.h"
#include "core.h"

bool screenDisp = false;

void dispRam() {
    for (uint8_t i = 0; i < 16; i++) {
        displayCharacter(i, 0, screen_mem[i]);
    }
    for (uint8_t i = 0; i < 16; i++) {
        displayCharacter(i, 1, screen_mem[16 + i]);
    }
}

void dispProc() {
    clearDisplay();
    char buf[17];
    sprintf(buf, "A"BYTE_TO_BINARY_PATTERN" PC%04X", BYTE_TO_BINARY(a), pc);
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 0, buf[i]);
    sprintf(buf, "P"BYTE_TO_BINARY_PATTERN" %02X S%02X", BYTE_TO_BINARY(status), instructions & 0xFF, sp);
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 1, buf[i]);
}

void run() {
    resetMachine();
    reset6502();
    screenDisp = true;
    while (true) {        
        if (screenDisp)
            dispRam();
        else
            dispProc();
        
        uint16_t tmp_keys = getPressedKeys();
        if (tmp_keys >> KEY_ALT & 1) {
            return;
        }
        else if (tmp_keys >> KEY_DEL & 1) {
            screenDisp = true;
        }
        else if (tmp_keys >> KEY_SAVE & 1) {
            screenDisp = false;
        }
        else {
            step6502();
        }
    }
}
