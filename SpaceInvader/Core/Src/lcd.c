#include "lcd.h"

float touchX=0.f;
float touchY=0.f;

void saveBuffer(uint32_t destination, uint32_t source)
{
	hdma2d.Init.Mode = DMA2D_M2M;
	hdma2d.Init.OutputOffset = 0;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_Start(&hdma2d, source, destination, LCD_WIDTH, LCD_HEIGHT);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void swapBuffer()
{
	uint32_t temp = CURRENT_BUFFER;
	CURRENT_BUFFER = CURRENT_BACK_BUFFER;
	CURRENT_BACK_BUFFER = temp;

	temp = CURRENT_BUFFER_END;
	CURRENT_BUFFER_END = CURRENT_BACK_BUFFER_END;
	CURRENT_BACK_BUFFER_END = temp;

    while(hltdc.Instance->SRCR & LTDC_SRCR_VBR);
}

void clearFramebuffer(uint32_t color)
{
    volatile uint32_t *fb = (uint32_t *)CURRENT_BUFFER;
    for (int i = 0; i < 240*320; ++i)
    {
        fb[i] = color;
    }
}

void restoreLayerAlpha(int layer)
{
	hdma2d.LayerCfg[layer].AlphaMode = DMA2D_NO_MODIF_ALPHA;
	HAL_DMA2D_ConfigLayer(&hdma2d, layer);
}

void forceLayerOpaque(int layer)
{
	hdma2d.LayerCfg[layer].AlphaMode = DMA2D_REPLACE_ALPHA;
	hdma2d.LayerCfg[layer].InputAlpha = 0xFF;
	HAL_DMA2D_ConfigLayer(&hdma2d, layer);
}

void displayFrame()
{
    HAL_LTDC_SetAddress(&hltdc, CURRENT_BUFFER, 0);
    HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
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

/* Private init function for the ILI9341 display driver.
 * Registers configured according to the datasheet,
 * taking hints from the STM32 BSP library function. */
void initLCD(void)
{
		// 0x01: Software Reset
		LCD_Write_Cmd(0x01);
	    HAL_Delay(5);

	    // 0xCA
	    LCD_Write_Cmd(0xCA);
	    LCD_Write_Data(0xC3);
	    LCD_Write_Data(0x08);
	    LCD_Write_Data(0x50);

	    // 0xCF: Power Control B
	    LCD_Write_Cmd(0xCF);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0xC1);
	    LCD_Write_Data(0x30);

	    // 0xED: Power On Sequence
	    LCD_Write_Cmd(0xED);
	    LCD_Write_Data(0x64);
	    LCD_Write_Data(0x03);
	    LCD_Write_Data(0x12);
	    LCD_Write_Data(0x81);

	    // 0xE8: Driver Timing Control A
	    LCD_Write_Cmd(0xE8);
	    LCD_Write_Data(0x85);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x78);

	    // 0xCB: Power Control A
	    LCD_Write_Cmd(0xCB);
	    LCD_Write_Data(0x39);
	    LCD_Write_Data(0x2C);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x34);
	    LCD_Write_Data(0x02);

	    // 0xF7: Pump Ratio Control
	    LCD_Write_Cmd(0xF7);
	    LCD_Write_Data(0x20);

	    // 0xEA: Driver Timing Control B
	    LCD_Write_Cmd(0xEA);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);

	    // 0xB1: Frame Rate Control
	    LCD_Write_Cmd(0xB1);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x1B); // Frame frequency

	    // 0xB6: Display Function Control
	    LCD_Write_Cmd(0xB6);
	    LCD_Write_Data(0x0A);
	    LCD_Write_Data(0xA2);

	    // 0xC0: Power Control 1
	    LCD_Write_Cmd(0xC0);
	    LCD_Write_Data(0x10);

	    // 0xC1: Power Control 2
	    LCD_Write_Cmd(0xC1);
	    LCD_Write_Data(0x10);

	    // 0xC5: VCOM Control 1
	    LCD_Write_Cmd(0xC5);
	    LCD_Write_Data(0x45);
	    LCD_Write_Data(0x15);

	    // 0xC7: VCOM Control 2
	    LCD_Write_Cmd(0xC7);
	    LCD_Write_Data(0x90);

	    // 0x36: Memory Access Control (MAC)
	    // Orientation, scan direction, RGB sequence
	    LCD_Write_Cmd(0x36);
	    LCD_Write_Data(0xC8);

	    // 0xF2: 3 Gamma Enable
	    LCD_Write_Cmd(0xF2);
	    LCD_Write_Data(0x00);

	    // 0xB0: RGB Interface Signal Control
	    LCD_Write_Cmd(0xB0);
	    LCD_Write_Data(0xC2);

	    // 0xB6: Display Function Control
	    LCD_Write_Cmd(0xB6);
	    LCD_Write_Data(0x0A);
	    LCD_Write_Data(0xA7);
	    LCD_Write_Data(0x27);
	    LCD_Write_Data(0x04);

	    /* 0x2A: Column address set (0 - 239) */
	    LCD_Write_Cmd(0x2A);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);               // Start Col: 0x0000
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0xEF);               // End Col: 0x00EF (239)

	    /* 0x2B: Page address set (0 do 319) */
	    LCD_Write_Cmd(0x2B);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x00);               // Start Page: 0x0000
	    LCD_Write_Data(0x01);
	    LCD_Write_Data(0x3F);               // End Page: 0x013F (319)

	    // 0xF6: Interface Control
	    LCD_Write_Cmd(0xF6);
	    LCD_Write_Data(0x01);
	    LCD_Write_Data(0x00);
	    LCD_Write_Data(0x06);

	    // 0x2C: Memory Write (GRAM)
	    LCD_Write_Cmd(0x2C);
	    HAL_Delay(200);

	    // 0x26: Gamma Control
	    LCD_Write_Cmd(0x26);
	    LCD_Write_Data(0x01);

	    // 0xE0: Gamma correction: Positive
	    LCD_Write_Cmd(0xE0);
	    LCD_Write_Data(0x0F);
	    // rest gamma settings
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

	    // 0xE1: Gamma correction: Negative
	    LCD_Write_Cmd(0xE1);
	    LCD_Write_Data(0x00);
	    // rest gamma settings
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

	    // 0x11: Sleep Out
	    LCD_Write_Cmd(0x11);
	    HAL_Delay(200);

	    // 0x29: !Display ON!
	    LCD_Write_Cmd(0x29);

	    // 0x2C: Next display data (GRAM)
	    LCD_Write_Cmd(0x2C);
}


