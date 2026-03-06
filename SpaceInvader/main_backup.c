/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "fonts.h"
#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FRAMEBUFF_1 ((uint32_t)0xD0000000)
#define FRAMEBUFF_1_END ((uint32_t)FRAMEBUFF_1+LCD_HEIGHT*LCD_WIDTH*4)
#define FRAMEBUFF_2 ((uint32_t)FRAMEBUFF_1_END+4)
#define FRAMEBUFF_2_END ((uint32_t)FRAMEBUFF_2+LCD_HEIGHT*LCD_WIDTH*4)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DMA2D_HandleTypeDef hdma2d;

I2C_HandleTypeDef hi2c3;

LTDC_HandleTypeDef hltdc;

SPI_HandleTypeDef hspi5;

UART_HandleTypeDef huart1;

SDRAM_HandleTypeDef hsdram1;

/* USER CODE BEGIN PV */
uint32_t CURRENT_BUFFER=FRAMEBUFF_1;
uint32_t CURRENT_BUFFER_END=FRAMEBUFF_1_END;
uint32_t CURRENT_BACK_BUFFER=FRAMEBUFF_2;
uint32_t CURRENT_BACK_BUFFER_END=FRAMEBUFF_2_END;

typedef enum {
    DRAW,
    HANDLE_INPUT // Rozkaz: Sprawdź czy coś kliknięto
} ScreenCmd;

typedef enum {
    MENU,
    OPTIONS,
    GAME,
	END
} MenuState;

MenuState State=MENU;
volatile int TouchDetected=0;
volatile int RotDetected=0;
float dps_x=0.f;
float dps_offset=0.f;
float angle=0.f;
uint32_t last_tick=0;
float touchX;
float touchY;

bool isHard=false;
bool isTouch=false;

bool isCenter=false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI5_Init(void);
static void MX_LTDC_Init(void);
static void MX_FMC_Init(void);
static void MX_DMA2D_Init(void);
static void MX_I2C3_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
static void LCD_Write_Cmd(uint8_t);
static void LCD_Write_Data(uint8_t);
static void initLCD(void);
static void initSDRAM(SDRAM_HandleTypeDef *);
void testSDRAM(void);
void clearFramebuffer(uint32_t);

static void Gyro_Write_Reg(uint8_t, uint8_t);
static void initGyro(void);
void readGyroData(void);
void calibrateGyro(void);

static void TouchPanel_Write_Reg(uint8_t, uint8_t);
static void initTouchPanel(void);
static void enableTouchIT(void);
static void disableTouchIT(void);
static void clearTouchIT(void);
void readTouchData(void);

void transformPosOrientation(int*, int*);
void drawChar(int, int, char, uint32_t);
void drawString(int, int, int, char*, uint32_t);
void setBackgroundColor(uint32_t);
void drawTable(int, int, const uint32_t*, int, int);

int _write(int, char *, int);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == TP_INT_Pin)
	{
		TouchDetected=1;
	}
	if (GPIO_Pin == GYRO_INT2_Pin)
	{
		RotDetected=1;
	}
}

