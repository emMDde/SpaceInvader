#ifndef INC_SRAM_H_
#define INC_SRAM_H_

#include "main.h"

#define SDRAM_MODEREG_BURST_LENGTH_1          ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   ((uint16_t)0x0000)
#define SDRAM_MODEREG_CAS_LATENCY_3           ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE  ((uint16_t)0x0200)
#define SDRAM_TIMEOUT                 ((uint32_t)0xFFFF)
#define SDRAM_REFRESH_COUNT           ((uint32_t)0x0603)

void initSDRAM(SDRAM_HandleTypeDef *);
void testSDRAM(void);

#endif /* INC_SRAM_H_ */
