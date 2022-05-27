/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "crc.h"
#include "dma2d.h"
#include "i2c.h"
#include "ltdc.h"
#include "rng.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include "serial_logging.h"
#include "stm32f429i_discovery.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "eeprom.h"
#include "settings.h"
#include "touchscreen.h"
#include "console.h"
#include "game.h"
#include "cards.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DEBOUNCE_TIME_MS 10      // Time to wait (in milliseconds) before checking for a new user button press
#define FRAME_DELAY      20      // Time to wait (in milliseconds) between updating frames
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t button0_debounce_time_old = 0; // Counter to track button debounce time
uint32_t gameStart;                     // When (in ms since boot) the current game started
uint32_t lastFrameTick = 0;             // Counter to track when to update frame
uint32_t lastSecondTick = 0;            // Counter to track when to update time indicator
uint32_t nextTick = 0;                  // Counter for measuring time
TS_StateTypeDef TS_State;               // Touchscreen struct
uint16_t touchStateTransition = 0;      // Counter for touch state event detection
void (*checkTouch)(void);               // Function pointer for touch states
bool gameOn;                            // Whether a game is in play or not

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// External-interrupt callback to toggle LD4 when user button is pressed
// Debounces by expecting a 10ms gap (or more) between valid presses
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  uint32_t button0_debounce_time_new = HAL_GetTick();
  if ((button0_debounce_time_new - button0_debounce_time_old)
      >= DEBOUNCE_TIME_MS) {
    HAL_GPIO_TogglePin(LD4_GPIO_Port, LD4_Pin);
    button0_debounce_time_old = button0_debounce_time_new;
  }
}

/*
 * Nonblocking(?) Touch detection with debounce based on WaitForPressedState() method
 */
// Run this when a touch has been confirmed to be occurring
void handleTouchBegin(void) {
  int16_t x = TS_State.X;
  int16_t y = TS_State.Y;
  y = 320 - y;
  Serial_Message("\nTouch X coordinate: ");
  Print_Int(TS_State.X);
  Serial_Message("\nTouch Y coordinate: ");
  Print_Int(TS_State.Y);

  // gameTouchHandler returns 1 if the game has completed and 0 otherwise
  if (gameTouchHandler(x, y)) {
    // TODO: win condition
    gameOn = false;
    drawTime(lastSecondTick - gameStart, true);
  }
}

// Run this when a touch has been confirmed to have ended
void handleTouchEnd(void) {
//  int16_t x = TS_State.X;
//  int16_t y = TS_State.Y;
//  y = 320 - y;
}

// Run this function to determine how to parse a touch while in CLEAR_IDLE state (i.e. no touch is occurring)
// If a touch is detected, enter TOUCH_MAYBE state
void clearIdle(void) {
  if (TS_State.TouchDetected) {
    checkTouch = &touchMaybe;
    touchStateTransition = 1;
  }
}

// Run this function to determine how to parse a touch while in TOUCH_MAYBE state (i.e. a touch might be occurring)
// If four consecutive frames register touches (60 ms), enter TOUCH_IDLE state
void touchMaybe(void) {
  if (TS_State.TouchDetected) {
    touchStateTransition++;
    if (4 == touchStateTransition) {
      checkTouch = &touchIdle;
      touchStateTransition = 0;
      handleTouchBegin();
    }
  } else {
    checkTouch = &clearIdle;
    touchStateTransition = 0;
  }
}

// Run this function to determine how to parse a touch while in TOUCH_IDLE state (i.e. finger is currently touching screen)
// If no touch is detected, enter CLEAR_MAYBE state
void touchIdle() {
  if (!TS_State.TouchDetected) {
    checkTouch = &clearMaybe;
    touchStateTransition = 1;
  }
}

// Run this function to determine how to parse a touch while in CLEAR_MAYBE state (i.e. a touch might have ended)
// If four consecutive frames register no touch (60 ms), enter CLEAR_IDLE state
void clearMaybe() {
  if (TS_State.TouchDetected) {
    checkTouch = &touchIdle;
    touchStateTransition = 0;
  } else {
    touchStateTransition++;
    if (4 == touchStateTransition) {
      checkTouch = &clearIdle;
      touchStateTransition = 0;
      handleTouchEnd();
    }
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
  MX_CRC_Init();
  MX_FMC_Init();
  MX_I2C3_Init();
  MX_SPI5_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_USB_OTG_HS_HCD_Init();
  MX_UART5_Init();
  MX_TIM7_Init();
  MX_DMA2D_Init();
  MX_LTDC_Init();
  MX_RNG_Init();
  /* USER CODE BEGIN 2 */

  Serial_Message("\r\n######################\r\n\nDevice has turned on.\r\n");

  // Enable LCD and touchscreen, but don't turn it on yet
  BSP_LCD_Init();

//  if (TS_Get_Params() != 0) {
//    Touchscreen_Calibration();
//    HAL_FLASH_Lock();
//  }

  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  // Begin Console
  ConsoleInit();

  // Begin Game
  lastFrameTick = HAL_GetTick();
  lastSecondTick = lastFrameTick;
  gameStart = lastSecondTick + 250; // Hacky way to add a grace period before clock starts
  prosetInit();
  checkTouch = &clearIdle;
  gameOn = true; //////////////////////////////// BAD

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (gameOn) {
      // Update clock if necessary
      // Idle so screen is drawn at (at most) 50 FPS
      // Check if a new touch has occurred
      nextTick = HAL_GetTick();
      if ((nextTick - lastSecondTick) > 1000) {
        // Update clock time
        lastSecondTick += 1000;
        drawTime(lastSecondTick - gameStart, false);
      }
      if ((nextTick - lastFrameTick) > FRAME_DELAY) {
        lastFrameTick = nextTick;
        BSP_TS_GetState(&TS_State);

        checkTouch();
        //ConsoleProcess(); // Should this be done here or elsewhere?
      }
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM6 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  Serial_Message("ERROR");
  //__disable_irq();
  while (1) {
    /* Toggle LED3 fast */
    BSP_LED_Toggle(LED3);
    HAL_Delay(40);
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