void drawRect(int posX, int posY, uint32_t color, int sizeX, int sizeY)
{
	transformPosOrientation(&posX, &posY);
	uint32_t destination_address = CURRENT_BUFFER + (posX + posY * LCD_WIDTH) * 4;
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.OutputOffset = LCD_WIDTH-sizeY;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_Start(&hdma2d, color, destination_address, sizeY, sizeX);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void drawPlayer(int posX, int posY)
{
	drawTable(posX, posY, PLAYER_TEXTURE, PLAYER_WIDTH, PLAYER_HEIGHT);
}

void drawEnemy(int posX, int posY)
{
	drawTable(posX, posY, ENEMY_TEXTURE, ENEMY_WIDTH, ENEMY_HEIGHT);
}

void drawShield(int posX, int posY)
{
	drawRect(posX, posY, 0xFFFF0000, 80, 10);
}

void drawBullet(int posX, int posY)
{
	drawTable(posX, posY, BULLET_TEXTURE, BULLET_WIDTH, BULLET_HEIGHT);
}

void drawMenuButtons(Button* play, Button* option)
{
	if(!play->isPressed) drawTable(play->x,play->y,BUTTON,BUTTON_WIDTH,BUTTON_HEIGHT);
	else drawTable(play->x,play->y,BUTTON_PRESS,BUTTON_WIDTH,BUTTON_HEIGHT);
	if(!option->isPressed) drawTable(option->x,option->y,BUTTON,BUTTON_WIDTH,BUTTON_HEIGHT);
	else drawTable(option->x,option->y,BUTTON_PRESS,BUTTON_WIDTH,BUTTON_HEIGHT);
	drawString(28,114,0,"GRAJ",0x00000000);
	drawString(221,114,-1,"OPCJE",0x00000000);
}

void drawButton(Button* btn)
{
    const uint32_t* texture = btn->isPressed ? BUTTON_PRESS : BUTTON;
    const char* label = btn->isPressed ? btn->pressed_label : btn->label;
    isCenter = true;
    drawTable(btn->x, btn->y, texture, BUTTON_WIDTH, BUTTON_HEIGHT);
    drawString(btn->x-BUTTON_HEIGHT/2+1, btn->y+BUTTON_WIDTH/2+2, (const)btn->label_offset, label, 0xFFFFFFFF);
    isCenter = false;
}

bool isButtonTouched(Button* btn)
{
    if(touchX < btn->x && touchX > btn->x-BUTTON_HEIGHT && touchY > btn->y && touchY < btn->y+BUTTON_WIDTH)
    {
        return true;
    }
    return false;
}

void drawOptionButtons()
{
	if(!isHard)
	{
		drawTable(200,60,BUTTON,BUTTON_WIDTH,BUTTON_HEIGHT);
		drawString(128,73,0,"EASY",0xFFFFFFFF);
	}
	else
	{
		drawTable(200,60,BUTTON_PRESS,BUTTON_WIDTH,BUTTON_HEIGHT);
		drawString(128,73,0,"HARD",0xFFFFFFFF);
	}
	if(!isTouch)
	{
		drawTable(200,150,BUTTON,BUTTON_WIDTH,BUTTON_HEIGHT);
		drawString(122,163,-2,"OBROT",0xFFFFFFFF);
	}
	else
	{
		drawTable(200,150,BUTTON_PRESS,BUTTON_WIDTH,BUTTON_HEIGHT);
		drawString(122,163,-2,"DOTYK",0xFFFFFFFF);
	}
}

void drawMainMenu()
{
	setBackgroundColor(0xFF000000);
	drawBullet(60,70);
	drawBullet(160+BULLET_WIDTH/2-1,LCD_WIDTH-1-PLAYER_HEIGHT-BULLET_HEIGHT-7);
	drawBullet(250,155);
	drawString(55, 60, 0,"Space Invader", 0xFFFFFFFF);
	drawPlayer(160+PLAYER_WIDTH/2,LCD_WIDTH-1-PLAYER_HEIGHT);
	for(int i=0; i<8;++i)
	{
		drawEnemy(315-i*40,5);
	}
	drawShield(100, 180);
	drawShield(300, 180);
}

void drawOptionMenu()
{
	setBackgroundColor(0xFF000000);
	drawBullet(25+BULLET_HEIGHT/2,80);
	drawBullet(25+BULLET_HEIGHT/2,150);
	drawEnemy(40,10);
	drawEnemy(310,68);
	drawEnemy(310, 158);
	drawPlayer(40,200);
	drawString(90,30,0,"Trudnosc:",0xFFFFFFFF);
	drawString(70,120,0,"Sterowanie:",0xFFFFFFFF);
	drawRect(319, 0, 0xFFFF0000, 40, 40);
	drawChar(293,10,'X',0xFFFFFFFF);
}

void swapBuffer()
{
    HAL_LTDC_SetAddress(&hltdc, CURRENT_BUFFER, 0);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);

	uint32_t temp = CURRENT_BUFFER;
	CURRENT_BUFFER = CURRENT_BACK_BUFFER;
	CURRENT_BACK_BUFFER = temp;

	temp = CURRENT_BUFFER_END;
	CURRENT_BUFFER_END = CURRENT_BACK_BUFFER_END;
	CURRENT_BACK_BUFFER_END = temp;
}

void manageMainMenuButtons(ScreenCmd cmd)
{
	Button play = { .x = 100, .y = 100, .label = "GRAJ", .pressed_label = "GRAJ", .text_color = 0x00000000, .isPressed = false };
	Button option = { .x = 300, .y = 100, .label = "OPCJE", .pressed_label = "OPCJE", .text_color = 0x00000000, .label_offset=-1, .isPressed = false };

	if(cmd==DRAW)
	{
		drawButton(&play);
		drawButton(&option);
	}
	else if (cmd==HANDLE_INPUT)
	{
		if(isButtonTouched(&play))
		{
			play.isPressed=true;
			drawButton(&play);
	        HAL_Delay(200);
	        State=GAME;
		}
	    else if(isButtonTouched(&option))
	    {
	    	option.isPressed=true;
	    	drawButton(&option);
	    	HAL_Delay(200);
	    	State=OPTIONS;
	    	drawOptionMenu();
	    	manageOptionMenuButtons(DRAW);
	    }
	}
}

