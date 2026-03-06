#include "sram.h"

/* Private initialization function for the SDRAM.
 * Uses default timings for the memory on the STM32F429I board,
 * inspired by the reference function from the BSP library. */
void initSDRAM(SDRAM_HandleTypeDef *hsdram)
{
	volatile uint32_t tmpmrd = 0;
	FMC_SDRAM_CommandTypeDef Command;

	// CKE clock enabled
	Command.CommandMode             = FMC_SDRAM_CMD_CLK_ENABLE;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 1;
	Command.ModeRegisterDefinition  = 0;
	HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

	HAL_Delay(1);

	// Precharge all command
	Command.CommandMode             = FMC_SDRAM_CMD_PALL;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 1;
	HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

	// Auto refresh timing settings
	Command.CommandMode             = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.CommandTarget           = FMC_SDRAM_CMD_TARGET_BANK2;
	Command.AutoRefreshNumber       = 4;
    HAL_SDRAM_SendCommand(hsdram, &Command, SDRAM_TIMEOUT);

    // SDRAM parameters, timings
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

    // Refresh counter settings
    HAL_SDRAM_ProgramRefreshRate(hsdram, SDRAM_REFRESH_COUNT);
}

void testSDRAM()
{
	volatile uint32_t *fb = (uint32_t *)CURRENT_BUFFER;
	fb[1] = 0xCAFEBABE;

	if (fb[1] == 0xCAFEBABE) HAL_GPIO_WritePin(GPIOG, Dioda1_Pin, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOG, Dioda2_Pin, GPIO_PIN_SET);
}


