#include "duo_travel.h"
#include <util/delay.h>
#include <avr/pgmspace.h>

#define SCK_PIN_OUTPUT   DDRB |= (1 << DDB5)
#define MISO_PIN_INPUT   DDRB &= ~(1 << DDB4)
#define MOSI_PIN_OUTPUT   DDRB |= (1 << DDB3)

#define EEPROM_CS_PIN_OUTPUT   DDRB |= (1 << DDB1)
#define EEPROM_CS_PIN_HIGH   PORTB |= (1 << PORTB1)
#define EEPROM_CS_PIN_LOW   PORTB &= ~(1 << PORTB1)

#define DISPLAY_CLOCK_PIN_OUTPUT   DDRC |= (1 << DDC4)
#define DISPLAY_CLOCK_PIN_HIGH   PORTC |= (1 << PORTC4)
#define DISPLAY_CLOCK_PIN_LOW   PORTC &= ~(1 << PORTC4)

#define DISPLAY_MODE_PIN_OUTPUT   DDRC |= (1 << DDC5)
#define DISPLAY_MODE_PIN_HIGH   PORTC |= (1 << PORTC5)
#define DISPLAY_MODE_PIN_LOW   PORTC &= ~(1 << PORTC5)

#define DISPLAY_DATA_PINS_OUTPUT   DDRC |= 0x0F
#define DISPLAY_DATA_PINS_WRITE(data)   PORTC = (PORTC & 0xF0) | (data)

int32_t storageAddress = -100;
int8_t lastStorageData;
int8_t repeatKey = -2;

static int8_t receiveSpiInt8() {
    SPDR = 0xFF;
    while (!(SPSR & (1 << SPIF))) {
    
    }
    return SPDR;
}

static void sendSpiInt8(int8_t value) {
    SPDR = value;
    while (!(SPSR & (1 << SPIF))) {
    
    }
}

static void sendDisplayDataInt4(uint8_t data) {
    DISPLAY_CLOCK_PIN_LOW;
    _delay_us(5);
    DISPLAY_DATA_PINS_WRITE(data & 0x0F);
    DISPLAY_CLOCK_PIN_HIGH;
    _delay_us(5);
    DISPLAY_CLOCK_PIN_LOW;
    _delay_us(5);
}

static void sendDisplayData(uint8_t data) {
    sendDisplayDataInt4((data >> 4) & 0x0F);
    sendDisplayDataInt4(data & 0x0F);
}

static void sendDisplayCommandInt4(uint8_t data) {
    DISPLAY_MODE_PIN_LOW;
    sendDisplayDataInt4(data);
    _delay_us(50);
}

void sendDisplayCommand(uint8_t data) {
    DISPLAY_MODE_PIN_LOW;
    sendDisplayData(data);
    _delay_us(50);
}

void sendDisplayCharacter(uint8_t data) {
    DISPLAY_MODE_PIN_HIGH;
    sendDisplayData(data);
    _delay_us(50);
}

void displayCharacter(int8_t posX, int8_t posY, int8_t character) {
    if (posX < 0 || posX >= DISPLAY_WIDTH || posY < 0 || posY >= DISPLAY_HEIGHT) {
        return;
    }
    sendDisplayCommand(0x80 | (posX + posY * 64));
    sendDisplayCharacter(character);
}

int8_t getPressedKey() {
    int8_t tempData;
    BUTTON_INPUT_PINS_MODE(0x01);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        return KEY_SYMBOL_UP;
    }
    if (tempData & 0x02) {
        return KEY_SYMBOL_DOWN;
    }
    if (tempData & 0x04) {
        return KEY_SYMBOL_LEFT;
    }
    if (tempData & 0x08) {
        return KEY_SYMBOL_RIGHT;
    }
    BUTTON_INPUT_PINS_MODE(0x02);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        return KEY_CURSOR_UP;
    }
    if (tempData & 0x02) {
        return KEY_CURSOR_DOWN;
    }
    if (tempData & 0x04) {
        return KEY_CURSOR_LEFT;
    }
    if (tempData & 0x08) {
        return KEY_CURSOR_RIGHT;
    }
    BUTTON_INPUT_PINS_MODE(0x04);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        return KEY_DELETE;
    }
    if (tempData & 0x02) {
        return KEY_CLEAR_LINE;
    }
    if (tempData & 0x04) {
        return KEY_ESCAPE;
    }
    BUTTON_INPUT_PINS_MODE(0x08);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        return KEY_FINISH;
    }
    if (tempData & 0x02) {
        return KEY_NEWLINE;
    }
    if (tempData & 0x04) {
        return KEY_SELECT_OPTION;
    }
    return -1;
}

