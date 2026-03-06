#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "main.h"

extern DMA2D_HandleTypeDef hdma2d;
extern SPI_HandleTypeDef hspi5;
extern LTDC_HandleTypeDef hltdc;


void clearFramebuffer(uint32_t);
void swapBuffer(void);
void saveBuffer(uint32_t, uint32_t);
void initLCD(void);
void displayFrame(void);
void forceLayerOpaque(int);
void restoreLayerAlpha(int);

#endif /* INC_LCD_H_ */
