#ifndef INC_FLASH_H_
#define INC_FLASH_H_

#include "main.h"

#define FLASH_SECTOR_ADDR   0x081E0000
#define FLASH_SECTOR_NUM     FLASH_SECTOR_23
#define FLASH_SECTOR_END_ADDR   0x081FFFFF
#define FLASH_SECTOR_SIZE       (128 * 1024)

void readRecord(void);
void eraseFlashSector(void);
void saveRecordToFlash(void);

#endif /* INC_FLASH_H_ */
