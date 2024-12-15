/*
 * TIM2.c
 *
 *  Created on: Oct 28, 2024
 *      Author: camil
 */
#include "main.h"
#define INCREMENT 256
#define NVIC_REG_MASK 0x1F


void TIM2_Init(void){
	// enable TIM2 clock
	RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN);
	// set TIM2 as an up counter
	TIM2->CR1 &= ~TIM_CR1_DIR;
	// count for a really really long time
	TIM2->ARR = INCREMENT;
	// enable update event interrupt in TIM2 and CCR1 interrupts
	TIM2->DIER |=(TIM_DIER_UIE );
	// clear interrupt status register for update event and CCR1
	TIM2->SR &= ~(TIM_SR_UIF);
	// start timer
	TIM2->CR1 |= TIM_CR1_CEN;
	// enable TIM2 interrupts
	NVIC->ISER[0] = (1 <<(TIM2_IRQn & NVIC_REG_MASK));
}


