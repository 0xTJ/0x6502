/* Fake6502 CPU emulator core v1.1 *******************
 * (c)2011-2013 Mike Chambers                        *
 *****************************************************/

#include <stdio.h>
#include <stdint.h>
#include <avr/pgmspace.h>
#include "machine.h"

//externally supplied functions
extern uint8_t read6502(uint16_t address);
extern void write6502(uint16_t address, uint8_t value);

//6502 defines
#define UNDOCUMENTED //when this is defined, undocumented opcodes are handled.
                     //otherwise, they're simply treated as NOPs.

//#define NES_CPU      //when this is defined, the binary-coded decimal (BCD)
                     //status flag is not honored by ADC and SBC. the 2A03
                     //CPU in the Nintendo Entertainment System does not
                     //support BCD operation.

#define FLAG_CARRY     0x01
#define FLAG_ZERO      0x02
#define FLAG_INTERRUPT 0x04
#define FLAG_DECIMAL   0x08
#define FLAG_BREAK     0x10
#define FLAG_CONSTANT  0x20
#define FLAG_OVERFLOW  0x40
#define FLAG_SIGN      0x80

#define BASE_STACK     0x100

#define saveaccum(n) a = (uint8_t)((n) & 0x00FF)


//flag modifier macros
#define setcarry() status |= FLAG_CARRY
#define clearcarry() status &= (~FLAG_CARRY)
#define setzero() status |= FLAG_ZERO
#define clearzero() status &= (~FLAG_ZERO)
#define setinterrupt() status |= FLAG_INTERRUPT
#define clearinterrupt() status &= (~FLAG_INTERRUPT)
#define setdecimal() status |= FLAG_DECIMAL
#define cleardecimal() status &= (~FLAG_DECIMAL)
#define setoverflow() status |= FLAG_OVERFLOW
#define clearoverflow() status &= (~FLAG_OVERFLOW)
#define setsign() status |= FLAG_SIGN
#define clearsign() status &= (~FLAG_SIGN)


//flag calculation macros
#define zerocalc(n) {\
    if ((n) & 0x00FF) clearzero();\
        else setzero();\
}

#define signcalc(n) {\
    if ((n) & 0x0080) setsign();\
        else clearsign();\
}

#define carrycalc(n) {\
    if ((n) & 0xFF00) setcarry();\
        else clearcarry();\
}

#define overflowcalc(n, m, o) { /* n = result, m = accumulator, o = memory */ \
    if (((n) ^ (uint16_t)(m)) & ((n) ^ (o)) & 0x0080) setoverflow();\
        else clearoverflow();\
}


//6502 CPU registers
uint16_t pc;
uint8_t sp, a, x, y, status = FLAG_CONSTANT;


//helper variables
uint64_t instructions = 0; //keep track of total instructions executed
uint32_t clockticks6502 = 0, clockgoal6502 = 0;
uint16_t oldpc, ea, reladdr, value, result;
uint8_t opcode, oldstatus;

//a few general functions used by various other functions
void push16(uint16_t pushval) {
    write6502(BASE_STACK + sp, (pushval >> 8) & 0xFF);
    write6502(BASE_STACK + ((sp - 1) & 0xFF), pushval & 0xFF);
    sp -= 2;
}

void push8(uint8_t pushval) {
    write6502(BASE_STACK + sp--, pushval);
}

uint16_t pull16() {
    uint16_t temp16;
    temp16 = read6502(BASE_STACK + ((sp + 1) & 0xFF)) | ((uint16_t)read6502(BASE_STACK + ((sp + 2) & 0xFF)) << 8);
    sp += 2;
    return(temp16);
}

uint8_t pull8() {
    return (read6502(BASE_STACK + ++sp));
}

void reset6502() {
    pc = (uint16_t)read6502(0xFFFC) | ((uint16_t)read6502(0xFFFD) << 8);
    a = 0;
    x = 0;
    y = 0;
    sp = 0xFD;
    // status |= FLAG_CONSTANT;
    status = FLAG_CONSTANT;
}


static const uint8_t addrtable_ref[256];
static void (*addrtable[13])();
static const uint8_t optable_ref[256];
static void (*optable[65])();
uint8_t penaltyop, penaltyaddr;

