/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OSC_IN_Pin GPIO_PIN_14
#define OSC_IN_GPIO_Port GPIOC
#define OSC_OUT_Pin GPIO_PIN_15
#define OSC_OUT_GPIO_Port GPIOC
#define GYRO_SCK_Pin GPIO_PIN_7
#define GYRO_SCK_GPIO_Port GPIOF
#define GYRO_MISO_Pin GPIO_PIN_8
#define GYRO_MISO_GPIO_Port GPIOF
#define GYRO_MOSI_Pin GPIO_PIN_9
#define GYRO_MOSI_GPIO_Port GPIOF
#define GYRO_CS_Pin GPIO_PIN_1
#define GYRO_CS_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOC
#define GYRO_INT1_Pin GPIO_PIN_1
#define GYRO_INT1_GPIO_Port GPIOA
#define GYRO_INT2_Pin GPIO_PIN_2
#define GYRO_INT2_GPIO_Port GPIOA
#define GYRO_INT2_EXTI_IRQn EXTI2_IRQn
#define LCD_WR_Pin GPIO_PIN_13
#define LCD_WR_GPIO_Port GPIOD
#define TP_SDA_Pin GPIO_PIN_9
#define TP_SDA_GPIO_Port GPIOC
#define TP_SCL_Pin GPIO_PIN_8
#define TP_SCL_GPIO_Port GPIOA
#define TP_INT_Pin GPIO_PIN_15
#define TP_INT_GPIO_Port GPIOA
#define TP_INT_EXTI_IRQn EXTI15_10_IRQn
#define Dioda1_Pin GPIO_PIN_13
#define Dioda1_GPIO_Port GPIOG
#define Dioda2_Pin GPIO_PIN_14
#define Dioda2_GPIO_Port GPIOG

/* USER CODE BEGIN Private defines */
#define LCD_WIDTH 240
#define LCD_HEIGHT 320

#define FRAMEBUFF_1 ((uint32_t)0xD0000000)
#define FRAMEBUFF_1_END ((uint32_t)FRAMEBUFF_1+LCD_HEIGHT*LCD_WIDTH*4)
#define FRAMEBUFF_2 ((uint32_t)FRAMEBUFF_1_END+4)
#define FRAMEBUFF_2_END ((uint32_t)FRAMEBUFF_2+LCD_HEIGHT*LCD_WIDTH*4)
#define SAVE_BUFFER ((uint32_t)FRAMEBUFF_2_END+4)

extern uint32_t CURRENT_BUFFER;
extern uint32_t CURRENT_BUFFER_END;
extern uint32_t CURRENT_BACK_BUFFER;
extern uint32_t CURRENT_BACK_BUFFER_END;

#define TS_Xmin (int)200
#define TS_Ymin (int)130
#define TS_Xmax (int)3900
#define TS_Ymax (int)3750

#define BUTTON_DEBOUNCE 100

#define SHIELD_WIDTH 80
#define SHIELD_HEIGHT 10

typedef enum {
    MENU,
    OPTIONS,
    GAME,
	END
} MenuState;

extern bool textCenter;
extern volatile int TouchDetected;
extern volatile int RotDetected;
extern MenuState State;
extern int points;
extern int record;
extern bool isWon;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
