#include "emulator.h"
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include "duo_travel.h"
#include "core.h"


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
    sprintf(buf, "P"BYTE_TO_BINARY_PATTERN" I%02X", BYTE_TO_BINARY(status), instructions & 0xFF);
    for (int i = 0; i < 16 && buf[i]; i++)
        displayCharacter(i, 1, buf[i]);
}

void run() {
    while (getPressedKeys());
    
    resetMachine();
    reset6502();
    bool screenDisp = false;
    bool manual_clock = true;
    while (true) {        
        if (screenDisp)
            dispRam();
        else
            dispProc();
        
        uint16_t tmp_keys = getPressedKeys();
        while (tmp_keys < 0xFF && manual_clock)
            tmp_keys = getPressedKeys();
        if (tmp_keys >> KEY_ALT & 1) {
            return;
        }
        else if (tmp_keys >> KEY_DEL & 1) {
            manual_clock = !manual_clock;
            while (getPressedKeys() >> KEY_DEL & 1);
        }
        else if (tmp_keys >> KEY_SAVE & 1) {
            screenDisp = !screenDisp;
            while (getPressedKeys() >> KEY_SAVE & 1);
        }
        else if (tmp_keys >> KEY_FINISH & 1 || manual_clock == false) {
            step6502();
            if (manual_clock && getPressedKeys() >> KEY_FINISH & 1)
                _delay_ms(200);
        }
    }
}
