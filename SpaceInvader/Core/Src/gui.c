#include "gui.h"

bool isHard=false;
bool isTouch=false;
uint32_t lastTouchTime=0;
int lastButtonTouched=-1;

void handleEndMenuButtons(Button *buttons)
{
	if(isButtonTouched(&buttons[END_RETRY]))
	{
		buttons[END_RETRY].isPressed=true;
		drawEndMenu(buttons);
		HAL_Delay(200);
		State=GAME;
	}
	else if(isButtonTouched(&buttons[END_MENU]))
	{
		buttons[END_MENU].isPressed=true;
		drawEndMenu(buttons);
		HAL_Delay(200);
	    State=MENU;
	}
}

void drawEndMenuBG()
{
	uint32_t bg_color;
	uint32_t color;
	const char* title;
	if(isWon)
	{
		bg_color=0x8800FF00;
		color=GREEN;
		title="YOU WON";
	}
	else
	{
		bg_color=0x88FF0000;
		color=RED;
		title="GAME OVER";
	}
	drawShadow(bg_color, SAVE_BUFFER);
	textCenter=true;
	//drawString(LCD_HEIGHT/2+1, 40+1, 0, title, WHITE);
	//drawString(LCD_HEIGHT/2-1, 40-1, 0, title, WHITE);
	//drawString(LCD_HEIGHT/2-1, 40+1, 0, title, WHITE);
	//drawString(LCD_HEIGHT/2+1, 40-1, 0, title, WHITE);
	drawString(LCD_HEIGHT/2, 40, 0, title, color);
	drawString(LCD_HEIGHT/2-40, 80, 0, "Wynik:", WHITE);
	drawString(LCD_HEIGHT/2-30, 105, 0, "Rekord:", WHITE);
	textCenter=false;
	drawString(LCD_HEIGHT/2+35, 69, -3, IntToStr(points), YELLOW);
	drawString(LCD_HEIGHT/2+35, 94, -3, IntToStr(record), ORANGE);
}

void drawEndMenu(Button *buttons)
{
	swapBuffer();
	drawEndMenuBG();
	drawButton(&buttons[END_RETRY]);
	drawButton(&buttons[END_MENU]);
	displayFrame();
}

void handleOptionMenuButtons(Button* buttons)
{
	if(isButtonTouched(&buttons[OPTION_DIFF]))
	{
		if(lastButtonTouched!=OPTION_DIFF) isHard=!isHard;
		buttons[OPTION_DIFF].isPressed=isHard;
	    drawOptionMenu(buttons);
	    lastButtonTouched=OPTION_DIFF;
	}
	else if(isButtonTouched(&buttons[OPTION_CTR]))
	{
		if(lastButtonTouched!=OPTION_CTR) isTouch=!isTouch;
		buttons[OPTION_CTR].isPressed=isTouch;
	    drawOptionMenu(buttons);
	    lastButtonTouched=OPTION_CTR;
	}
	else if(touchX<320 && touchX>270 && touchY>0 && touchY<50)
	{
		State=MENU;
		drawMainMenu(buttons);
	}
	else lastButtonTouched=-1;
}

void drawOptionMenuBG()
{
	setBackgroundColor(0xFF000000);
	drawBullet(25+BULLET_HEIGHT/2,80);
	drawBullet(25+BULLET_HEIGHT/2,150);
	drawEnemy(40,10);
	drawEnemy(310,68);
	drawEnemy(310, 158);
	drawPlayer(40,200);
	drawString(90,30,0,"Trudnosc:",0xFFFFFFFF);
	drawString(70,120,0,"Sterowanie:",0xFFFFFFFF);
	drawRect(319, 0, 0xFFFF0000, 40, 40);
	drawChar(293,10,'X',0xFFFFFFFF);
}

void drawOptionMenu(Button* buttons)
{
	swapBuffer();
	drawOptionMenuBG();
	drawButton(&buttons[OPTION_DIFF]);
	drawButton(&buttons[OPTION_CTR]);
	displayFrame();
}

void handleMainMenuButtons(Button* buttons)
{
	if(isButtonTouched(&buttons[MAIN_PLAY]))
	{
		buttons[MAIN_PLAY].isPressed=true;
		drawMainMenu(buttons);
	    HAL_Delay(200);
	    State=GAME;
	}
	else if(isButtonTouched(&buttons[MAIN_OPTION]))
	{
		buttons[MAIN_OPTION].isPressed=true;
	    drawMainMenu(buttons);
	    buttons[MAIN_OPTION].isPressed=false;
	    HAL_Delay(200);
	    State=OPTIONS;
	    drawOptionMenu(buttons);
	}
}

void drawMainMenuBG()
{
	setBackgroundColor(0xFF000000);
	drawBullet(60,70);
	drawBullet(160+BULLET_WIDTH/2-1,LCD_WIDTH-1-PLAYER_HEIGHT-BULLET_HEIGHT-7);
	drawBullet(250,155);
	drawString(55, 60, 0,"Space Invader", 0xFFFFFFFF);
	drawPlayer(160+PLAYER_WIDTH/2,LCD_WIDTH-1-PLAYER_HEIGHT);
	for(int i=0; i<8;++i)
	{
		drawEnemy(315-i*40,5);
	}
	drawShield(100, 180, 1);
	drawShield(300, 180, 1);
}

void drawMainMenu(Button* buttons)
{
	swapBuffer();
	drawMainMenuBG();
	drawButton(&buttons[MAIN_PLAY]);
	drawButton(&buttons[MAIN_OPTION]);
	displayFrame();
}

void drawButton(Button* btn)
{
    const uint32_t* texture = btn->isPressed ? BUTTON_PRESS : BUTTON;
    const char* label = btn->isPressed ? btn->pressed_label : btn->label;
    drawTable(btn->x, btn->y, texture, BUTTON_WIDTH, BUTTON_HEIGHT);
    textCenter = true;
    drawString(btn->x-BUTTON_WIDTH/2+btn->label_xShift, btn->y+BUTTON_HEIGHT/2+2, btn->label_offset, label, btn->text_color);
    textCenter = false;
}

bool isButtonTouched(Button* btn)
{
    if(touchX < btn->x && touchX > btn->x-BUTTON_WIDTH && touchY > btn->y && touchY < btn->y+BUTTON_HEIGHT)
    {
        return true;
    }
    return false;
}

const char* IntToStr(int x)
{
    static char buffer[12];
    int i = 0;
    bool isNegative = x<0;

    if(isNegative) x=-x;

    do{
        int value=x%10;
        buffer[i++]=value+'0';
        x=x/10;
    } while(x!=0);

    if(isNegative) buffer[i++] = '-';
    buffer[i]='\0';

    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char temp = buffer[start];
        buffer[start] = buffer[end];
        buffer[end] = temp;
        start++;
        end--;
    }

    return buffer;
}