uint16_t getPressedKeys() {
    uint16_t result = 0;
    int8_t tempData;
    BUTTON_INPUT_PINS_MODE(0x01);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        result |= 1 << KEY_SYMBOL_UP;
    }
    if (tempData & 0x02) {
        result |= 1 << KEY_SYMBOL_DOWN;
    }
    if (tempData & 0x04) {
        result |= 1 << KEY_SYMBOL_LEFT;
    }
    if (tempData & 0x08) {
        result |= 1 << KEY_SYMBOL_RIGHT;
    }
    BUTTON_INPUT_PINS_MODE(0x02);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        result |= 1 << KEY_CURSOR_UP;
    }
    if (tempData & 0x02) {
        result |= 1 << KEY_CURSOR_DOWN;
    }
    if (tempData & 0x04) {
        result |= 1 << KEY_CURSOR_LEFT;
    }
    if (tempData & 0x08) {
        result |= 1 << KEY_CURSOR_RIGHT;
    }
    BUTTON_INPUT_PINS_MODE(0x04);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        result |= 1 << KEY_DELETE;
    }
    if (tempData & 0x02) {
        result |= 1 << KEY_CLEAR_LINE;
    }
    if (tempData & 0x04) {
        result |= 1 << KEY_ESCAPE;
    }
    BUTTON_INPUT_PINS_MODE(0x08);
    _delay_us(10);
    tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x01) {
        result |= 1 << KEY_FINISH;
    }
    if (tempData & 0x02) {
        result |= 1 << KEY_NEWLINE;
    }
    if (tempData & 0x04) {
        result |= 1 << KEY_SELECT_OPTION;
    }
    return result;
}

bool escapeKeyIsPressed() {
    BUTTON_INPUT_PINS_MODE(0x04);
    _delay_us(10);
    int8_t tempData = ~BUTTON_OUTPUT_PIN_READ;
    if (tempData & 0x04) {
        return true;
    } else {
        return false;
    }
}

int8_t getKey() {
    int8_t tempKey = getPressedKey();
    _delay_ms(5);
    int16_t tempDelay;
    if (tempKey >= 0) {
        if (tempKey == repeatKey) {
            tempDelay = 20;
        } else {
            tempDelay = 150;
        }
    } else {
        repeatKey = -2;
    }
    while (true) {
        int8_t tempKey2 = getPressedKey();
        _delay_ms(5);
        if (tempKey2 < 0) {
            tempKey = tempKey2;
            repeatKey = -2;
        } else if (tempKey2 == tempKey) {
            tempDelay -= 1;
            if (tempDelay <= 0) {
                repeatKey = tempKey2;
                return tempKey2;
            }
        } else {
            return tempKey2;
        }
    }
}

static void sendAddressToStorage(int32_t address) {
    sendSpiInt8((address & 0x0000FF00) >> 8);
    sendSpiInt8(address & 0x000000FF);
}

void readStorage(void *destination, int32_t address, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    int32_t index = 0;
    if (address == storageAddress - 1) {
        *(int8_t *)(destination + index) = lastStorageData;
        index += 1;
    } else if (address != storageAddress) {
        EEPROM_CS_PIN_HIGH;
        _delay_us(5);
        EEPROM_CS_PIN_LOW;
        sendSpiInt8(0x03);
        sendAddressToStorage(address);
        storageAddress = address;
    }
    while (index < amount) {
        int8_t tempData = receiveSpiInt8();
        *(int8_t *)(destination + index) = tempData;
        lastStorageData = tempData;
        storageAddress += 1;
        index += 1;
    }
}

void writeStorage(int32_t address, const void *source, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    storageAddress = address;
    int8_t tempShouldWrite = true;
    int32_t index = 0;
    while (tempShouldWrite) {
        EEPROM_CS_PIN_HIGH;
        _delay_us(5);
        EEPROM_CS_PIN_LOW;
        sendSpiInt8(0x06);
        EEPROM_CS_PIN_HIGH;
        _delay_us(5);
        EEPROM_CS_PIN_LOW;
        sendSpiInt8(0x02);
        sendAddressToStorage(storageAddress);
        while (true) {
            if (index >= amount) {
                tempShouldWrite = false;
                break;
            }
            sendSpiInt8(*(int8_t *)(source + index));
            storageAddress += 1;
            index += 1;
            if (storageAddress % 64 == 0) {
                break;
            }
        }
        EEPROM_CS_PIN_HIGH;
        _delay_ms(8);
    }
    storageAddress = -100;
}

