#include <avr/pgmspace.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "duo_travel.h"
#include "machine.h"

// #define PROGRAMMER

#ifndef PROGRAMMER
#include "emulator.h"
#include "editor.h"
#else
#include "programmer.h"
#endif

int main(void) {
    init();
    #ifdef PROGRAMMER
    programmer();
    #else
    loadPageDefs();
    editor();
    #endif
    return 0;
}
