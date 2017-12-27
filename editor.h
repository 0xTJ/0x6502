#ifndef INCLUDE_EDITOR_H
#define INCLUDE_EDITOR_H

#include <stdint.h>

static void dispEditor(uint16_t addr, uint8_t byte);
static uint16_t getAddr(uint16_t addr);
void editor();

#endif  // NOT INCLUDE_EDITOR_H