void manageOptionMenuButtons(ScreenCmd cmd)
{
	Button difficulty = {.x = 200, .y = 60, .label = "EASY", .pressed_label = "HARD", .text_color=0xFFFFFFFF };
	Button control = { .x = 200, .y = 150, .label = "OBROT", .pressed_label = "DOTYK", .text_color=0xFFFFFFFF, .label_offset=-2 };
	difficulty.isPressed=isHard;
	control.isPressed=isTouch;

	if(cmd==DRAW)
	{
		drawOptionMenu();
		drawButton(&difficulty);
		drawButton(&control);
	}
	else if (cmd==HANDLE_INPUT)
	{
		if(isButtonTouched(&difficulty))
		{
			isHard=!isHard;
	        difficulty.isPressed=isHard;
	        drawButton(&difficulty);
	        HAL_Delay(100);
		}
		if(isButtonTouched(&control))
		{
			isTouch=!isTouch;
	        control.isPressed=isTouch;
	        drawButton(&control);
	        HAL_Delay(100);
	    }
		if(touchX<320 && touchX>270 && touchY>0 && touchY<50)
		{
			State=MENU;
			drawMainMenu();
			manageMainMenuButtons(DRAW);
		}
	}
}

void operateMenu()
{
	Button play, option;
	play.x=100; play.y=100;
	option.x=300; option.y=100;
	drawMainMenu();
	drawMenuButtons(&play, &option);
	//manageMainMenuButtons(DRAW);
	enableTouchIT();
	while(State!=GAME)
	{
		if(TouchDetected)
		{
			TouchDetected=0;
			readTouchData();
/*			if(State==MENU)
			{
				manageMainMenuButtons(HANDLE_INPUT);
			}
			if(State==OPTIONS)
			{
				manageOptionMenuButtons(HANDLE_INPUT);
			}*/
			if(touchX<play.x && touchX>play.x-BUTTON_HEIGHT && touchY>play.y & touchY<play.y+BUTTON_WIDTH && State==MENU)
			{
				play.isPressed=true;
				drawMenuButtons(&play, &option);
				HAL_Delay(200);
				State=GAME;
			}
			if(touchX<option.x && touchX>option.x-BUTTON_HEIGHT && touchY>option.y & touchY<option.y+BUTTON_WIDTH && State==MENU)
			{
				option.isPressed=true;
				drawMenuButtons(&play, &option);
				HAL_Delay(200);
				State=OPTIONS;
				drawOptionMenu();
				drawOptionButtons();
				option.isPressed=false;
			}
			if(touchX<320 && touchX>270 && touchY>0 && touchY<50 && State==OPTIONS)
			{
				State=MENU;
				drawMainMenu();
				drawMenuButtons(&play, &option);
			}
			if(touchX<200 && touchX>120 && touchY>60 && touchY<110 && State==OPTIONS)
			{
				isHard=!isHard;
				drawOptionButtons();
				HAL_Delay(100);
			}
			if(touchX<200 && touchX>120 && touchY>150 && touchY<200 && State==OPTIONS)
			{
				isTouch=!isTouch;
				drawOptionButtons();
				HAL_Delay(100);
			}
			touchX=0;
			touchY=0;
			clearTouchIT();
		}
	}
	disableTouchIT();
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
  MX_SPI5_Init();
  MX_FMC_Init();
  MX_DMA2D_Init();
  MX_LTDC_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  initGyro();
  initTouchPanel();
  //testSDRAM();
  calibrateGyro();
  last_tick=HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while(1)
  {
	  operateMenu();
	  return 0;
	  if(TouchDetected)
	  {
		  TouchDetected=0;
		  //printf("INTERRUPT: Dotknieto ekran!\r\n");
		  setBackgroundColor(0xFFFF0000);
		  readTouchData();
		  drawString((int)touchX, (int)touchY, 0, "Touch", 0xFFFFFFFF);
		  clearTouchIT();
	  }
	  if(RotDetected)
	  {
	      RotDetected=0;
	      uint32_t current_tick = HAL_GetTick();
	      float dt=(current_tick-last_tick)/1000.0f;
	      last_tick=current_tick;
		  readGyroData();
		  dps_x-=dps_offset;
		  angle+=dps_x*dt;
		  angle*=0.99f;
		  if(angle>90.f) angle=90.f;
		  if(angle<-90.f) angle=-90.f;
	      //printf("Interrupt: x:%f, angle:%f\r\n",dps_x,angle);
	  }
	  //readGyroData(); // Pobiera 'dps' (surowe)
      //printf("x:%f\r\n",dps);

	//printf("Test\r\n");

//	  readGyroData();
//	      if(dps>=30)
//	      {
//	          HAL_GPIO_WritePin(Dioda1_GPIO_Port, Dioda1_Pin, GPIO_PIN_SET);
//	      }
//	      else
//	      {
//	          HAL_GPIO_WritePin(Dioda1_GPIO_Port, Dioda1_Pin, GPIO_PIN_RESET);
//	      }
//	      if(dps<=-30)
//	      {
//	          HAL_GPIO_WritePin(Dioda2_GPIO_Port, Dioda2_Pin, GPIO_PIN_SET);
//	      }
//	      else
//	      {
//	          HAL_GPIO_WritePin(Dioda2_GPIO_Port, Dioda2_Pin, GPIO_PIN_RESET);
//	      }

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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DMA2D Initialization Function
  * @param None
  * @retval None
  */
static void MX_DMA2D_Init(void)
{

  /* USER CODE BEGIN DMA2D_Init 0 */

  /* USER CODE END DMA2D_Init 0 */

  /* USER CODE BEGIN DMA2D_Init 1 */

  /* USER CODE END DMA2D_Init 1 */
  hdma2d.Instance = DMA2D;
  hdma2d.Init.Mode = DMA2D_R2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */
  // Konfiguracja warstwy tła
  hdma2d.LayerCfg[0].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  hdma2d.LayerCfg[0].InputColorMode = DMA2D_INPUT_ARGB8888;
  HAL_DMA2D_ConfigLayer(&hdma2d, 0);
  // Konfiguracja warstwy ktora będzie nadpisywana z blendowaniem
  //hdma2d.LayerCfg[1].AlphaMode = DMA2D_NO_MODIF_ALPHA;
  //hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
  //hdma2d.LayerCfg[1].InputOffset = 0;
  //HAL_DMA2D_ConfigLayer(&hdma2d, 1);
  setBackgroundColor(0xFF000000);
  /* USER CODE END DMA2D_Init 2 */

}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 400000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief LTDC Initialization Function
  * @param None
  * @retval None
  */
static void MX_LTDC_Init(void)
{

  /* USER CODE BEGIN LTDC_Init 0 */

  // Konfiguracja zegara LCD. Wartosc zalecana - 6 MHz
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /* F poczatkowa = 1 MHz */
  /* PLLSAIN step I = 1 Mhz * 192 = 192 Mhz */
  /* PLLSAIR step II = 192/4 = 48 Mhz */
  /* LTDC f koncowa = 48 / LTDC_PLLSAI_DIVR_8 = 48/8 = 6Mhz */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 4;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
	  Error_Handler();
  }

  /* USER CODE END LTDC_Init 0 */

  LTDC_LayerCfgTypeDef pLayerCfg = {0};

  /* USER CODE BEGIN LTDC_Init 1 */

  /* USER CODE END LTDC_Init 1 */
  hltdc.Instance = LTDC;
  hltdc.Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc.Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc.Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc.Init.PCPolarity = LTDC_PCPOLARITY_IPC;
  hltdc.Init.HorizontalSync = 9;
  hltdc.Init.VerticalSync = 1;
  hltdc.Init.AccumulatedHBP = 29;
  hltdc.Init.AccumulatedVBP = 3;
  hltdc.Init.AccumulatedActiveW = 269;
  hltdc.Init.AccumulatedActiveH = 323;
  hltdc.Init.TotalWidth = 279;
  hltdc.Init.TotalHeigh = 327;
  hltdc.Init.Backcolor.Blue = 0;
  hltdc.Init.Backcolor.Green = 0;
  hltdc.Init.Backcolor.Red = 0;
  if (HAL_LTDC_Init(&hltdc) != HAL_OK)
  {
    Error_Handler();
  }
  pLayerCfg.WindowX0 = 0;
  pLayerCfg.WindowX1 = 240;
  pLayerCfg.WindowY0 = 0;
  pLayerCfg.WindowY1 = 320;
  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = 0xD0000000;
  pLayerCfg.ImageWidth = 240;
  pLayerCfg.ImageHeight = 320;
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LTDC_Init 2 */
  initLCD();
  /* USER CODE END LTDC_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

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

/* FMC initialization function */
static void MX_FMC_Init(void)
{

  /* USER CODE BEGIN FMC_Init 0 */

  /* USER CODE END FMC_Init 0 */

  FMC_SDRAM_TimingTypeDef SdramTiming = {0};

  /* USER CODE BEGIN FMC_Init 1 */

  /* USER CODE END FMC_Init 1 */

  /** Perform the SDRAM1 memory initialization sequence
  */
  hsdram1.Instance = FMC_SDRAM_DEVICE;
  /* hsdram1.Init */
  hsdram1.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_8;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_12;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_16;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_1;
  /* SdramTiming */
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 3;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler( );
  }

  /* USER CODE BEGIN FMC_Init 2 */
  initSDRAM(&hsdram1);
  /* USER CODE END FMC_Init 2 */
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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GYRO_CS_Pin|LCD_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, Dioda1_Pin|Dioda2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GYRO_CS_Pin LCD_CS_Pin */
  GPIO_InitStruct.Pin = GYRO_CS_Pin|LCD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : GYRO_INT1_Pin */
  GPIO_InitStruct.Pin = GYRO_INT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GYRO_INT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GYRO_INT2_Pin */
  GPIO_InitStruct.Pin = GYRO_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GYRO_INT2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_WR_Pin */
  GPIO_InitStruct.Pin = LCD_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_WR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_INT_Pin */
  GPIO_InitStruct.Pin = TP_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TP_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Dioda1_Pin Dioda2_Pin */
  GPIO_InitStruct.Pin = Dioda1_Pin|Dioda2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void drawTable(int posX, int posY, const uint32_t *table_adr, int tabWidth, int tabHeight)
{
	transformPosOrientation(&posX, &posY);
	uint32_t destination_address = CURRENT_BUFFER + (posX + posY * LCD_WIDTH) * 4;
	hdma2d.Init.Mode = DMA2D_M2M_BLEND;
	hdma2d.Init.OutputOffset = LCD_WIDTH - tabWidth;
	HAL_DMA2D_Init(&hdma2d);
	hdma2d.LayerCfg[0].InputOffset = LCD_WIDTH - tabWidth;
	HAL_DMA2D_ConfigLayer(&hdma2d, 0);
	HAL_DMA2D_BlendingStart(&hdma2d, (uint32_t)table_adr, destination_address, destination_address, tabWidth, tabHeight);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void setBackgroundColor(uint32_t color)
{
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.OutputOffset = 0;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_Start(&hdma2d, color, CURRENT_BUFFER, LCD_WIDTH, LCD_HEIGHT);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void drawString(int posX, int posY, int offset, char* string, uint32_t color)
{
	if(posX>=LCD_HEIGHT || posY>=LCD_WIDTH || posX<0 || posY<0) return;
	int textSize=0;
	char *ptr=string;
	while(*ptr++)
	{
		textSize++;
	}
	//if(offset<0) offset=0;
	if(isCenter) posX-=(float)(textSize/2.f)*(Font24.Width+offset);
	if(isCenter) posY-=Font24.Height/2;
	for(int i=0; i<textSize; i++)
	{
		drawChar(posX+(Font24.Width+offset)*i, posY, string[i], color);
	}
}

void drawChar(int posX, int posY, char c, uint32_t color)
{
	if(posX>=LCD_HEIGHT || posY>=LCD_WIDTH || posX<0 || posY<0) return;
	transformPosOrientation(&posX, &posY);
	int bytes_count=(Font24.Width+7)/8; // Ilość bajtów zawierających jeden wiersz tabeli
	int index=(c-' ')*Font24.Height*bytes_count; // Indeks znaku w tablicy cziocnki
	uint32_t *fb_start = (uint32_t *)CURRENT_BUFFER;
	uint32_t *fb_end = (uint32_t *)CURRENT_BUFFER_END;
	volatile uint32_t *fb = (uint32_t *)CURRENT_BUFFER+posX+LCD_WIDTH*posY;

	for(int x=0; x<Font24.Width; x++)
	{
		int current_bit=x%8;
		int tmp_index=index;
		for(int y=0; y<Font24.Height; y++)
		{
			int bit_value = (0x80 >> current_bit) & Font24.table[tmp_index];
			if(bit_value)
			{
				if(fb_start>fb+y || fb+y>=fb_end) continue;
				fb[y]=color;
			}
			tmp_index+=bytes_count;
		}
		if(x%8==7) index++;
		fb-=LCD_WIDTH;
	}
}

void transformPosOrientation(int* posX, int* posY)
{
	int temp=*posY;
	*posY=LCD_HEIGHT-*posX;
	*posX=temp;
}

static void Gyro_Write_Reg(uint8_t reg, uint8_t value)
{
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &reg, 1, 100);
	HAL_SPI_Transmit(&hspi5, &value, 1, 100);
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
}

static void initGyro()
{
	Gyro_Write_Reg(0x24, 0x80); // Reset
	HAL_Delay(10);
	Gyro_Write_Reg(0x20, 0x8F);  // Ctrl_reg1: Power ON, XYZ włączone, LPF Cutoff 12.5 hz, BDW 100 Hz
	Gyro_Write_Reg(0x21, 0x09);  // Ctrl_reg2: Ustawienia: Filtr HPF 0.5 hz
	Gyro_Write_Reg(0x22, 0x08);  // Ctrl_reg3: Wlaczenie przerwania
	Gyro_Write_Reg(0x23, 0x30);  // Ctrl_reg4: Ustawienie zakresu pomiarowego
	Gyro_Write_Reg(0x24, 0x10);  // Ctrl_reg5: Wlaczenie HPF
	HAL_Delay(50);
	readGyroData();
}

void readGyroData()
{
    uint8_t reg_adresses[3]={0xE8,0,0}; // Adresy odczytów czujnika w OSI X.0xE8=0x80(READ)+0x40(INCREMENT)+0x28(ADRESS)
    uint8_t reg_values[3]={0,0,0};
    int16_t mesurement;
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi5, reg_adresses, reg_values, 3, 100);
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
    mesurement=(int16_t)((reg_values[2] << 8) | reg_values[1]);
    dps_x=mesurement*0.07f;
}

void calibrateGyro()
{
	int samples=100;
	float sum=0.00f;
	for(int i=0; i<samples; ++i)
	{
		HAL_Delay(10);
		readGyroData();
		sum+=dps_x;
	}
	dps_offset=(float)sum/samples;
}

static void TouchPanel_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t dev_write_adr=0x82;
	HAL_I2C_Mem_Write(&hi2c3, dev_write_adr, reg, 1, &value, 1, HAL_MAX_DELAY);
}

static void initTouchPanel()
{
	TouchPanel_Write_Reg(0x03, 0x02); // Reset
	HAL_Delay(10);
	TouchPanel_Write_Reg(0x03, 0x00); // Wyjscie z resetu
	TouchPanel_Write_Reg(0x04, 0x0C); // Wlaczenie zegarow dla TS i Temp
	TouchPanel_Write_Reg(0x09, 0x01); // Wlaczenie przerwan
	TouchPanel_Write_Reg(0x0A, 0x02); // Wlaczenie przerwania od danych dotyku
	TouchPanel_Write_Reg(0x40, 0x03); // Wlaczenie sensora dotyku
	TouchPanel_Write_Reg(0x41, 0xA3); // Ustawienia specyfikacji odczytu
	TouchPanel_Write_Reg(0x4A, 0x01); // Ilesc dotykow w buforze przy przerwaniu - 1
	TouchPanel_Write_Reg(0x4B, 0x01); // Reset FIFO
	HAL_Delay(5);
	TouchPanel_Write_Reg(0x4B, 0x00); // Wlaczenie FIFO
	TouchPanel_Write_Reg(0x58, 0x01); // Ustawienia prądu
	TouchPanel_Write_Reg(0x0B, 0xFF); // Wyczyszczenie przerwań
}

static void enableTouchIT()
{
	TouchPanel_Write_Reg(0x4B, 0x01); // Reset FIFO
	HAL_Delay(5);
	TouchPanel_Write_Reg(0x4B, 0x00); // Wlaczenie FIFO
	TouchPanel_Write_Reg(0x0B, 0xFF); // Wyczyszczenie przerwań
	TouchPanel_Write_Reg(0x09, 0x01); // Wlaczenie przerwan
}

static void disableTouchIT(void)
{
	TouchPanel_Write_Reg(0x09, 0x00); // Wylaczenie przerwan
}

static void clearTouchIT(void)
{
	TouchPanel_Write_Reg(0x4B, 0x01); // Reset FIFO
	//HAL_Delay(5);
	TouchPanel_Write_Reg(0x4B, 0x00); // Wlaczenie FIFO
	TouchPanel_Write_Reg(0x0B, 0xFF); // Wyczyszczenie przerwań
}

void readTouchData(void)
{
	uint8_t data[4]={0};
	HAL_I2C_Mem_Read(&hi2c3, 0x82, 0x4D, 1, data, 4, HAL_MAX_DELAY);
	uint16_t mesurementX=(data[0] << 8) | data[1];
	uint16_t mesurementY=(data[2] << 8) | data[3];
	if(mesurementX<TS_Xmin) mesurementX=TS_Xmin;
	if(mesurementX>TS_Xmax) mesurementX=TS_Xmax;
	if(mesurementY<TS_Ymin) mesurementY=TS_Ymin;
	if(mesurementY>TS_Ymax) mesurementY=TS_Ymax;
	touchX=((float)(mesurementY-TS_Ymin)/(TS_Ymax-TS_Ymin))*LCD_HEIGHT;
	touchY=LCD_WIDTH-((float)(mesurementX-TS_Xmin)/(TS_Xmax-TS_Xmin))*LCD_WIDTH;
}

void testSDRAM()
{
	volatile uint32_t *fb = (uint32_t *)CURRENT_BUFFER;
	fb[1] = 0xCAFEBABE;

	if (fb[1] == 0xCAFEBABE) HAL_GPIO_WritePin(GPIOG, Dioda1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOG, Dioda2_Pin, GPIO_PIN_SET);
}

void clearFramebuffer(uint32_t color)
{
    volatile uint32_t *fb = (uint32_t *)CURRENT_BUFFER;
    for (int i = 0; i < 240*320; ++i)
    {
        fb[i] = color;
    }
}

int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	return len;
}

