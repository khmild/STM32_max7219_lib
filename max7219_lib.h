/**
 * @file max7219_lib.h
 * @author Denys Khmil
 * @brief This file contents the max7219 class
 */
#ifndef MAX7219_LIB
#define MAX7219_LIB

#include "main.h"

typedef enum{
    DIG0 = 0x01,
    DIG1 = 0x02,
    DIG2 = 0x03,
    DIG3 = 0x04,
    DIG4 = 0x05,
    DIG5 = 0x06,
    DIG6 = 0x07,
    DIG7 = 0x08
} DigitsAddr;

typedef enum{
    DECODE_MODE = 0x09,
    INTENSITY = 0x0A,
    SCAN_LIMIT = 0x0B,
    SHUTDOWN = 0x0C,
    DISPLAY_TEST = 0x0F
} SettingsAddr;

typedef enum{
    SHUTDOWN_MODE = 0,
    NORMAL_MODE = 1
}ShutdownModes;

typedef enum{
    NO_DECODE_7_0 = 0x00,
    CODE_B_FOR_0 = 0x01,
    CODE_B_FOR_3_TO_0 = 0x0f,
    CODE_B_FOR_7_TO_0 = 0xff
}DecodeModes;

typedef enum{
    INTENS_MIN = 0x00,
    INTENS_30 = 0x05,
    INTENS_60 = 0x0a,
    INTENS_MAX = 0x0f
}IntensityModes;

typedef enum{
    SYMBOL_MINUS = 0x0a,
    SYMBOL_E = 0x0b,
    SYMBOL_H = 0x0c,
    SYMBOL_L = 0x0d,
    SYMBOL_P = 0x0e
}SpecialSymbols;

class max7219
{
public:
    max7219(SPI_HandleTypeDef _spi, GPIO_TypeDef *_cs_port, uint8_t _cs_pin);
    void printInt(const int32_t data);
    void printDigit(DigitsAddr digit, uint8_t value);
    void printSymbol(DigitsAddr digit, SpecialSymbols symbol);
    void printDot(DigitsAddr digit);
    void setBrightness(IntensityModes brightness);
    void setDecodeMode(DecodeModes mode);
    void setPowerMode(ShutdownModes mode);
    void clear();

private:    
    void sendToAddr(uint8_t addr, uint8_t data);
    void Initialize();
    void set_CS(uint8_t state);
    
    
    SPI_HandleTypeDef spi;
    GPIO_TypeDef *cs_port;
    uint8_t cs_pin;
    
    static uint8_t gpio_rcc_clk;
};




#endif