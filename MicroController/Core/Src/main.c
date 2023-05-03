/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "motor.h"
#include "ultra_sonic.h"
#include "charBuffer.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern uint16_t frontDist;
extern uint16_t rightDist;
extern int16_t deltaRight;
extern uint8_t freshData;
enum state
{
  searching,
  HugWall,
  curveRight,
  curveLeft,
  turningRight,
  turningLeft,
  reverse,
  unJam
};

uint16_t targetDist;

charBuffer *UsartReadBuffer;
charBuffer *UsartWriteQueue;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

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
  UsartReadBuffer = NewCharBuffer();
  UsartWriteQueue = NewCharBuffer();
  AppendCharBuffer(UsartWriteQueue, "----------------------------------------------------------------------\r\nUsart Write Buffer Init\r\nAwait further Messages\r\n", 122);
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
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM15_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_IC_Start_IT(&htim15, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT(&htim17, TIM_CHANNEL_1);
  // HAL_TIM_Base_Start_IT(&htim15);

  ultra_init();

  HAL_TIM_Base_Start_IT(&htim6);
  maneuver_init();

  // MoveForward(25, 125);
  // motorIdle(30);
  // MoveBackward(50, 125);
  // motorIdle(30);
  // turnRight(75, 125);
  // motorIdle(30);
  // turnLeft(125, 125);
  // motorIdle(30);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  // uint16_t turn90 = 10;
  uint16_t power = 31;
  frontDist = 500;
  rightDist = 500;
  targetDist = 125;

  uint16_t motorTime = 5;
  uint16_t turnTime = 20;
  uint16_t curveTrim = 10;
  uint16_t RightClose = 50;
  uint16_t probJammed = 30;
  uint16_t RightFar = 100;
  uint16_t RightCorner = 300;
  uint16_t FrontFar = 100;
  uint16_t FrontClose = 50;

  uint16_t lastState = searching;
  uint16_t staleCount = 0;

  enum state currentState = searching;

  while (1)
  {
    // State Actions the action for the new state is executed First then new data is taken
    setTrim(0);
    switch (currentState)
    {
    case searching:
      MoveForward(power, motorTime);
      break;
    case HugWall:
      if (deltaRight > -10)
      {
        setTrim(curveTrim / 2);
      }
      else if (deltaRight < 10)
      {
        setTrim(-curveTrim / 2);
      }
      MoveForward(power, motorTime);
      break;
    case curveRight:
      setTrim(-curveTrim);
      MoveForward(power, motorTime);
      break;
    case curveLeft:
      setTrim(curveTrim);
      MoveForward(power, motorTime);
      break;
    case turningRight:
      turnRight(power, turnTime);
      MoveForward(power, turnTime * 3);
      turnRight(power, turnTime);
      break;
    case turningLeft:
      turnLeft(power, turnTime);
      break;
      // default statements
    case unJam:
      turnRight(power, turnTime);
      MoveBackward(power, turnTime);
      turnLeft(power, turnTime);
      // turnLeft(power,motorTime);
      break;
    case reverse:
      MoveBackward(power, turnTime);
      // turnLeft(power,motorTime);
      break;
    }

    // Spin main till the motor is done And we have new data
    while (motorActive() || freshData < 3)
    {
    }
    // Resets the memory indicating new data has been collected
    freshData = 0;


    if(currentState == lastState){
      staleCount++;
    }else{
      staleCount = 0;
    }

    
    lastState = currentState;


    switch (currentState)
    {
    case searching:
      if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist < FrontFar)
      {
        currentState = turningLeft;
      }
      else if (rightDist < RightFar)
      {
        currentState = HugWall;
      }
      break;
    case HugWall:
      if (rightDist < probJammed || frontDist < probJammed)
      {
        currentState = unJam;
      }
      else if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist < FrontFar)
      {
        currentState = turningLeft;
      }
      else if (rightDist > RightCorner)
      {
        currentState = turningRight;
      }
      else if (rightDist > RightFar)
      {
        currentState = curveRight;
      }
      else if (rightDist < RightClose)
      {
        currentState = curveLeft;
      }
      break;
    case curveRight:
      if (rightDist < probJammed || frontDist < probJammed)
      {
        currentState = unJam;
      }
      else if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist < FrontFar)
      {
        currentState = turningLeft;
      }
      else if (rightDist > RightClose)
      {
        currentState = HugWall;
      }
      break;
    case curveLeft:
      if (rightDist < probJammed || frontDist < probJammed)
      {
        currentState = unJam;
      }
      else if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist < FrontFar)
      {
        currentState = turningLeft;
      }
      else if (rightDist < RightFar)
      {
        currentState = HugWall;
      }
      break;
    case turningRight:
      if(staleCount > 10){
        currentState = searching;
      }
      else if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist < FrontFar)
      {
        currentState = turningLeft;
      }
      else if (rightDist < RightFar)
      {
        currentState = HugWall;
      }
      break;
    case turningLeft:
      if (rightDist < probJammed || frontDist < probJammed)
      {
        currentState = unJam;
      }
      else if (frontDist < FrontClose)
      {
        currentState = reverse;
      }
      else if (frontDist > FrontFar && rightDist < RightCorner)
      {
        currentState = searching;
      }
      break;
    case unJam:
      if (rightDist > probJammed && frontDist > probJammed)
      {
        currentState = searching;
      }
      break;
    case reverse:
      if (frontDist > FrontClose)
      {
        currentState = turningLeft;
      }
      break;
      // default statements
    }


  }
  // 	(while1){
  // 	while(1){
  //     if(motorActive()){
  //       continue;
  //     }
  // 		if(frontDist >= 125 && rightDist >= 125){
  // 			MoveForward(power, 5);
  // 		}
  // 		else
  // 			break;
  // 	}

  // 	//turnRight(power,50);

  //   while (1)
  //   {
  //     if(motorActive()){
  //       continue;
  //     }
  // 		// if(frontDist >= 125 && rightDist >= targetDist*2){
  //     //   break;
  // 		// }
  //     if(frontDist <= 125 && rightDist <= targetDist){
  //       setTrim(0);
  //       turnLeft(power,20);
  //       continue;
  // 		}
  //     if(frontDist <= 125){
  //       setTrim(0);
  //       MoveBackward(power,20);
  //       turnLeft(power, 50);
  //       continue;
  //     }
  //      if(rightDist > targetDist*4){
  //       setTrim(0);
  //       MoveForward(power, 30);
  //       turnRight(power, 15);
  //       MoveForward(power, 30);
  //       // turnRight(power, 15);

  //       // turnLeft(power,7);
  //       continue;
  //       // MoveForward(power,5);
  //     }

  //     if(deltaRight > 0){
  //       setTrim((targetDist - rightDist)*0.125);
  //     }else{
  //       setTrim((targetDist - rightDist)*0.25);
  //     }
  //     MoveForward(power,1);
  //     // if(rightDist > targetDist){
  //     // trimMotorsRight(1);
  //     //   MoveForward(power,2);
  //     // }else{
  //     //   trimMotorsLeft(1);
  //     //   MoveForward(power,2);
  //     // }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
  //   }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

#ifdef USE_FULL_ASSERT
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
