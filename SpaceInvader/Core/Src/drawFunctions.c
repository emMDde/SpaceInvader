#include "drawFunctions.h"

void drawPlayer(int posX, int posY)
{
	drawTable(posX, posY, PLAYER_TEXTURE, PLAYER_WIDTH, PLAYER_HEIGHT);
}

void drawEnemy(int posX, int posY)
{
	drawTable(posX, posY, ENEMY_TEXTURE, ENEMY_WIDTH, ENEMY_HEIGHT);
}

void drawShield(int posX, int posY, int health)
{
	uint32_t color;
	if(health==3) color=GREEN;
	if(health==2) color=YELLOW;
	if(health==1) color=RED;
	drawRect(posX, posY, color, SHIELD_WIDTH, SHIELD_HEIGHT);
}

void drawBullet(int posX, int posY)
{
	drawTable(posX, posY, BULLET_TEXTURE, BULLET_WIDTH, BULLET_HEIGHT);
}

void drawEnemyBullet(int posX, int posY)
{
	drawTable(posX, posY, ENEMY_BULLET_TEXTURE, BULLET_WIDTH, BULLET_HEIGHT);
}

void drawArrowButtons()
{
	drawTable(RIGHT_ARROW_X, ARROW_Y, RIGHT_ARROW_TEXTURE, BUTTON_ARROW_WIDTH, BUTTON_ARROW_HEIGHT);
	drawTable(LEFT_ARROW_X, ARROW_Y, LEFT_ARROW_TEXTURE, BUTTON_ARROW_WIDTH, BUTTON_ARROW_HEIGHT);
}

void setBackgroundColor(uint32_t color)
{
	hdma2d.Init.Mode = DMA2D_R2M;
	hdma2d.Init.OutputOffset = 0;
	HAL_DMA2D_Init(&hdma2d);
	HAL_DMA2D_Start(&hdma2d, color, CURRENT_BUFFER, LCD_WIDTH, LCD_HEIGHT);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
}

void drawShadow(uint32_t color, uint32_t background)
{
	setBackgroundColor(color);
	hdma2d.Init.Mode = DMA2D_M2M_BLEND;
	HAL_DMA2D_Init(&hdma2d);
	hdma2d.LayerCfg[0].InputOffset = 0;
	HAL_DMA2D_ConfigLayer(&hdma2d, 0);
	HAL_DMA2D_BlendingStart(&hdma2d, CURRENT_BUFFER, background, CURRENT_BUFFER, LCD_WIDTH, LCD_HEIGHT);
	HAL_DMA2D_PollForTransfer(&hdma2d, HAL_MAX_DELAY);
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

void drawTable(int posX, int posY, const uint32_t *table_adr, int tabHeight, int tabWidth)
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

void drawString(int posX, int posY, int offset, const char* string, uint32_t color)
{
	if(posX>=LCD_HEIGHT || posY>=LCD_WIDTH || posX<0 || posY<0) return;
	int textSize=0;
	const char *ptr=string;
	while(*ptr++)
	{
		textSize++;
	}
	//if(offset<0) offset=0;
	if(textCenter) posX-=(float)(textSize/2.f)*(Font24.Width+offset);
	if(textCenter) posY-=Font24.Height/2;
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