static void LCD_Write_Cmd(uint8_t cmd)
{
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, (uint8_t *) &cmd, 1, 100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void LCD_Write_Data(uint8_t data)
{
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, (uint8_t *) &data, 1, 100);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

/* Prywatna funkcja inicjalizująca pamiec SDRAM
 * Wartosc domyslne dla pamieci na plytce STM32F429I
 * zasugerowano się funkcją z biblioteki BSP */
static void initSDRAM(SDRAM_HandleTypeDef *hsdram)
{
	volatile uint32_t tmpmrd = 0;
	FMC_SDRAM_CommandTypeDef Command;

	// Włączenie zegara CKE
	Command.CommandMode             = FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 1;
	Command.ModeRegisterDefinition  = 0;
	HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

	HAL_Delay(1);

	// Komenda precharge all
	Command.CommandMode             = FMC_SDRAM_CMD_PALL;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 1;
	HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

	// Ustawienie timingu auto refresh
	Command.CommandMode             = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 4;
    HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

    // Konfiguracja wewnętrznych parametrów SDRAM, timingi
    tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_1          |
           SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |
           SDRAM_MODEREG_CAS_LATENCY_3           | // CAS 3
           SDRAM_MODEREG_OPERATING_MODE_STANDARD |
           SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    Command.CommandMode             = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
    Command.AutoRefreshNumber       = 1;
    Command.ModeRegisterDefinition  = tmpmrd;
    HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

    // Ustawienie licznika odświeżania
    HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH_COUNT);
}

