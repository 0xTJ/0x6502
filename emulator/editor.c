#include "editor.h"
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"
#include "machine.h"
#include "emulator.h"
#include "programmer.h"

static void dispEditor(uint16_t addr, uint8_t byte) {
    clearDisplay();
    displayCharacter(3, 1, '0');
    displayCharacter(2, 1, '1');
    displayCharacter(1, 1, '2');
    displayCharacter(0, 1, '3');
    displayCharacter(3, 0, '4');
    displayCharacter(2, 0, '5');
    displayCharacter(1, 0, '6');
    displayCharacter(0, 0, '7');
    
    displayCharacter(12 + 3, 1, byte & 0x01 ? '1' : '0');
    displayCharacter(12 + 2, 1, byte & 0x02 ? '1' : '0');
    displayCharacter(12 + 1, 1, byte & 0x04 ? '1' : '0');
    displayCharacter(12 + 0, 1, byte & 0x08 ? '1' : '0');
    displayCharacter(12 + 3, 0, byte & 0x10 ? '1' : '0');
    displayCharacter(12 + 2, 0, byte & 0x20 ? '1' : '0');
    displayCharacter(12 + 1, 0, byte & 0x40 ? '1' : '0');
    displayCharacter(12 + 0, 0, byte & 0x80 ? '1' : '0');
    
    char buf[7];
    sprintf(buf, "A:%04X", addr);
    for (int i = 0; i < 6; i++)
        displayCharacter(5 + i, 0, buf[i]);
    
    sprintf(buf, "P:  %02X", byte);
    for (int i = 0; i < 6; i++)
        displayCharacter(5 + i, 1, buf[i]);
}

static uint16_t getAddr(uint16_t addr) {
    bool doLoop = true;
    uint8_t addr_h = (addr >> 8) & 0xFF, addr_l = addr & 0xFF;
    uint8_t *addr_sel = &addr_h;
    
    clearDisplay();
    while (doLoop) {
        char buf[17];
        sprintf(buf, "U:0x%02X %c"BYTE_TO_BINARY_PATTERN, addr_h, addr_sel == &addr_h ? 0x7E : ' ', BYTE_TO_BINARY(addr_h));
        for (int i = 0; i < 16; i++)
            displayCharacter(i, 0, buf[i]);
        sprintf(buf, "L:0x%02X %c"BYTE_TO_BINARY_PATTERN, addr_l, addr_sel == &addr_l ? 0x7E : ' ', BYTE_TO_BINARY(addr_l));
        for (int i = 0; i < 16; i++)
            displayCharacter(i, 1, buf[i]);
        
        switch (getKey()) {
                case KEY_B0:
                    *addr_sel ^= 0x01;
                    break;
                case KEY_B1:
                    *addr_sel ^= 0x02;
                    break;
                case KEY_B2:
                    *addr_sel ^= 0x04;
                    break;
                case KEY_B3:
                    *addr_sel ^= 0x08;
                    break;
                case KEY_B4:
                    *addr_sel ^= 0x10;
                    break;
                case KEY_B5:
                    *addr_sel ^= 0x20;
                    break;
                case KEY_B6:
                    *addr_sel ^= 0x40;
                    break;
                case KEY_B7:
                    *addr_sel ^= 0x80;
                    break;
                case KEY_FINISH:
                    doLoop = false;
                    break;
                case KEY_SELECT_OPTION:
                    addr_sel = addr_sel == &addr_l ? &addr_h : &addr_l;
                    break;
        }
    }
    
    while (getPressedKey() >= 0);
    return (addr_h << 8) + addr_l;
}

void editor() {
    uint16_t addr = 0x0000;
    clearDisplay();
    while (true) {
        dispEditor(addr, read6502(addr));
        
        uint8_t tmp_key = getKey();
        if (tmp_key == KEY_B0) {
            write6502(addr, read6502(addr) ^ 0x01);
        }
        else if (tmp_key == KEY_B1) {
            write6502(addr, read6502(addr) ^ 0x02);
        }
        else if (tmp_key == KEY_B2) {
            write6502(addr, read6502(addr) ^ 0x04);
        }
        else if (tmp_key == KEY_B3) {
            write6502(addr, read6502(addr) ^ 0x08);
        }
        else if (tmp_key == KEY_B4) {
            write6502(addr, read6502(addr) ^ 0x10);
        }
        else if (tmp_key == KEY_B5) {
            write6502(addr, read6502(addr) ^ 0x20);
        }
        else if (tmp_key == KEY_B6) {
            write6502(addr, read6502(addr) ^ 0x40);
        }
        else if (tmp_key == KEY_B7) {
            write6502(addr, read6502(addr) ^ 0x80);
        }
        else if (tmp_key == KEY_NXT) {
            addr++;
        }
        else if (tmp_key == KEY_PRV) {
            addr--;
        }
        else if (tmp_key == KEY_SAVE) {
            const uint16_t comp = (1 << KEY_SAVE) | (1 << KEY_DEL);
            
            clearDisplay();
            char buf1[] = "Hold NEWLINE and";
            for (int i = 0; i < 16 && buf1[i]; i++)
                displayCharacter(i, 0, buf1[i]);
            char buf2[] = "CLEAR to flash";
            for (int i = 0; i < 16 && buf2[i]; i++)
                displayCharacter(i, 1, buf2[i]);
            
            bool do_flash = true;
            while (getPressedKeys() != comp) {
                if (getPressedKey() == KEY_ESCAPE) {
                    do_flash = false;
                    break;
                }
            }
            
            if (do_flash)
                for (uint8_t i = 0; i < 40; i++) {
                    _delay_ms(20);
                    if (getPressedKeys() != comp) {
                        do_flash = false;
                        break;
                    }
                }
            if (do_flash)
                programmer();
            while (getPressedKey() >= 0);
        }
        else if (tmp_key == KEY_DEL) {
            
        }
        else if (tmp_key == KEY_RUN) {
            is_running = true;
            run();
            is_running = false;
        }
        else if (tmp_key == KEY_ALT) {
            addr = getAddr(addr);
        }
    }
}