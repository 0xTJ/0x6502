#pragma once
#include <stdint.h>
#include <string.h>

extern uint16_t editor_addr;

uint8_t editor_read(uint16_t address);
void editor_write(uint16_t address, uint8_t value);
