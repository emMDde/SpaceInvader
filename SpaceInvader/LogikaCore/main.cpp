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
<<<<<<< Updated upstream:SpaceInvader/Core/Src/main.cpp
#include "S_Inv.h"
#include "stdlib.h"
=======
#include "fonts.h"
#include "bullet.h"
>>>>>>> Stashed changes:SpaceInvader/Core/Src/main.c
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FRAMEBUFFER_ADDR ((uint32_t)0xD0000000)
#define FRAMEBUFFER_END ((uint32_t)FRAMEBUFFER_ADDR+LCD_HEIGHT*LCD_WIDTH*4)
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
float dps;
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

static void initGyro(void);
void readGyroData(void);
extern "C"
{
int _write(int, char *, int);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void transformPosOrientation(int* posX, int* posY)
{
	int temp=*posY;
	*posY=LCD_HEIGHT-1-*posX;
	*posX=temp;
}

void clearFramebuffer(uint32_t color)
{
    volatile uint32_t *fb = (uint32_t *)FRAMEBUFFER_ADDR;
    for (int i = 0; i < 240*320; ++i)
    {
        fb[i] = color;
    }
}

void drawChar(int posX, int posY, char c, uint32_t color)
{
	if(posX>=LCD_HEIGHT || posY>=LCD_WIDTH || posX<0 || posY<0) return;
	transformPosOrientation(&posX, &posY);
	int bytes_count=(Font24.Width+7)/8; // Ilość bajtów zawierających jeden wiersz tabeli
	int index=(c-' ')*Font24.Height*bytes_count; // Indeks znaku w tablicy cziocnki
	uint32_t *fb_start = (uint32_t *)FRAMEBUFFER_ADDR;
	uint32_t *fb_end = (uint32_t *)FRAMEBUFFER_END;
	volatile uint32_t *fb = (uint32_t *)FRAMEBUFFER_ADDR+posX+LCD_WIDTH*posY;

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

void drawString(int posX, int posY, int offset, char* string, uint32_t color)
{
	if(posX>=LCD_HEIGHT || posY>=LCD_WIDTH || posX<0 || posY<0) return;
	int textSize=0;
	char *ptr=string;
	while(*ptr++)
	{
		textSize++;
	}
	if(offset<0) offset=0;
	for(int i=0; i<textSize; i++)
	{
		drawChar(posX+(Font24.Width+offset)*i, posY, string[i], color);
	}
}

void drawPlayer(int posX, int posY);

void drawEnemy(int posX, int posY);

void drawShield(int posX, int posY);

void drawMainMenu()
{
	  clearFramebuffer(0xFF000000);
	  drawString(55, 103, 0,"Space Invader", 0xFFFFFFFF);
}


void drawTable(int posX, int posY)
{
	transformPosOrientation(&posX, &posY);

	hdma2d.Init.OutputOffset = LCD_WIDTH - BULLET_WIDTH; // Offset dla FrameBuffera
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_ConfigLayer(&hdma2d, 1); // Konfiguracja warstwy 1 (Foreground/Źródło)
	    hdma2d.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888; // Format źródła
	    hdma2d.LayerCfg[1].InputOffset = 0; // Bufor tekstury jest zwarty
	HAL_DMA2D_Start(&hdma2d, (uint32_t)BULLET_TEXTURE, FRAMEBUFFER_ADDR+(posX+posY*LCD_WIDTH)*4, BULLET_WIDTH, BULLET_HEIGHT);
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
  MX_LTDC_Init();
  MX_FMC_Init();
  MX_DMA2D_Init();
  MX_I2C3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  initGyro();
  testSDRAM();
<<<<<<< Updated upstream:SpaceInvader/Core/Src/main.cpp
  clearFramebuffer(0x00FF0000);

  ///////////////DO TESTOWANIA

  printf("Naciśnij przycisk, aby startować...\r\n");

  while (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
  {
//Proba nadania losowosci ziarna dla srand w zaleznosci kiedy nacisnie sie przycisk
  }

  srand(HAL_GetTick());

  S_Inv gra;
  for (int i=0;i<20;i++)
  {
	  gra.movePlayerLeft();
  }
  gra.movePlayerRight();
  gra.updateEnemiesPositions();
  gra.updateEnemiesPositions();
  gra.playerShoot();
  gra.enemyShoot();

  for (int i=0;i<18;i++)
  {
	  gra.updateEnemyBullets();
  }
  gra.movePlayerRight();

  for (int i=0;i<13;i++)
  {
	  gra.updatePlayerBullets();
  }
  gra.playerShoot();
  gra.playerShoot();
  gra.playerShoot();
  for (int i=0;i<5;i++)
  {
	  gra.updatePlayerBullets();
  }
  gra.renderGame();
  int i=0;

  ///////////////KONIEC TESTOWANIA
=======
  drawMainMenu();
  drawTable(65,103);
>>>>>>> Stashed changes:SpaceInvader/Core/Src/main.c
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //printf("Test\r\n");
	  i++;
	  if(i==1)
	  {
		  gra.printBoard();
		  printf("%d\r\n",gra.getPoints());
	  }
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
  hdma2d.Init.Mode = DMA2D_M2M;
  hdma2d.Init.ColorMode = DMA2D_OUTPUT_ARGB8888;
  hdma2d.Init.OutputOffset = 0;
  if (HAL_DMA2D_Init(&hdma2d) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DMA2D_Init 2 */

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
  hi2c3.Init.ClockSpeed = 100000;
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
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
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
  HAL_GPIO_WritePin(GPIOC, GYRO_CS_Pin|LCD_CS_Pin, GPIO_PIN_RESET);

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

  /*Configure GPIO pins : GYRO_INT1_Pin GYRO_INT2_Pin */
  GPIO_InitStruct.Pin = GYRO_INT1_Pin|GYRO_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LCD_WR_Pin */
  GPIO_InitStruct.Pin = LCD_WR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LCD_WR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TP_INT_Pin */
  GPIO_InitStruct.Pin = TP_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
  HAL_GPIO_Init(TP_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Dioda1_Pin Dioda2_Pin */
  GPIO_InitStruct.Pin = Dioda1_Pin|Dioda2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static void initGyro()
{
	uint8_t ctrl_reg1 = 0x0F;  // Power ON, XYZ włączone
	uint8_t addres = 0x20;  // Adres CTRL_REG1

	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &addres, 1, 100);
	HAL_SPI_Transmit(&hspi5, &ctrl_reg1, 1, 100);
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
}

void readGyroData()
{
    uint8_t reg_adresses[3]={0xE8,0,0}; // Adresy odczytów czujnika w OSI X.0xE8=0x80-READ+0x40-INCREMENT+0x28
    uint8_t reg_values[3]={0,0,0};
    int16_t mesurement;

    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi5, reg_adresses, reg_values, 3, 100);
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
    mesurement=(reg_values[2] << 8) | reg_values[1];
    dps=mesurement*0.00875f;
}

void testSDRAM()
{
	volatile uint32_t *fb = (uint32_t *)FRAMEBUFFER_ADDR;
	fb[1] = 0xCAFEBABE;

	if (fb[1] == 0xCAFEBABE) HAL_GPIO_WritePin(GPIOG, Dioda1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOG, Dioda2_Pin, GPIO_PIN_SET);
}
extern "C"
{
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t*) ptr, len, HAL_MAX_DELAY);
	return len;
}
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
