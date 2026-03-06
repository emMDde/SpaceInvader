#ifndef INC_DRAWFUNCTIONS_H_
#define INC_DRAWFUNCTIONS_H_

#include "main.h"
#include "colors.h"
#include "fonts.h"
#include "bullet.h"
#include "player.h"
#include "enemy.h"
#include "arrow.h"

#define LEFT_ARROW_X BUTTON_ARROW_WIDTH+8
#define RIGHT_ARROW_X LCD_HEIGHT-8
#define ARROW_Y LCD_WIDTH-2.2*BUTTON_ARROW_HEIGHT

extern DMA2D_HandleTypeDef hdma2d;

void drawPlayer(int, int);
void drawEnemy(int, int);
void drawShield(int, int, int);
void drawBullet(int, int);
void drawEnemyBullet(int, int);
void setBackgroundColor(uint32_t);
void drawArrowButtons(void);

void transformPosOrientation(int*, int*);
void drawChar(int, int, char, uint32_t);
void drawString(int, int, int, const char*, uint32_t);
void drawTable(int, int, const uint32_t*, int, int);
void drawRect(int, int, uint32_t, int, int);
void drawShadow(uint32_t, uint32_t);

#endif
