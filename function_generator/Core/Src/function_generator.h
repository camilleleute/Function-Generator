/*
 * function_generator.h
 *
 *  Created on: Oct 28, 2024
 *      Author: camil
 */

#ifndef SRC_FUNCTION_GENERATOR_H_
#define SRC_FUNCTION_GENERATOR_H_

#define SAMPLE_RATE 1560
#define THREE_VOLTS 3550


void DAC_write(uint16_t);
static void Keypad_TurnOnCols(void);
static void Keypad_TurnOffCols(void);
void Keypad_Init(void);
int8_t Keypad_GetButton(void);
int8_t Keypad_CalculateButton(int8_t, int8_t);
void DAC_init(void);
uint16_t DAC_volt_conv(uint16_t);
void SystemClock_Config(void);
void TIM2_Init(void);



#endif /* SRC_FUNCTION_GENERATOR_H_ */
