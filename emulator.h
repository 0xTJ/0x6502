#ifndef INCLUDE_DUO_EMULATOR_H
#define INCLUDE_DUO_EMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include "machine.h"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define KEY_B1 0
#define KEY_B0 1
#define KEY_B5 2
#define KEY_B4 3
#define KEY_B3 4
#define KEY_B2 5
#define KEY_B7 6
#define KEY_B6 7
#define KEY_NXT 8
#define KEY_PRV 9
#define KEY_SAVE 10
#define KEY_ALT 11
#define KEY_RUN 12
#define KEY_DEL 13

#define DYN_CACHE_SIZE 1
#if DYN_CACHE_SIZE < 1
#error RAM cache is required!
#endif

void run();

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);

#endif  // NOT INCLUDE_DUO_EMULATOR_H
