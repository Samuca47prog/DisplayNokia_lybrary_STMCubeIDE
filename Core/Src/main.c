/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "LCD.h"
#include "imagem.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DISP_DELAY 2000
#define DTELA 400; //2s
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

uint16_t delayTela;
uint8_t me_display;
uint8_t tela;

uint16_t a;

LCD_HandleTypeDef hlcd; // handle do display
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI2_Init();
  MX_TIM11_Init();
  /* USER CODE BEGIN 2 */

  // ====================================================================================
  // --- Configurações do hardware do display ---
  	hlcd.hspi = &hspi2;

  	hlcd.CS_Port = NK_CS_GPIO_Port;
  	hlcd.CS_Pin = NK_CS_Pin;

  	hlcd.DC_Port = NK_DC_GPIO_Port;
  	hlcd.DC_Pin = NK_DC_Pin;

  	hlcd.RS_Port = NK_RS_GPIO_Port;
  	hlcd.RS_Pin = NK_RS_Pin;

  	// manda a estrutura de dados com as configurações do display pra inicialização
	LCD5110_init(&hlcd);

	// aula 4
	while(LCD5110_set_XY(0, 0)!=HAL_OK){};
	//while(LCD5110_write(liber_bmp2, 500)!=HAL_OK);


	while(LCD5110_write_str("Hello World")!=HAL_OK){};
	while(LCD5110_set_XY(0, 2)!=HAL_OK){};
	while(LCD5110_write_str("Estou Vivo")!=HAL_OK){};
	//HAL_Delay(DISP_DELAY);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

	tela = 1;
	me_display = 0;
	delayTela = DTELA;

  while (1)
  {

	  if (__HAL_TIM_GET_FLAG(&htim11, TIM_FLAG_UPDATE))
	  {
		  __HAL_TIM_CLEAR_FLAG(&htim11, TIM_FLAG_UPDATE);

			  switch(me_display)
			  {
			  	case 0:
			  		delayTela--;

			  		  if (delayTela == 0)
			  		  {
			  			  delayTela=DTELA;
			  			  me_display++;
			  		  }
			  	break;

			  	case 1:
			  		if (LCD5110_clear()==HAL_OK)
			  		{
			  			me_display++;
			  		}
			  	break;

			  	case 2:
			  		if (LCD5110_set_XY(0,0)==HAL_OK)
			  		{
			  			me_display += tela;
			  		}
			  	break;

			  	case 3:
			  		if (LCD5110_write_str("gremio")==HAL_OK)
			  		{
			  			me_display = 0;
			  			tela++;
			  		}
			  	break;

			  	case 4:
			  		if (LCD5110_write(liber_bmp2, 500)==HAL_OK)
			  		{
			  			me_display = 0;
			  			tela++;
			  		}
			  	break;

			  	case 5:
			  		if (LCD5110_write(liber_bmp3, 500)==HAL_OK)
			  		{
			  			me_display = 0;
			  			tela = 1;
			  		}
			  	break;

			  }
	  }




/*
	   	LCD5110_clear();
	   	LCD5110_set_XY(0,0);
	   	LCD5110_write_str("ABCDEFGHIJKLMNOPWRSTUVXYZ   abcdefghijklmnopqrstuvxyz   1234567890");
	   	HAL_Delay(DISP_DELAY);


		LCD5110_clear();
		LCD5110_set_XY(0,0);
		LCD_write_IT(liber_bmp2,500,1);
		HAL_Delay(DISP_DELAY);


	   	LCD5110_clear();
	   	LCD5110_set_XY(0,0);
	   	LCD_write(liber_bmp3,500,1);
	   	HAL_Delay(DISP_DELAY);
*/


/*
		LCD5110_clear();
		LCD5110_set_XY(0,0);
		for (a=0;a<=500;a++){
			LCD5110_LCD_write_byte(liber_bmp3[a],1);
		}
		HAL_Delay(DISP_DELAY);

*/

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

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
  /* USER CODE END Error_Handler_Debug */
}

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
