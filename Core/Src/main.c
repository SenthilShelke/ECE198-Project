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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
#define TRIG_PIN GPIO_PIN_7
#define TRIG_PORT GPIOC
#define ECHO_PIN GPIO_PIN_8
#define ECHO_PORT GPIOA
uint32_t pMillis;
uint32_t Value1 = 0;
uint32_t Value2 = 0;
uint16_t Distance  = 0;  // cm
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//UART
uint8_t tx_buff= 1;

//LCD
LCD_HandleTypeDef lcd = {
   .RS_Port = GPIOA, .RS_Pin = GPIO_PIN_0,
   .EN_Port = GPIOA, .EN_Pin = GPIO_PIN_1,
   .D4_Port = GPIOA, .D4_Pin = GPIO_PIN_4,
   .D5_Port = GPIOA, .D5_Pin = GPIO_PIN_5,
   .D6_Port = GPIOA, .D6_Pin = GPIO_PIN_6,
   .D7_Port = GPIOA, .D7_Pin = GPIO_PIN_7,
};

//DHT11
#define DHT11_PORT GPIOB
#define DHT11_PIN GPIO_PIN_9
uint8_t RHI, RHD, TCI, TCD, SUM;
uint32_t pMillis, cMillis;
float tCelsius = 0;
float tFahrenheit = 0;
float RH = 0;
void microDelay (uint16_t delay)
{
 __HAL_TIM_SET_COUNTER(&htim1, 0);
 while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}
