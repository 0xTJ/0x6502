#include "machine.h"
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include "duo_travel.h"

bool is_running = false;
const struct PageDef *defd_pages[0x100];
const struct PageDef page_defs[DEFD_PAGE_COUNT] = {
    {
        .page = 0x00,
        .type = RAM,
        .eeprom_addr = 0x00,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x01,
        .type = RAM,
        .eeprom_addr = 0x01,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    },
    {
        .page = 0x02,
        .type = IO,
        .eeprom_addr = 0x01,
        .ioInitFunc = p02Init,
        .ioReadFunc = p02Read,
        .ioWriteFunc = p02Write
    },
    {
        .page = 0xFF,
        .type = ROM,
        .eeprom_addr = 0x7F,
        .ioReadFunc = NULL,
        .ioWriteFunc = NULL
    }
};

void loadPageDefs() {
    clearDisplay();
    for (uint16_t i = 0; i < DEFD_PAGE_COUNT; i++) {
        defd_pages[page_defs[i].page] = &(page_defs[i]);
    }
    resetMachine();
}

void resetMachine() {
    for (uint16_t i = 0; i < DEFD_PAGE_COUNT; i++) {
        if (page_defs[i].type == RAM) {
            for (uint16_t a = 0; a < 0x100; a += 0x10) {
                uint8_t tmp[0x10] = { 0 };
                writeStorage(((uint16_t) page_defs[i].eeprom_addr << 8) + a, tmp, 0x10);
            }
        }
        else if (page_defs[i].type == IO) {
            page_defs[i].ioInitFunc();
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