//addressing mode functions, calculates effective addresses
static void imp() { //implied
    volatile int test = 0;
    test = 1;
}

static void acc() { //accumulator
}

static void imm() { //immediate
    ea = pc++;
}

static void zp() { //zero-page
    ea = (uint16_t)read6502((uint16_t)pc++);
}

static void zpx() { //zero-page,X
    ea = ((uint16_t)read6502((uint16_t)pc++) + (uint16_t)x) & 0xFF; //zero-page wraparound
}

static void zpy() { //zero-page,Y
    ea = ((uint16_t)read6502((uint16_t)pc++) + (uint16_t)y) & 0xFF; //zero-page wraparound
}

static void rel() { //relative for branch ops (8-bit immediate value, sign-extended)
    reladdr = (uint16_t)read6502(pc++);
    if (reladdr & 0x80) reladdr |= 0xFF00;
}

static void abso() { //absolute
    ea = (uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8);
    pc += 2;
}

static void absx() { //absolute,X
    uint16_t startpage;
    ea = ((uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8));
    startpage = ea & 0xFF00;
    ea += (uint16_t)x;

    if (startpage != (ea & 0xFF00)) { //one cycle penlty for page-crossing on some opcodes
        penaltyaddr = 1;
    }

    pc += 2;
}

static void absy() { //absolute,Y
    uint16_t startpage;
    ea = ((uint16_t)read6502(pc) | ((uint16_t)read6502(pc+1) << 8));
    startpage = ea & 0xFF00;
    ea += (uint16_t)y;

    if (startpage != (ea & 0xFF00)) { //one cycle penlty for page-crossing on some opcodes
        penaltyaddr = 1;
    }

    pc += 2;
}

static void ind() { //indirect
    uint16_t eahelp, eahelp2;
    eahelp = (uint16_t)read6502(pc) | (uint16_t)((uint16_t)read6502(pc+1) << 8);
    eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //replicate 6502 page-boundary wraparound bug
    ea = (uint16_t)read6502(eahelp) | ((uint16_t)read6502(eahelp2) << 8);
    pc += 2;
}

static void indx() { // (indirect,X)
    uint16_t eahelp;
    eahelp = (uint16_t)(((uint16_t)read6502(pc++) + (uint16_t)x) & 0xFF); //zero-page wraparound for table pointer
    ea = (uint16_t)read6502(eahelp & 0x00FF) | ((uint16_t)read6502((eahelp+1) & 0x00FF) << 8);
}

static void indy() { // (indirect),Y
    uint16_t eahelp, eahelp2, startpage;
    eahelp = (uint16_t)read6502(pc++);
    eahelp2 = (eahelp & 0xFF00) | ((eahelp + 1) & 0x00FF); //zero-page wraparound
    ea = (uint16_t)read6502(eahelp) | ((uint16_t)read6502(eahelp2) << 8);
    startpage = ea & 0xFF00;
    ea += (uint16_t)y;

    if (startpage != (ea & 0xFF00)) { //one cycle penlty for page-crossing on some opcodes
        penaltyaddr = 1;
    }
}

static uint16_t getvalue() {
    if (addrtable[opcode] == acc) return((uint16_t)a);
        else return((uint16_t)read6502(ea));
}

static void putvalue(uint16_t saveval) {
    if (addrtable[opcode] == acc) a = (uint8_t)(saveval & 0x00FF);
        else write6502(ea, (saveval & 0x00FF));
}


//instruction handler functions
static void adc() {
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a + value + (uint16_t)(status & FLAG_CARRY);

    carrycalc(result);
    zerocalc(result);
    overflowcalc(result, a, value);
    signcalc(result);

    #ifndef NES_CPU
    if (status & FLAG_DECIMAL) {
        clearcarry();

        if ((a & 0x0F) > 0x09) {
            a += 0x06;
        }
        if ((a & 0xF0) > 0x90) {
            a += 0x60;
            setcarry();
        }

        clockticks6502++;
    }
    #endif

    saveaccum(result);
}

