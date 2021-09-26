/**
 * @file max7219_lib.cpp
 * @author Denys Khmil
 * @brief This file contents all the max7219 functions
 */
#include "max7219_lib.h"

/**
 * @brief MAX7219 constructor.
 * @param _spi: SPI instance.
 * @param _cs_port: chip select port.
 * @param _cs_pin: chip select pin.
 */
max7219::max7219(SPI_HandleTypeDef _spi, GPIO_TypeDef *_cs_port, uint8_t _cs_pin){
    /*Save spi instance and CS port and pin*/
    this->spi = _spi;
    this->cs_port = _cs_port;
    this->cs_pin = _cs_pin;

    static uint8_t gpio_rcc_clk = 0;

    /*Initialize CS port as output*/
    if((_cs_port == GPIOA)&!(gpio_rcc_clk&(1<<0))){
		__HAL_RCC_GPIOA_CLK_ENABLE();
		gpio_rcc_clk = gpio_rcc_clk|(1<<0);
	}
	if((_cs_port == GPIOB)&!(gpio_rcc_clk&(1<<1))){
		__HAL_RCC_GPIOB_CLK_ENABLE();
		gpio_rcc_clk = gpio_rcc_clk|(1<<1);
	}
	if((_cs_port == GPIOC)&!(gpio_rcc_clk&(1<<2))){
		__HAL_RCC_GPIOC_CLK_ENABLE();
		gpio_rcc_clk = gpio_rcc_clk|(1<<2);
	}
	if((_cs_port == GPIOD)&!(gpio_rcc_clk&(1<<3))){
		__HAL_RCC_GPIOD_CLK_ENABLE();
		gpio_rcc_clk = gpio_rcc_clk|(1<<3);
	}

    /*GPIO initialization structure*/
    GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure CS pin Output Level*/
	HAL_GPIO_WritePin(_cs_port, _cs_pin, GPIO_PIN_RESET);

	/*Configure GPIO pin*/
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = _cs_pin;
	HAL_GPIO_Init(_cs_port, &GPIO_InitStruct);

	this->Initialize();
}

/**
 * @brief Initialize display by default settings:
 *  - Decode mode: Code B for symbols 7 to 0
 *  - All 8 symbols are active
 *  - Brightness 60%
 * 	- Normal power mode
 */
void max7219::Initialize(){
	/*Use code B for every symbol*/
	this->sendToAddr(DECODE_MODE, CODE_B_FOR_7_TO_0);
	/*All 8 digits are active*/
	this->sendToAddr(SCAN_LIMIT, 0x07);
	/*Set brightness to 60%*/
	this->sendToAddr(INTENSITY, INTENS_60);
	/*Turn on the display*/
	this->sendToAddr(SHUTDOWN, NORMAL_MODE);
}

/**
 * @brief Set chip select high/low
 * @param state: 0/1 - LOW/HIGH
 */
void max7219::set_CS(uint8_t state){
    if (state){
        HAL_GPIO_WritePin(this->cs_port, this->cs_pin, GPIO_PIN_SET);
    }
    else{
        HAL_GPIO_WritePin(this->cs_port, this->cs_pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief Send data to address
 * @param addr: Address
 * @param data: Data
 */
void max7219::sendToAddr(uint8_t addr, uint8_t data){
	this->set_CS(1);
	HAL_SPI_Transmit(&this->spi, &addr, 1, 1000);
	HAL_SPI_Transmit(&this->spi, &data, 1, 1000);
	this->set_CS(0);
}

/**
 * @brief Print one number on specified position.
 * @param digit: Number of digit (position).
 * @param value: Symbol to print.
 */
void max7219::printDigit(DigitsAddr digit, uint8_t value){
	this->sendToAddr(digit, value);
}

/**
 * @brief Print special symbol on specified position.
 * @param digit: Number of digit (position).
 * @param symbol: Symbol to print.
 */
void max7219::printSymbol(DigitsAddr digit, SpecialSymbols symbol){
	this->sendToAddr(digit, symbol);
}

/**
 * @brief Print dot on specified digit.
 * @param digit: Number of digit (position).
 */
void max7219::printDot(DigitsAddr digit){
	this->sendToAddr(digit, 0x8f);
}

/**
 * @brief Set brightness of the display.
 * @param brightness: Brightness mode.
 */
void max7219::setBrightness(IntensityModes brightness){
	this->sendToAddr(INTENSITY, brightness);
}

/**
 * @brief Set decode mode.
 * @param mode: Decode mode.
 */
void max7219::setDecodeMode(DecodeModes mode){
	this->sendToAddr(DECODE_MODE, mode);
}

/**
 * @brief Set power mode.
 * @param mode: Power mode.
 */
void max7219::setPowerMode(ShutdownModes mode){
	this->sendToAddr(SHUTDOWN, mode);
}

/**
 * @brief Print an integer number
 * @param data: number to print
 */
void max7219::printInt(const int32_t data){
	uint8_t minus_flag = 0;
	int16_t value = data;
	/*if negative, set flag and negate*/
	if (data < 0){
		minus_flag = 1;
		value = value*(-1);
	}

	/*if zero, print zero*/
	if (value == 0){
		this->sendToAddr(DIG0, 0);
		return;
	}

	/*print symbols one by one*/
	uint8_t i = 0;
	do{
		this->sendToAddr(++i, value%10);
		value = value/10;
	} while (value);

	if(minus_flag) this->sendToAddr(i+1, 0x0a);
}

/**
 * @brief Clear display
 */
void max7219::clear(){
	for (uint8_t i = 1; i < 9; i++)
	{
		/*Print blanks on each digit*/
		this->sendToAddr(i, 0x0f);
	}
}