#ifndef INCLUDE_MACHINE_H
#define INCLUDE_MACHINE_H

#include <stdbool.h>
#include <stdint.h>

#define IRQ_TUNING  (0x4000000)

#define STACK_BASE  (0x0100)

#define KEYBRD_BASE (0x0300)
#define KEYBRD_TOP  (0x037F)
#define KEYBRD_HEAD (0x0380)
#define KEYBRD_TAIL (0x0381)

#define SCREEN_BASE (0x0400)

#define SCREEN_HEIGHT   (25)
#define SCREEN_WIDTH    (40)

enum PageType { NONE = 0, ROM, DRAM, SRAM, IO };

struct PageDef {
    uint8_t page;
    enum PageType type;
    uint8_t *array;
    uint16_t array_base;
    void (*ioInitFunc)();
    uint8_t (*ioReadFunc)(uint8_t addr);
    void (*ioWriteFunc)(uint8_t addr, uint8_t value);
};

extern bool is_running;
extern const struct PageDef *defd_pages[];
extern const struct PageDef page_defs[];

void setupMachine();
void resetMachine();

void externProc();

uint8_t read6502(uint16_t address);
void write6502(uint16_t address, uint8_t value);

void get_keys(unsigned char c);

// extern uint8_t screen_mem[32];
// extern uint8_t sprite_mem[8 * 8];
// void p02Init();
// uint8_t p02Read(uint8_t addr);
// void p02Write(uint8_t addr, uint8_t value);

#endif  // NOT INCLUDE_MACHINE_H
