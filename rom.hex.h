#ifndef INCLUDE_ROM_HEX_H
#define INCLUDE_ROM_HEX_H

const uint8_t data0[] PROGMEM = { 0xF5 };

struct RomHex {
    const uint8_t byte_count;
    const uint16_t address;
    const uint8_t *data;
} const rom_hex[] PROGMEM = {
    {
        .byte_count = 1,
        .address = 0xFF50,
        .data = data0
    }
};

#endif // NOT INCLUDE_ROM_HEX_H
