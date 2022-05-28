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
uint32_t button0_counter = 0; // How many frames the button has been held for
uint32_t gameStart;          // When (in ms since boot) the current game started
uint32_t lastFrameTick = 0;             // Counter to track when to update frame
uint32_t lastSecondTick = 0;   // Counter to track when to update time indicator
uint32_t nextTick = 0;                  // Counter for measuring time
TS_StateTypeDef TS_State;               // Touchscreen struct
gameStatus gameState;                   // State of game
uint16_t touchStateTransition = 0;    // Counter for touch state event detection
void (*checkTouch)(void);               // Function pointer for touch states
bool gameOn;                            // Whether a game is in play or not

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void winConditions(void);
static gameStatus handleTouchBegin(void);
static void handleTouchEnd(void);
static void clearIdle(void);
static void touchMaybe(void);
static void touchIdle(void);
static void clearMaybe(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Methods to run when win condition is met
 * TODO: Finish
 */
static void winConditions(void) {
  gameState = GAME_ENDED;
  drawTime(lastSecondTick - gameStart, true);
  // Other win events?
}

/*
 * Nonblocking(?) Touch detection with debounce based on WaitForPressedState() method
 */
// Run this when a touch has been confirmed to be occurring
static gameStatus handleTouchBegin(void) {
  int16_t x = TS_State.X;
  int16_t y = TS_State.Y;
  y = 320 - y;
  Serial_Message_NB("Touch X coordinate: ");
  Print_Int(TS_State.X);
  Serial_Message_NB("Touch Y coordinate: ");
  Print_Int(TS_State.Y);

  if (GAME_IN_PLAY == gameState) {
    // gameTouchHandler returns either GAME_WIN or GAME_IN_PLAY
    return gameTouchHandler(x, y);
  } else {
    // Must be in GAME_LEVEL_SELECT state to reach here
    // levelSelectTouchHandler returns either GAME_INIT or GAME_LEVEL_SELECT
    return levelSelectTouchHandler(x, y);
  }

}

// Run this when a touch has been confirmed to have ended
static void handleTouchEnd(void) {
//  int16_t x = TS_State.X;
//  int16_t y = TS_State.Y;
//  y = 320 - y;
}

// Run this function to determine how to parse a touch while in CLEAR_IDLE state (i.e. no touch is occurring)
// If a touch is detected, enter TOUCH_MAYBE state
static void clearIdle(void) {
  if (TS_State.TouchDetected) {
    checkTouch = &touchMaybe;
    touchStateTransition = 1;
  }
}

// Run this function to determine how to parse a touch while in TOUCH_MAYBE state (i.e. a touch might be occurring)
// If four consecutive frames register touches (60 ms), enter TOUCH_IDLE state
static void touchMaybe(void) {
  if (TS_State.TouchDetected) {
    touchStateTransition++;
    if (4 == touchStateTransition) {
      checkTouch = &touchIdle;
      touchStateTransition = 0;
      gameState = handleTouchBegin();
    }
  } else {
    checkTouch = &clearIdle;
    touchStateTransition = 0;
  }
}

// Run this function to determine how to parse a touch while in TOUCH_IDLE state (i.e. finger is currently touching screen)
// If no touch is detected, enter CLEAR_MAYBE state
static void touchIdle() {
  if (!TS_State.TouchDetected) {
    checkTouch = &clearMaybe;
    touchStateTransition = 1;
  }
}

// Run this function to determine how to parse a touch while in CLEAR_MAYBE state (i.e. a touch might have ended)
// If four consecutive frames register no touch (60 ms), enter CLEAR_IDLE state
static void clearMaybe() {
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

  gameState = SYSTEM_BOOT;

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

  // Do EEPROM read to get last-set level
  eepromGetLevel();

  // Begin Game
  lastFrameTick = HAL_GetTick();
  lastSecondTick = lastFrameTick;
  gameStart = lastSecondTick;
  gameState = GAME_INIT;
  prosetInit();
  checkTouch = &clearIdle;
  gameState = GAME_IN_PLAY;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    nextTick = HAL_GetTick();

    // Update clock if a game is ongoing
    if ((GAME_IN_PLAY == gameState) && ((nextTick - lastSecondTick) > 1000)) {
      // Update clock time
      lastSecondTick += 1000;
      drawTime(lastSecondTick - gameStart, false);
    }

    // Idle so screen a is drawn at (at most) 50 FPS
    //   (and game updates occur at most 50 FPS
    // When updating screen, check for new touch or console input
    if ((nextTick - lastFrameTick) > FRAME_DELAY) {
      lastFrameTick = nextTick;

      // If game is in a transition state (GAME_INIT, GAME_WIN, ENTER_LEVEL_SELECT), move to next state
      if (GAME_INIT == gameState) {
        Serial_Message("Reset game");
        lastFrameTick = HAL_GetTick();
        lastSecondTick = lastFrameTick;
        gameStart = lastSecondTick;
        prosetInit();
        gameState = GAME_IN_PLAY;
      } else if (GAME_WIN == gameState) {
        winConditions();
      } else if (GAME_ENTER_LEVEL_SELECT == gameState) {
        // level select screen
        levelSelectInit();
        gameState = GAME_LEVEL_SELECT;
      }

      // If game is in IN_PLAY state or LEVEL_SELECT state, check for screen touch
      // This may cause the game state to enter one of the transition states
      if ((GAME_IN_PLAY == gameState) || (GAME_LEVEL_SELECT == gameState)) {
        BSP_TS_GetState(&TS_State);
        checkTouch();
      }

      // If game is in IN_PLAY, LEVEL_SELECT, or GAME_ENDED state, check for keypress
      // This may cause the game state to enter one of the transition states
      if ((GAME_IN_PLAY == gameState) || (GAME_LEVEL_SELECT == gameState)
          || (GAME_ENDED == gameState)) {
        gameState = ConsoleProcess(gameState);
      }

      // If game is in IN_PLAY, LEVEL_SELECT, or GAME_ENDED state, check for button press
      // Any length press clears selection if in GAME_IN_PLAY state
      //   (This may cause the game state to enter one of the transition states)
      // Hold for 1 second (50 frame) to reset game (if in GAME_IN_PLAY or GAME_ENDED state)
      // Hold for 4 seconds (200 frames) to enter LEVEL_SELECT
      if ((GAME_IN_PLAY == gameState) || (GAME_LEVEL_SELECT == gameState)
          || (GAME_ENDED == gameState)) {
        if (GPIO_PIN_SET == BSP_PB_GetState(BUTTON_KEY)) {
          button0_counter++;
          if (button0_counter == 200) {
            gameState = GAME_ENTER_LEVEL_SELECT;
          } else if ((button0_counter == 50)
              && (GAME_LEVEL_SELECT != gameState)) {
            gameState = GAME_INIT;
          } else if ((button0_counter == 1) && (GAME_IN_PLAY == gameState)) {
            gameState = clearTable();
          }
        } else {
          button0_counter = 0;
        }
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