/* Prywatna funkcja inicjalizująca sterownik wyświetlacza ILI9431
 * Skonfigurowano rejestry na podstawie dokumentacji oraz
 * zasugerowano się funkcją z biblioteki BSP */
static void initLCD(void)
{
	// --- 0x01: Software Reset ---
		LCD_Write_Cmd(0x01);
	    HAL_Delay(5);

	    // --- 0xCA ---
	    LCD_Write_Cmd(0xCA);
	    LCD_Write_Data(0xC3);
	    LCD_Write_Data(0x08);
	    LCD_Write_Data(0x50);

	    // --- 0xCF: Power Control B ---
	    LCD_Write_Cmd(0xCF);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0xC1);
	    LCD_Write_Data(0x30);

	    // --- 0xED: Power On Sequence ---
	    LCD_Write_Cmd(0xED);
	    LCD_Write_Data(0x64);
	    LCD_Write_Data(0x03);
	    LCD_Write_Data(0x12);
	    LCD_Write_Data(0x81);

	    // --- 0xE8: Driver Timing Control A ---
	    LCD_Write_Cmd(0xE8);
	    LCD_Write_Data(0x85);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x78);

	    // --- 0xCB: Power Control A ---
	    LCD_Write_Cmd(0xCB);
	    LCD_Write_Data(0x39);
	    LCD_Write_Data(0x2C);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x34);
	    LCD_Write_Data(0x02);

	    // --- 0xF7: Pump Ratio Control ---
	    LCD_Write_Cmd(0xF7);
	    LCD_Write_Data(0x20);

	    // --- 0xEA: Driver Timing Control B ---
	    LCD_Write_Cmd(0xEA);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);

	    // --- 0xB1: Frame Rate Control (Normal Mode) ---
	    LCD_Write_Cmd(0xB1);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x1B); // Częstotliwość klatek

	    // --- 0xB6: Display Function Control ---
	    LCD_Write_Cmd(0xB6);
	    LCD_Write_Data(0x0A);
	    LCD_Write_Data(0xA2);

	    // --- 0xC0: Power Control 1 ---
	    LCD_Write_Cmd(0xC0);
	    LCD_Write_Data(0x10);

	    // --- 0xC1: Power Control 2 ---
	    LCD_Write_Cmd(0xC1);
	    LCD_Write_Data(0x10);

	    // --- 0xC5: VCOM Control 1 ---
	    LCD_Write_Cmd(0xC5);
	    LCD_Write_Data(0x45);
	    LCD_Write_Data(0x15);

	    // --- 0xC7: VCOM Control 2 ---
	    LCD_Write_Cmd(0xC7);
	    LCD_Write_Data(0x90);

	    // --- 0x36: Memory Access Control (MAC) ---
	    // Ustawia orientację, kierunek skanowania i kolejność RGB
	    LCD_Write_Cmd(0x36);
	    LCD_Write_Data(0xC8); // 0xC8 to typowe ustawienie dla tego wyświetlacza

	    // --- 0xF2: 3 Gamma Enable ---
	    LCD_Write_Cmd(0xF2);
	    LCD_Write_Data(0x00);

	    // --- 0xB0: RGB Interface Signal Control ---
	    LCD_Write_Cmd(0xB0);
	    LCD_Write_Data(0xC2);

	    // --- 0xB6: Display Function Control
	    LCD_Write_Cmd(0xB6);
	    LCD_Write_Data(0x0A);
	    LCD_Write_Data(0xA7);
	    LCD_Write_Data(0x27);
	    LCD_Write_Data(0x04);

	    /* 0x2A: Ustawienie adresu kolumn (0 do 239) */
	    LCD_Write_Cmd(0x2A);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);               // Start Col: 0x0000
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0xEF);               // End Col: 0x00EF (239)

	    /* 0x2B: Ustawienie adresu stron (0 do 319) */
	    LCD_Write_Cmd(0x2B);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);               // Start Page: 0x0000
	    LCD_Write_Data(0x01);
	    LCD_Write_Data(0x3F);               // End Page: 0x013F (319)

	    // --- 0xF6: Interface Control ---
	    LCD_Write_Cmd(0xF6);
	    LCD_Write_Data(0x01);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x06);

	    // --- 0x2C: Memory Write (GRAM) ---
	    LCD_Write_Cmd(0x2C);
	    HAL_Delay(200);

	    // --- 0x26: Gamma Control ---
	    LCD_Write_Cmd(0x26);
	    LCD_Write_Data(0x01);

	    /* 0xE0: Korekcja Gamma: Positive */
	    LCD_Write_Cmd(0xE0);
	    LCD_Write_Data(0x0F);
	    // reszta ustawień Gamma
	    LCD_Write_Data(0x29);
	    LCD_Write_Data(0x24);
	    LCD_Write_Data(0x0C);
	    LCD_Write_Data(0x0E);
	    LCD_Write_Data(0x09);
	    LCD_Write_Data(0x4E);
	    LCD_Write_Data(0x78);
	    LCD_Write_Data(0x3C);
	    LCD_Write_Data(0x09);
	    LCD_Write_Data(0x13);
	    LCD_Write_Data(0x05);
	    LCD_Write_Data(0x17);
	    LCD_Write_Data(0x11);
	    LCD_Write_Data(0x00);

	    /* 0xE1: Korekcja Gamma: Negative */
	    LCD_Write_Cmd(0xE1);
	    LCD_Write_Data(0x00);
	    // reszta ustawień Gamma
	    LCD_Write_Data(0x16);
	    LCD_Write_Data(0x1B);
	    LCD_Write_Data(0x04);
	    LCD_Write_Data(0x11);
	    LCD_Write_Data(0x07);
	    LCD_Write_Data(0x31);
	    LCD_Write_Data(0x33);
	    LCD_Write_Data(0x42);
	    LCD_Write_Data(0x05);
	    LCD_Write_Data(0x0C);
	    LCD_Write_Data(0x0A);
	    LCD_Write_Data(0x28);
	    LCD_Write_Data(0x2F);
	    LCD_Write_Data(0x0F);

	    // --- 0x11: Sleep Out ---
	    LCD_Write_Cmd(0x11);
	    HAL_Delay(200);

	    // --- 0x29: Display ON ---
	    LCD_Write_Cmd(0x29);

	    // --- 0x2C: Następne dane to dane obrazu (GRAM) ---
	    LCD_Write_Cmd(0x2C);
}

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
