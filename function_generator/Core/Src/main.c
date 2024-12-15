/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "function_generator.h"
#include "waveform_LUTs"


// global variables
static int8_t freq_idx = 1;        // Initial frequency
static int8_t duty_cycle = 50;     // Initial duty cycle
static uint32_t idx = 0;		   // Initial index value

// Struct for waveform types
typedef enum {
   SINE,
   SAWTOOTH,
   SQUARE,
   TRIANGLE,
} wave_type;
volatile wave_type wave = SQUARE;    // Initial waveform type


int main(void) {
	 // initialization functions
	 HAL_Init();
	 SystemClock_Config();
	 DAC_init();
	 Keypad_Init();
	 TIM2_Init();
	__enable_irq();


	 while (1) {
		 // debouncing button fetch
		  while(Keypad_GetButton() == -1);
		  // get button value
		  int8_t button = Keypad_GetButton();
		  // wait for release
		  while(Keypad_GetButton() != -1);

		 // set variables based on button press
		  switch (button) {
		  case 1:
			  // 100Hz frequency
			  freq_idx = 1;
			  break;
		  case 2:
			  // 200Hz frequency
			  freq_idx = 2;
			  break;
		  case 3:
			  // 300Hz frequency
			  freq_idx = 3;
			  break;
		  case 4:
			  // 400Hz frequency
			  freq_idx = 4;
			  break;
		  case 5:
			  // 500Hz frequency
			  freq_idx = 5;
			  break;
		  case 6:
			  wave = SINE;
			  break;
		  case 7:
			  wave = TRIANGLE;
			  break;
		  case 8:
			  wave = SAWTOOTH;
			  break;
		  case 9:
			  wave = SQUARE;
			  break;
		  // *
		  case 10:
			  // decrement duty cycle by 105 unless at 10%
			  if (duty_cycle == 10) {
				  continue;
			  } else {
				  duty_cycle -= 10;
			  }
			  break;
		  // 0
		  case 0:
			  // reset to 50% duty cycle
			  duty_cycle = 50;
			  break;
		  // #
		  case 11:
			  // increment duty cycle by 105 unless at 90%
			  if (duty_cycle == 90) {
				  continue;
			  } else {
				  duty_cycle += 10;
			  }
			  break;
		  default:
			  break;
		  }

	}

}

void TIM2_IRQHandler(void){
	// value to be written to DAC
	static uint16_t data = 0;

	if (TIM2->SR & TIM_SR_UIF) {

		// reset idx if out of range
		if (idx >= SAMPLE_RATE){
			idx = 0;
		}

		// get data value based on wave configuration settings
		switch(wave) {
			case SINE:
				data = sine_LUT[idx];
				break;
			case SAWTOOTH:
				data = sawtooth_LUT[idx];
				break;
			case TRIANGLE:
				data = triangle_LUT[idx];
				break;
			case SQUARE:
				// get duty cycle value in sample rate terms
				uint32_t duty = (SAMPLE_RATE * duty_cycle)/100;
				// if indexed value less than sample, then go high, else go low
				if (idx < duty) {
					data = THREE_VOLTS;
				} else {
					data = 0;
				}
				break;
			default:
				break;
		}
		// write corresponding value to DAC
		data = DAC_volt_conv(data);
		DAC_write(data);
		// increment index based on frequency
		idx += freq_idx;
		// clear interrupt flag
		TIM2->SR &= ~TIM_SR_UIF;

	}
}

// FOR 40MHZ SCLK
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;  // 4 MHz MSI
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;                   // Divide by 1
  RCC_OscInitStruct.PLL.PLLN = 20;                  // Multiply by 20 (4 MHz * 20 = 80 MHz)
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;       // Divide by 2 to get 40 MHz
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;       // Optional, for other peripherals
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;       // Optional, for USB, SDIO, etc.
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;  // Use PLL output as system clock
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}







/**
  * @brief System Clock Configuration
  * @retval None
  */


/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}
  /* USER CODE END Error_Handler_Debug */


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
