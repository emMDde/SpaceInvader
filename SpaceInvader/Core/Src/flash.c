#include "flash.h"

void readRecord()
{
	uint32_t* addr = (uint32_t*)FLASH_SECTOR_ADDR;
	uint32_t* end  = (uint32_t*)FLASH_SECTOR_END_ADDR;

	while(addr<end)
	{
		if(*addr==0xFFFFFFFF) break;
		addr++;
	}
	if(addr==(uint32_t*)FLASH_SECTOR_ADDR) record=0;
	else record=(int)*(addr-1);
}

void eraseFlashSector()
{
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SectorError;

	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector       = FLASH_SECTOR_23;
	EraseInitStruct.NbSectors    = 1;
	EraseInitStruct.Banks        = FLASH_BANK_2;

	HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError);
	HAL_FLASH_Lock();
}

void saveRecordToFlash()
{
	uint32_t* addr = (uint32_t*)FLASH_SECTOR_ADDR;
	uint32_t* end  = (uint32_t*)FLASH_SECTOR_END_ADDR;

	while (addr<end)
	{
		if(*addr==0xFFFFFFFF) break;
		addr++;
	}

	if(addr>=end)
	{
		eraseFlashSector();
	    addr=(uint32_t*)FLASH_SECTOR_ADDR;
	}

	HAL_FLASH_Unlock();
	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)addr, (uint32_t)points);
	HAL_FLASH_Lock();
}

