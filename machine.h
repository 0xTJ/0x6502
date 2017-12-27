#ifndef INCLUDE_MACHINE_H
#define INCLUDE_MACHINE_H

#include <stdbool.h>
#include <stdint.h>

#define STACK_BASE  (0x0100)
#define SCREEN_BASE (0x0200)
#define SCREEN_SIZE (0x0020)
#define SPRITE_BASE (0x0220)
#define SPRITE_SIZE (0x0040)
#define KEYS_ADDR   (0x0280)

#define DEFD_PAGE_COUNT 4

enum PageType { NONE = 0, ROM, RAM, IO };

struct PageDef {
    uint8_t page;
    enum PageType type;
    uint8_t eeprom_addr;
    void (*ioInitFunc)();
    uint8_t (*ioReadFunc)(uint8_t addr);
    void (*ioWriteFunc)(uint8_t addr, uint8_t value);
};

extern bool is_running;
extern const struct PageDef *defd_pages[];
extern const struct PageDef page_defs[];

void loadPageDefs();
void resetMachine();

extern uint8_t screen_mem[32];
extern uint8_t sprite_mem[8 * 8];
void p02Init();
uint8_t p02Read(uint8_t addr);
void p02Write(uint8_t addr, uint8_t value);

#endif  // NOT INCLUDE_MACHINE_H