uint8_t DHT11_Start (void)
{
 uint8_t Response = 0;
 GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
 GPIO_InitStructPrivate.Pin = DHT11_PIN;
 GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as output
 HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 0);   // pull the pin low
 HAL_Delay(20);   // wait for 20ms
 HAL_GPIO_WritePin (DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
 microDelay (30);   // wait for 30us
 GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
 GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
 HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStructPrivate); // set the pin as input
 microDelay (40);
 if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))
 {
   microDelay (80);
   if ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN))) Response = 1;
 }
 pMillis = HAL_GetTick();
 cMillis = HAL_GetTick();
 while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
 {
   cMillis = HAL_GetTick();
 }
 return Response;
}
uint8_t DHT11_Read (void)
{
 uint8_t a,b;
 for (a=0;a<8;a++)
 {
   pMillis = HAL_GetTick();
   cMillis = HAL_GetTick();
   while (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
   {  // wait for the pin to go high
     cMillis = HAL_GetTick();
   }
   microDelay (40);   // wait for 40 us
   if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
     b&= ~(1<<(7-a));
   else
     b|= (1<<(7-a));
   pMillis = HAL_GetTick();
   cMillis = HAL_GetTick();
   while ((HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)) && pMillis + 2 > cMillis)
   {  // wait for the pin to go low
     cMillis = HAL_GetTick();
   }
 }
 return b;
}
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
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
 HAL_TIM_Base_Start(&htim1);
   HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);  // pull the TRIG pin low
   LCD_Init(&lcd);
  HAL_TIM_Base_Start(&htim1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 // Display messages
//  LCD_SetCursor(&lcd, 0, 0);       // Set cursor to row 0, column 0
//  LCD_WriteString(&lcd, "Hello, STM32!");
//
//  LCD_SetCursor(&lcd, 1, 0);       // Set cursor to row 1, column 0
//  LCD_WriteString(&lcd, "LCD 4-bit Mode");
	#define M_PI 3.14159265358979323846
  int tempdist1 = 0;
  int diameter = 7;
  int height = 20;
  int volume = M_PI *(diameter/2) *(diameter/2) * height;
  int currentVol;
  int volEmpt;
  while (1)
 {
    if (DHT11_Start())
	    {
	        RHI = DHT11_Read(); // Relative humidity integral
	        RHD = DHT11_Read(); // Relative humidity decimal
	        TCI = DHT11_Read(); // Celsius integral
	        TCD = DHT11_Read(); // Celsius decimal
	        SUM = DHT11_Read(); // Check sum
	        if (RHI + RHD + TCI + TCD == SUM) // Validate checksum
	        {

	            // Clear the LCD
	//	       	                  LCD_Clear(&lcd);
	          char tempStr[10];
			  LCD_SetCursor(&lcd, 0, 0);       // Set cursor to row 0, column 0
			  LCD_WriteString(&lcd, "Temp:");

			  LCD_SetCursor(&lcd, 0, 5);       // Set cursor to row 0, column 0
			  itoa(TCI, tempStr, 10);
			  LCD_WriteString(&lcd, tempStr);
			  LCD_SetCursor(&lcd, 0, 7);       // Set cursor to row 0, column 0
			  LCD_WriteString(&lcd, " C");
			  LCD_SetCursor(&lcd, 0, 9);       // Set cursor to row 0, column 0
//			  LCD_WriteString(&lcd, (char)223);


	        }
    }
	    // Add a small delay to prevent overwhelming the DHT11 sensor
    // Trigger the ultrasonic sensor
     HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET); // Trigger HIGH
     __HAL_TIM_SET_COUNTER(&htim1, 0);                     // Reset timer
     while (__HAL_TIM_GET_COUNTER(&htim1) < 10);           // Wait 10 µs
     HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET); // Trigger LOW

     // Measure echo pulse width
     pMillis = HAL_GetTick(); // Avoid infinite loop
     while (!(HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN)) && (HAL_GetTick() - pMillis < 10))
         ; // Wait for Echo HIGH
     Value1 = __HAL_TIM_GET_COUNTER(&htim1); // Record start time

     pMillis = HAL_GetTick(); // Avoid infinite loop
     while ((HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN)) && (HAL_GetTick() - pMillis < 50))
         ; // Wait for Echo LOW
     Value2 = __HAL_TIM_GET_COUNTER(&htim1); // Record end time

     // Calculate distance
     Distance = (Value2 - Value1) * 0.034 / 2;

     //fixes the bug with the value being 846, when nothing is near it
     if(Distance == 846){
    	 Distance = 0;
     }

    //fixes the bug with removing the excess letters showing up
    if(tempdist1 > Distance){
    	LCD_Clear(&lcd);
    }




    // Display distance on the LCD
     char tempStr1[10];
     LCD_SetCursor(&lcd, 1, 0); // Set cursor to row 1, column 0
     itoa(Distance, tempStr1, 10); // Convert Distance to string
     LCD_WriteString(&lcd, tempStr1); // Display Distance
     LCD_SetCursor(&lcd, 1, 3); // Set cursor to row 1, column 0
     LCD_WriteString(&lcd, " cm");   // Append units


    //displaying the valume of water in the tank
//    volEmpt = (Distance) * M_PI *(diameter/2) *(diameter/2);
//    currentVol = volume - volEmpt;
//    char tempStr1[10];
//     LCD_SetCursor(&lcd, 1, 0); // Set cursor to row 1, column 0
//     itoa(currentVol, tempStr1, 10); // Convert Distance to string
//     LCD_WriteString(&lcd, tempStr1); // Display Distance
//     LCD_SetCursor(&lcd, 1, 6); // Set cursor to row 1, column 0
//     LCD_WriteString(&lcd, " mL");   // Append units


     //turning the leds on and off
     if(Distance > 16 ){
    	 // red led on
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 0);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 1);
		 tx_buff = 0;
     }
     else{
    	 //green led on
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_10, 1);
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, 0);
		  tx_buff = 1;

     }

     HAL_UART_Transmit(&huart1, &tx_buff, 1,1000);




     HAL_Delay(500); // Add a delay to avoid rapid triggering
     tempdist1 = Distance;
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */
  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM1_Init 1 */
  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 72-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim1, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */
  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */
  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|LD2_Pin
                          |GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA4 LD2_Pin
                           PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|LD2_Pin
                          |GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 PB10 PB4 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_4|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
