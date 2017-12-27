#ifndef INCLUDE_ROM_HEX_H
#define INCLUDE_ROM_HEX_H

struct RomHex {
    uint8_t byte_count;
    uint16_t address;
    uint8_t *data;
} const rom_hex[] = {
    {
      .byte_count = 1,
      .address = 0xFF50,
      .data = (uint8_t []) { 0xF5 }
    }
};

#endif // NOT INCLUDE_ROM_HEX_H