static void and() {
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a & value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

static void asl() {
    value = getvalue();
    result = value << 1;

    carrycalc(result);
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void bcc() {
    if ((status & FLAG_CARRY) == 0) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void bcs() {
    if ((status & FLAG_CARRY) == FLAG_CARRY) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void beq() {
    if ((status & FLAG_ZERO) == FLAG_ZERO) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void bit() {
    value = getvalue();
    result = (uint16_t)a & value;

    zerocalc(result);
    status = (status & 0x3F) | (uint8_t)(value & 0xC0);
}

static void bmi() {
    if ((status & FLAG_SIGN) == FLAG_SIGN) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void bne() {
    if ((status & FLAG_ZERO) == 0) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void bpl() {
    if ((status & FLAG_SIGN) == 0) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void brk() {
    pc++;
    push16(pc); //push next instruction address onto stack
    push8(status | FLAG_BREAK); //push CPU status to stack
    setinterrupt(); //set interrupt flag
    pc = (uint16_t)read6502(0xFFFE) | ((uint16_t)read6502(0xFFFF) << 8);
}

static void bvc() {
    if ((status & FLAG_OVERFLOW) == 0) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void bvs() {
    if ((status & FLAG_OVERFLOW) == FLAG_OVERFLOW) {
        oldpc = pc;
        pc += reladdr;
        if ((oldpc & 0xFF00) != (pc & 0xFF00)) clockticks6502 += 2; //check if jump crossed a page boundary
            else clockticks6502++;
    }
}

static void clc() {
    clearcarry();
}

static void cld() {
    cleardecimal();
}

static void cli() {
    clearinterrupt();
}

static void clv() {
    clearoverflow();
}

static void cmp() {
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a - value;

    if (a >= (uint8_t)(value & 0x00FF)) setcarry();
        else clearcarry();
    if (a == (uint8_t)(value & 0x00FF)) setzero();
        else clearzero();
    signcalc(result);
}

static void cpx() {
    value = getvalue();
    result = (uint16_t)x - value;

    if (x >= (uint8_t)(value & 0x00FF)) setcarry();
        else clearcarry();
    if (x == (uint8_t)(value & 0x00FF)) setzero();
        else clearzero();
    signcalc(result);
}

static void cpy() {
    value = getvalue();
    result = (uint16_t)y - value;

    if (y >= (uint8_t)(value & 0x00FF)) setcarry();
        else clearcarry();
    if (y == (uint8_t)(value & 0x00FF)) setzero();
        else clearzero();
    signcalc(result);
}

static void dec() {
    value = getvalue();
    result = value - 1;

    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void dex() {
    x--;

    zerocalc(x);
    signcalc(x);
}

static void dey() {
    y--;

    zerocalc(y);
    signcalc(y);
}

static void eor() {
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a ^ value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

static void inc() {
    value = getvalue();
    result = value + 1;

    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void inx() {
    x++;

    zerocalc(x);
    signcalc(x);
}

static void iny() {
    y++;

    zerocalc(y);
    signcalc(y);
}

static void jmp() {
    pc = ea;
}

static void jsr() {
    push16(pc - 1);
    pc = ea;
}

static void lda() {
    penaltyop = 1;
    value = getvalue();
    a = (uint8_t)(value & 0x00FF);

    zerocalc(a);
    signcalc(a);
}

static void ldx() {
    penaltyop = 1;
    value = getvalue();
    x = (uint8_t)(value & 0x00FF);

    zerocalc(x);
    signcalc(x);
}

static void ldy() {
    penaltyop = 1;
    value = getvalue();
    y = (uint8_t)(value & 0x00FF);

    zerocalc(y);
    signcalc(y);
}

static void lsr() {
    value = getvalue();
    result = value >> 1;

    if (value & 1) setcarry();
        else clearcarry();
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void nop() {
    switch (opcode) {
        case 0x1C:
        case 0x3C:
        case 0x5C:
        case 0x7C:
        case 0xDC:
        case 0xFC:
            penaltyop = 1;
            break;
    }
}

static void ora() {
    penaltyop = 1;
    value = getvalue();
    result = (uint16_t)a | value;

    zerocalc(result);
    signcalc(result);

    saveaccum(result);
}

static void pha() {
    push8(a);
}

static void php() {
    push8(status | FLAG_BREAK);
}

static void pla() {
    a = pull8();

    zerocalc(a);
    signcalc(a);
}

static void plp() {
    status = pull8() | FLAG_CONSTANT;
}

static void rol() {
    value = getvalue();
    result = (value << 1) | (status & FLAG_CARRY);

    carrycalc(result);
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void ror() {
    value = getvalue();
    result = (value >> 1) | ((status & FLAG_CARRY) << 7);

    if (value & 1) setcarry();
        else clearcarry();
    zerocalc(result);
    signcalc(result);

    putvalue(result);
}

static void rti() {
    status = pull8();
    value = pull16();
    pc = value;
}

static void rts() {
    value = pull16();
    pc = value + 1;
}

static void sbc() {
    penaltyop = 1;
    value = getvalue() ^ 0x00FF;
    result = (uint16_t)a + value + (uint16_t)(status & FLAG_CARRY);

    carrycalc(result);
    zerocalc(result);
    overflowcalc(result, a, value);
    signcalc(result);

    #ifndef NES_CPU
    if (status & FLAG_DECIMAL) {
        clearcarry();

        a -= 0x66;
        if ((a & 0x0F) > 0x09) {
            a += 0x06;
        }
        if ((a & 0xF0) > 0x90) {
            a += 0x60;
            setcarry();
        }

        clockticks6502++;
    }
    #endif

    saveaccum(result);
}

static void sec() {
    setcarry();
}

static void sed() {
    setdecimal();
}

static void sei() {
    setinterrupt();
}

static void sta() {
    putvalue(a);
}

static void stx() {
    putvalue(x);
}

static void sty() {
    putvalue(y);
}

static void tax() {
    x = a;

    zerocalc(x);
    signcalc(x);
}

static void tay() {
    y = a;

    zerocalc(y);
    signcalc(y);
}

static void tsx() {
    x = sp;

    zerocalc(x);
    signcalc(x);
}

static void txa() {
    a = x;

    zerocalc(a);
    signcalc(a);
}

static void txs() {
    sp = x;
}

static void tya() {
    a = y;

    zerocalc(a);
    signcalc(a);
}

//undocumented instructions
#ifdef UNDOCUMENTED
    static void lax() {
        lda();
        ldx();
    }

    static void sax() {
        sta();
        stx();
        putvalue(a & x);
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void dcp() {
        dec();
        cmp();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void isb() {
        inc();
        sbc();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void slo() {
        asl();
        ora();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void rla() {
        rol();
        and();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void sre() {
        lsr();
        eor();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }

    static void rra() {
        ror();
        adc();
        if (penaltyop && penaltyaddr) clockticks6502--;
    }
#else
    #define lax nop
    #define sax nop
    #define dcp nop
    #define isb nop
    #define slo nop
    #define rla nop
    #define sre nop
    #define rra nop
#endif

static void (*addrtable[13])() = {
    imp,
    acc,
    imm,
    zp,
    zpx,
    zpy,
    rel,
    abso,
    absx,
    absy,
    ind,
    indx,
    indy
};

#define ADDR_IMP 0
#define ADDR_ACC 1
#define ADDR_IMM 2
#define ADDR_ZP 3
#define ADDR_ZPX 4
#define ADDR_ZPY 5
#define ADDR_REL 6
#define ADDR_ABSO 7
#define ADDR_ABSX 8
#define ADDR_ABSY 9
#define ADDR_IND 10
#define ADDR_INDX 11
#define ADDR_INDY 12

static const uint8_t addrtable_ref[256] PROGMEM = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */     ADDR_IMP, ADDR_INDX,  ADDR_IMP, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_ACC,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* 0 */
/* 1 */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, /* 1 */
/* 2 */    ADDR_ABSO, ADDR_INDX,  ADDR_IMP, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_ACC,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* 2 */
/* 3 */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, /* 3 */
/* 4 */     ADDR_IMP, ADDR_INDX,  ADDR_IMP, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_ACC,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* 4 */
/* 5 */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, /* 5 */
/* 6 */     ADDR_IMP, ADDR_INDX,  ADDR_IMP, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_ACC,  ADDR_IMM,  ADDR_IND, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* 6 */
/* 7 */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, /* 7 */
/* 8 */     ADDR_IMM, ADDR_INDX,  ADDR_IMM, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_IMP,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* 8 */
/* 9 */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPY,  ADDR_ZPY,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSY, ADDR_ABSY, /* 9 */
/* A */     ADDR_IMM, ADDR_INDX,  ADDR_IMM, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_IMP,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* A */
/* B */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPY,  ADDR_ZPY,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSY, ADDR_ABSY, /* B */
/* C */     ADDR_IMM, ADDR_INDX,  ADDR_IMM, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_IMP,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* C */
/* D */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, /* D */
/* E */     ADDR_IMM, ADDR_INDX,  ADDR_IMM, ADDR_INDX,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,   ADDR_ZP,  ADDR_IMP,  ADDR_IMM,  ADDR_IMP,  ADDR_IMM, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, ADDR_ABSO, /* E */
/* F */     ADDR_REL, ADDR_INDY,  ADDR_IMP, ADDR_INDY,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_ZPX,  ADDR_IMP, ADDR_ABSY,  ADDR_IMP, ADDR_ABSY, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX, ADDR_ABSX  /* F */
};

static void (*optable[65])() = {
    adc, and, asl,      // 1, 2, 3,
    bcc, bcs, beq, bit, // 4, 5, 6, 7,
    bmi, bne, bpl, brk, // 4, 5, 6, 7,
    bvc, bvs, clc, cld, // 4, 5, 6, 7,
    cli, clv, cmp, cpx,
    cpy, dcp, dec, dex,
    dey, eor, inc, inx,
    iny, isb, jmp,
    jsr, lax, lda, ldx,
    ldy, lsr, nop, ora,
    pha, php, pla, plp,
    rla, rol, ror, rra,
    rti, rts, sax, sbc,
    sec, sed, sei, slo,
    sre, sta, stx, sty,
    tax, tay, tsx, txa,
    txs, tya
};

#define OP_ADC 0
#define OP_AND 1
#define OP_ASL 2
#define OP_BCC 3
#define OP_BCS 4
#define OP_BEQ 5
#define OP_BIT 6
#define OP_BMI 7
#define OP_BNE 8
#define OP_BPL 9
#define OP_BRK 10
#define OP_BVC 11
#define OP_BVS 12
#define OP_CLC 13
#define OP_CLD 14
#define OP_CLI 15
#define OP_CLV 16
#define OP_CMP 17
#define OP_CPX 18
#define OP_CPY 19
#define OP_DCP 20
#define OP_DEC 21
#define OP_DEX 22
#define OP_DEY 23
#define OP_EOR 24
#define OP_INC 25
#define OP_INX 26
#define OP_INY 27
#define OP_ISB 28
#define OP_JMP 29
#define OP_JSR 30
#define OP_LAX 31
#define OP_LDA 32
#define OP_LDX 33
#define OP_LDY 34
#define OP_LSR 35
#define OP_NOP 36
#define OP_ORA 37
#define OP_PHA 38
#define OP_PHP 39
#define OP_PLA 40
#define OP_PLP 41
#define OP_RLA 42
#define OP_ROL 43
#define OP_ROR 44
#define OP_RRA 45
#define OP_RTI 46
#define OP_RTS 47
#define OP_SAX 48
#define OP_SBC 49
#define OP_SEC 50
#define OP_SED 51
#define OP_SEI 52
#define OP_SLO 53
#define OP_SRE 54
#define OP_STA 55
#define OP_STX 56
#define OP_STY 57
#define OP_TAX 58
#define OP_TAY 59
#define OP_TSX 60
#define OP_TXA 61
#define OP_TXS 62
#define OP_TYA 63

static const uint8_t optable_ref[256] PROGMEM = {
/*        |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |   8   |   9   |   A   |   B   |   C   |   D   |   E   |   F   |     */
/* 0 */    OP_BRK, OP_ORA, OP_NOP, OP_SLO, OP_NOP, OP_ORA, OP_ASL, OP_SLO, OP_PHP, OP_ORA, OP_ASL, OP_NOP, OP_NOP, OP_ORA, OP_ASL, OP_SLO, /* 0 */
/* 1 */    OP_BPL, OP_ORA, OP_NOP, OP_SLO, OP_NOP, OP_ORA, OP_ASL, OP_SLO, OP_CLC, OP_ORA, OP_NOP, OP_SLO, OP_NOP, OP_ORA, OP_ASL, OP_SLO, /* 1 */
/* 2 */    OP_JSR, OP_AND, OP_NOP, OP_RLA, OP_BIT, OP_AND, OP_ROL, OP_RLA, OP_PLP, OP_AND, OP_ROL, OP_NOP, OP_BIT, OP_AND, OP_ROL, OP_RLA, /* 2 */
/* 3 */    OP_BMI, OP_AND, OP_NOP, OP_RLA, OP_NOP, OP_AND, OP_ROL, OP_RLA, OP_SEC, OP_AND, OP_NOP, OP_RLA, OP_NOP, OP_AND, OP_ROL, OP_RLA, /* 3 */
/* 4 */    OP_RTI, OP_EOR, OP_NOP, OP_SRE, OP_NOP, OP_EOR, OP_LSR, OP_SRE, OP_PHA, OP_EOR, OP_LSR, OP_NOP, OP_JMP, OP_EOR, OP_LSR, OP_SRE, /* 4 */
/* 5 */    OP_BVC, OP_EOR, OP_NOP, OP_SRE, OP_NOP, OP_EOR, OP_LSR, OP_SRE, OP_CLI, OP_EOR, OP_NOP, OP_SRE, OP_NOP, OP_EOR, OP_LSR, OP_SRE, /* 5 */
/* 6 */    OP_RTS, OP_ADC, OP_NOP, OP_RRA, OP_NOP, OP_ADC, OP_ROR, OP_RRA, OP_PLA, OP_ADC, OP_ROR, OP_NOP, OP_JMP, OP_ADC, OP_ROR, OP_RRA, /* 6 */
/* 7 */    OP_BVS, OP_ADC, OP_NOP, OP_RRA, OP_NOP, OP_ADC, OP_ROR, OP_RRA, OP_SEI, OP_ADC, OP_NOP, OP_RRA, OP_NOP, OP_ADC, OP_ROR, OP_RRA, /* 7 */
/* 8 */    OP_NOP, OP_STA, OP_NOP, OP_SAX, OP_STY, OP_STA, OP_STX, OP_SAX, OP_DEY, OP_NOP, OP_TXA, OP_NOP, OP_STY, OP_STA, OP_STX, OP_SAX, /* 8 */
/* 9 */    OP_BCC, OP_STA, OP_NOP, OP_NOP, OP_STY, OP_STA, OP_STX, OP_SAX, OP_TYA, OP_STA, OP_TXS, OP_NOP, OP_NOP, OP_STA, OP_NOP, OP_NOP, /* 9 */
/* A */    OP_LDY, OP_LDA, OP_LDX, OP_LAX, OP_LDY, OP_LDA, OP_LDX, OP_LAX, OP_TAY, OP_LDA, OP_TAX, OP_NOP, OP_LDY, OP_LDA, OP_LDX, OP_LAX, /* A */
/* B */    OP_BCS, OP_LDA, OP_NOP, OP_LAX, OP_LDY, OP_LDA, OP_LDX, OP_LAX, OP_CLV, OP_LDA, OP_TSX, OP_LAX, OP_LDY, OP_LDA, OP_LDX, OP_LAX, /* B */
/* C */    OP_CPY, OP_CMP, OP_NOP, OP_DCP, OP_CPY, OP_CMP, OP_DEC, OP_DCP, OP_INY, OP_CMP, OP_DEX, OP_NOP, OP_CPY, OP_CMP, OP_DEC, OP_DCP, /* C */
/* D */    OP_BNE, OP_CMP, OP_NOP, OP_DCP, OP_NOP, OP_CMP, OP_DEC, OP_DCP, OP_CLD, OP_CMP, OP_NOP, OP_DCP, OP_NOP, OP_CMP, OP_DEC, OP_DCP, /* D */
/* E */    OP_CPX, OP_SBC, OP_NOP, OP_ISB, OP_CPX, OP_SBC, OP_INC, OP_ISB, OP_INX, OP_SBC, OP_NOP, OP_SBC, OP_CPX, OP_SBC, OP_INC, OP_ISB, /* E */
/* F */    OP_BEQ, OP_SBC, OP_NOP, OP_ISB, OP_NOP, OP_SBC, OP_INC, OP_ISB, OP_SED, OP_SBC, OP_NOP, OP_ISB, OP_NOP, OP_SBC, OP_INC, OP_ISB  /* F */
};

static const uint8_t ticktable[256] PROGMEM = {
/*        |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |  9  |  A  |  B  |  C  |  D  |  E  |  F  |     */
/* 0 */      7,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    4,    4,    6,    6,  /* 0 */
/* 1 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 1 */
/* 2 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    4,    4,    6,    6,  /* 2 */
/* 3 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 3 */
/* 4 */      6,    6,    2,    8,    3,    3,    5,    5,    3,    2,    2,    2,    3,    4,    6,    6,  /* 4 */
/* 5 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 5 */
/* 6 */      6,    6,    2,    8,    3,    3,    5,    5,    4,    2,    2,    2,    5,    4,    6,    6,  /* 6 */
/* 7 */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* 7 */
/* 8 */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* 8 */
/* 9 */      2,    6,    2,    6,    4,    4,    4,    4,    2,    5,    2,    5,    5,    5,    5,    5,  /* 9 */
/* A */      2,    6,    2,    6,    3,    3,    3,    3,    2,    2,    2,    2,    4,    4,    4,    4,  /* A */
/* B */      2,    5,    2,    5,    4,    4,    4,    4,    2,    4,    2,    4,    4,    4,    4,    4,  /* B */
/* C */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* C */
/* D */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7,  /* D */
/* E */      2,    6,    2,    8,    3,    3,    5,    5,    2,    2,    2,    2,    4,    4,    6,    6,  /* E */
/* F */      2,    5,    2,    8,    4,    4,    6,    6,    2,    4,    2,    7,    4,    4,    7,    7   /* F */
};


void nmi6502() {
    push16(pc);
    push8(status);
    status |= FLAG_INTERRUPT;
    pc = (uint16_t)read6502(0xFFFA) | ((uint16_t)read6502(0xFFFB) << 8);
}

void irq6502() {
    push16(pc);
    push8(status);
    status |= FLAG_INTERRUPT;
    pc = (uint16_t)read6502(0xFFFE) | ((uint16_t)read6502(0xFFFF) << 8);
}

uint8_t callexternal = 0;
void (*loopexternal)();

void exec6502(uint32_t tickcount) {
    clockgoal6502 += tickcount;

    while (clockticks6502 < clockgoal6502) {
        opcode = read6502(pc++);
        
        penaltyop = 0;
        penaltyaddr = 0;

        (*addrtable[pgm_read_byte(addrtable_ref + opcode)])();
        (*optable[pgm_read_byte(optable_ref + opcode)])();
        clockticks6502 += pgm_read_byte(ticktable + opcode);
        if (penaltyop && penaltyaddr) clockticks6502++;

        instructions++;

        if (callexternal) (*loopexternal)();
    }

}

void step6502() {
    opcode = read6502(pc++);

    penaltyop = 0;
    penaltyaddr = 0;

    (*addrtable[pgm_read_byte(addrtable_ref + opcode)])();
    (*optable[pgm_read_byte(optable_ref + opcode)])();
    clockticks6502 += pgm_read_byte(ticktable + opcode);
    if (penaltyop && penaltyaddr) clockticks6502++;
    clockgoal6502 = clockticks6502;

    instructions++;

    if (callexternal) (*loopexternal)();
}

void hookexternal(void *funcptr) {
    if (funcptr != (void *)NULL) {
        loopexternal = funcptr;
        callexternal = 1;
    } else callexternal = 0;
}
