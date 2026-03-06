#include "touch_panel.h"


static void TouchPanel_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t dev_write_adr=0x82;
	HAL_I2C_Mem_Write(&hi2c3, dev_write_adr, reg, 1, &value, 1, HAL_MAX_DELAY);
}

void initTouchPanel()
{
	TouchPanel_Write_Reg(0x03, 0x02); // Reset
	HAL_Delay(10);
	TouchPanel_Write_Reg(0x03, 0x00); // Wyjscie z resetu
	TouchPanel_Write_Reg(0x04, 0x0C); // Wlaczenie zegarow dla TS i Temp
	TouchPanel_Write_Reg(0x09, 0x00); // Wylaczenie przerwan
	TouchPanel_Write_Reg(0x0A, 0x02); // Wlaczenie przerwania od danych dotyku
	TouchPanel_Write_Reg(0x40, 0x03); // Wlaczenie sensora dotyku
	TouchPanel_Write_Reg(0x41, 0x9B); // Ustawienia specyfikacji odczytu //A3
	TouchPanel_Write_Reg(0x4A, 0x01); // Ilesc dotykow w buforze przy przerwaniu - 1
	TouchPanel_Write_Reg(0x58, 0x01); // Ustawienia prądu
}

void enableTouchIT()
{
	TouchPanel_Write_Reg(0x4B, 0x01); // Reset FIFO
	HAL_Delay(5);
	TouchPanel_Write_Reg(0x0B, 0xFF); // Wyczyszczenie przerwań
	TouchPanel_Write_Reg(0x4B, 0x00); // Wlaczenie FIFO
	TouchPanel_Write_Reg(0x09, 0x01); // Wlaczenie przerwan
}

void disableTouchIT(void)
{
	TouchPanel_Write_Reg(0x09, 0x00); // Wylaczenie przerwan
	touchX=0;
	touchY=0;
}

void clearTouchIT(void)
{
	TouchPanel_Write_Reg(0x4B, 0x01); // Reset FIFO
	TouchPanel_Write_Reg(0x0B, 0xFF); // Wyczyszczenie przerwań
	TouchPanel_Write_Reg(0x4B, 0x00); // Wlaczenie FIFO
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
