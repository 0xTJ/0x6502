#ifndef INCLUDE_DUO_TRAVEL_H
#define INCLUDE_DUO_TRAVEL_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 2

#define KEY_CURSOR_LEFT 0
#define KEY_CURSOR_RIGHT 1
#define KEY_CURSOR_UP 2
#define KEY_CURSOR_DOWN 3
#define KEY_SYMBOL_LEFT 4
#define KEY_SYMBOL_RIGHT 5
#define KEY_SYMBOL_UP 6
#define KEY_SYMBOL_DOWN 7
#define KEY_SELECT_OPTION 8
#define KEY_DELETE 9
#define KEY_NEWLINE 10
#define KEY_ESCAPE 11
#define KEY_FINISH 12
#define KEY_CLEAR_LINE 13

#define BUTTON_INPUT_PINS_MODE(mode)   DDRD = (DDRD & 0x0F) | ((int16_t)(mode) << 4)
#define BUTTON_INPUT_PINS_LOW   PORTD &= ~0xF0

#define BUTTON_OUTPUT_PINS_INPUT   DDRD &= ~0x0F
#define BUTTON_OUTPUT_PIN_READ   (PIND & 0x0F)
#define BUTTON_OUTPUT_PINS_HIGH   PORTD |= 0x0F

void sendDisplayCommand(uint8_t data);
void sendDisplayCharacter(uint8_t data);
void displayCharacter(int8_t posX, int8_t posY, int8_t character);
int8_t getPressedKey();
uint16_t getPressedKeys();
bool escapeKeyIsPressed();
int8_t getKey();
void readStorage(void *destination, int32_t address, int32_t amount);
void writeStorage(int32_t address, const void *source, int32_t amount);
void writeStorageEeprom(int32_t address, const void *source, int32_t amount);
int8_t readStorageInt8(int32_t address);
void clearDisplay();
void clearDisplayRegion(int8_t posX, int8_t posY, int8_t width);
void debugPrint(int16_t value);
void init();

#endif  // NOT INCLUDE_DUO_TRAVEL_H