void writeStorageEeprom(int32_t address, const void *source, int32_t amount) {
    if (amount <= 0) {
        return;
    }
    storageAddress = address;
    int8_t tempShouldWrite = true;
    int32_t index = 0;
    while (tempShouldWrite) {
        EEPROM_CS_PIN_HIGH;
        _delay_us(5);
        EEPROM_CS_PIN_LOW;
        sendSpiInt8(0x06);
        EEPROM_CS_PIN_HIGH;
        _delay_us(5);
        EEPROM_CS_PIN_LOW;
        sendSpiInt8(0x02);
        sendAddressToStorage(storageAddress);
        while (true) {
            if (index >= amount) {
                tempShouldWrite = false;
                break;
            }
            sendSpiInt8(pgm_read_byte(source + index));
            storageAddress += 1;
            index += 1;
            if (storageAddress % 64 == 0) {
                break;
            }
        }
        EEPROM_CS_PIN_HIGH;
        _delay_ms(8);
    }
    storageAddress = -100;
}

int8_t readStorageInt8(int32_t address) {
    int8_t output;
    readStorage(&output, address, 1);
    return output;
}

void clearDisplay() {
    int32_t tempPosY = 0;
    while (tempPosY < DISPLAY_HEIGHT) {
        int32_t tempPosX = 0;
        while (tempPosX < DISPLAY_WIDTH) {
            displayCharacter(tempPosX, tempPosY, ' ');
            tempPosX += 1;
        }
        tempPosY += 1;
    }
}

void clearDisplayRegion(int8_t posX, int8_t posY, int8_t width) {
    int8_t tempOffset = 0;
    while (tempOffset < width) {
        displayCharacter(posX + tempOffset, posY, ' ');
        tempOffset += 1;
    }
}

void debugPrint(int16_t value) {
    clearDisplay();
    if (value < 0) {
        value = -value;
    }
    int8_t index = 0;
    int16_t tempValue = 10000;
    while (tempValue >= 1) {
        displayCharacter(index, 0, '0' + value / tempValue);
        value %= tempValue;
        index += 1;
        tempValue /= 10;
    }
    getKey();
}

void init() {
    DISPLAY_CLOCK_PIN_LOW;
    BUTTON_INPUT_PINS_LOW;
    BUTTON_OUTPUT_PINS_HIGH;
    
    SCK_PIN_OUTPUT;
    MISO_PIN_INPUT;
    MOSI_PIN_OUTPUT;
    EEPROM_CS_PIN_OUTPUT;
    DISPLAY_CLOCK_PIN_OUTPUT;
    DISPLAY_MODE_PIN_OUTPUT;
    DISPLAY_DATA_PINS_OUTPUT;
    BUTTON_OUTPUT_PINS_INPUT;
    
    // SS.
    DDRB |= (1 << DDB2);
    
    // Configure SPI.
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR |= (1 << SPI2X);
    
    // 4-bit display operation.
    _delay_ms(30);
    sendDisplayCommandInt4(0x03);
    _delay_ms(5);
    sendDisplayCommandInt4(0x03);
    _delay_us(100);
    sendDisplayCommandInt4(0x03);
    _delay_ms(5);
    sendDisplayCommandInt4(0x02);
    // 2 lines.
    sendDisplayCommand(0x28);
    // Turn cursor off.
    sendDisplayCommand(0x08);
    // Clear screen.
    sendDisplayCommand(0x01);
    _delay_ms(5);
    // Don't shift screen.
    sendDisplayCommand(0x06);
    // // Program backslash.
    // sendDisplayCommand(0x48);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x10);
    // sendDisplayCharacter(0x08);
    // sendDisplayCharacter(0x04);
    // sendDisplayCharacter(0x02);
    // sendDisplayCharacter(0x01);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x00);
    // // Program tilde.
    // sendDisplayCommand(0x50);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x09);
    // sendDisplayCharacter(0x15);
    // sendDisplayCharacter(0x12);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x00);
    // sendDisplayCharacter(0x00);
    // Turn display on.
    sendDisplayCommand(0x0C);
}
