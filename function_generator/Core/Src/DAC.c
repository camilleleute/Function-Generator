/*
 * DAC.c
 *
 *  Created on: Oct 28, 2024
 *      Author: camil
 */
#include "main.h"
#define UPPER_MASK 0x3000


void DAC_init(void){
	// Write proper GPIO registers: Configure GPIO for MOSI, MISO and SCK pins
	RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
	//SPI clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE7);
	GPIOA->MODER |= (2 << GPIO_MODER_MODE4_Pos | 2 << GPIO_MODER_MODE5_Pos
					| 2 << GPIO_MODER_MODE7_Pos);
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT7);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED4 | GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED7);
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD7);
	// configure PA4/NSS, PA5/SCLK, PA7/MOSI for SPI1/AF5
	GPIOA->AFR[0] &= ~((0xF << GPIO_AFRL_AFSEL4_Pos)|(0xF << GPIO_AFRL_AFSEL5_Pos)| (0xF << GPIO_AFRL_AFSEL7_Pos)); // clear
	GPIOA->AFR[0] |= ((0x5 << GPIO_AFRL_AFSEL4_Pos)| (0x5 << GPIO_AFRL_AFSEL5_Pos)| (0x5 << GPIO_AFRL_AFSEL7_Pos)); // set
	// baud rate of Fpclk/2
	SPI1->CR1 &= ~SPI_CR1_BR;
	//clock in 00 mode
	SPI1->CR1 &= ~(SPI_CR1_CPOL | SPI_CR1_CPHA);
	// simplex mode, send data only
	SPI1->CR1 &= ~SPI_CR1_RXONLY;
	SPI1->CR1 |= SPI_CR1_BIDIOE;
	// MSB First
	SPI1->CR1 &= ~(SPI_CR1_LSBFIRST);
	// Hardware manage the CS
	SPI1->CR1 &= ~(SPI_CR1_SSM);
	// Controller configuration
	SPI1->CR1 |= (SPI_CR1_MSTR);
	// 16 Bit data transfer
	SPI1->CR2 |= SPI_CR2_DS;
	// Disable hardware NSS management
	SPI1->CR2 |= SPI_CR2_SSOE;
	SPI1->CR2 |= SPI_CR2_NSSP_Msk;
	//disable interrupts
	SPI1->CR2 &= ~(SPI_CR2_TXEIE | SPI_CR2_RXNEIE);
	// FRXTH enabled since DS is 12
	SPI1->CR2 &= ~SPI_CR2_FRXTH;
	// enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
}
uint16_t DAC_volt_conv(uint16_t num) {
	// if number exceeds max output, set to max output
	if (num >= 4095) {
			num = 4095;
	}
	// clear upper nibble and set it to config settings
	num &= ~(0xF000);
	num |= UPPER_MASK;
	return num;
}
void DAC_write(uint16_t num) {
	// waiting till done transmitting w/ polling
	while (!(SPI1->SR & SPI_SR_TXE));
	// write to DAC
	SPI1->DR = num;
	// wait till not busy
	while (SPI1->SR & SPI_SR_BSY);
}


