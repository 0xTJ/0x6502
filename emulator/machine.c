#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"
#include "core.h"

bool is_running = false;
const struct PageDef *defd_pages[0x100];
#include "memconf.h"
const uint16_t defd_page_count = sizeof(page_defs) / sizeof(struct PageDef);

void loadPageDefs() {
    clearDisplay();
    for (uint16_t i = 0; i < defd_page_count; i++) {
        defd_pages[page_defs[i].page] = &(page_defs[i]);
    }
    resetMachine();
}

void resetMachine() {
    for (uint16_t i = 0; i < defd_page_count; i++) {
        if (page_defs[i].type == DRAM) {
            // for (uint16_t a = 0; a < 0x100; a += 0x10) {
                // uint8_t tmp[0x10] = { 0 };
                // writeStorage(((uint16_t) page_defs[i].eeprom_addr << 8) + a, tmp, 0x10);
            // }
        }
        else if (page_defs[i].type == IO) {
            page_defs[i].ioInitFunc();
        }
    }
    
    hookexternal(externProc);
}

void externProc() {
    
}

uint8_t read6502(uint16_t address) {
    uint8_t page = (address >> 8) & 0xFF;
    uint8_t addr = address & 0xFF;
    
    if (defd_pages[page] != NULL) {
        switch (defd_pages[page]->type) {
            case NONE:
                return 0;
            case ROM:
            case DRAM:
            case SRAM:
                return readStorageInt8(address);
                break;
            case IO:
                return defd_pages[page]->ioReadFunc(addr);
        }
    }
    else
        return 0;
}

void write6502(uint16_t address, uint8_t value) {
    uint8_t page = (address >> 8) & 0xFF;
    uint8_t addr = address & 0xFF;
    
    if (defd_pages[page] != NULL) {
        switch (defd_pages[page]->type) {
            case NONE:
                return;
            case ROM:
                if (is_running)
                    return;
            case DRAM:
            case SRAM:
                writeStorage(((uint16_t) defd_pages[page]->eeprom_addr << 8) + addr, &value, 1);
                break;
            case IO:
                defd_pages[page]->ioWriteFunc(addr, value);
                return;
        }
    }
}

uint8_t screen_mem[32];
uint8_t sprite_mem[8 * 8];

static void updateSprite(uint8_t num) {
    sendDisplayCommand(0x40 + (num * 8));
    for (uint8_t i = 0; i < 8; i++) {
        sendDisplayCharacter(sprite_mem[(num * 8) + i]);
    }
}

void p02Init() {
    for (uint8_t i = 0; i < 32; i++) {
        screen_mem[i] = 0;
    }
    for (uint8_t i = 0; i < 8; i++) {
        for (uint8_t j = 0; j < 8; j++) {
            sprite_mem[i * 8 + j] = 0x00;
        }
        updateSprite(i);
    }
}

uint8_t p02Read(uint8_t addr) {
    if ((SCREEN_BASE & 0xFF) <= addr && addr <= (SCREEN_BASE + (SCREEN_SIZE - 1) & 0xFF)) {
        return screen_mem[addr - (SCREEN_BASE & 0xFF)];
    }
    else if ((SPRITE_BASE & 0xFF) <= addr && addr <= ((SPRITE_BASE + (SPRITE_SIZE - 1)) & 0xFF)) {
        uint8_t sprite = (addr - (SPRITE_BASE & 0xFF)) / 8;
        uint8_t byte = addr - (SPRITE_BASE & 0xFF) - (sprite * 8);
        return sprite_mem[sprite * 8 + byte];
    }
    else if (addr == (KEYS_ADDR & 0xFF)) {
        return getPressedKeys() & 0xFF;
    }
}

void p02Write(uint8_t addr, uint8_t value) {
    if ((SCREEN_BASE & 0xFF) <= addr && addr <= (SCREEN_BASE + (SCREEN_SIZE - 1) & 0xFF)) {
        screen_mem[addr - (SCREEN_BASE & 0xFF)] = value;
    }
    else if ((SPRITE_BASE & 0xFF) <= addr && addr <= ((SPRITE_BASE + (SPRITE_SIZE - 1)) & 0xFF)) {
        uint8_t sprite = (addr - (SPRITE_BASE & 0xFF)) / 8;
        uint8_t byte = addr - (SPRITE_BASE & 0xFF) - (sprite * 8);
        sprite_mem[sprite * 8 + byte] = value;
        updateSprite(sprite);
    }
    else if (addr == (KEYS_ADDR & 0xFF)) {
    }
}
