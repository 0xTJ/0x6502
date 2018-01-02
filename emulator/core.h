#ifndef INCLUDE_CORE_H
#define INCLUDE_CORE_H

extern uint16_t pc;
extern uint8_t sp, a, x, y, status;

extern uint64_t instructions;
extern uint32_t clockticks6502, clockgoal6502;

void reset6502();

void nmi6502();
void irq6502();

void exec6502(uint32_t tickcount);
void step6502();
void hookexternal(void *funcptr);

#endif  // NOT INCLUDE_CORE_H
