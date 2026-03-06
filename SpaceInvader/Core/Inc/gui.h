#ifndef INC_GUI_H_
#define INC_GUI_H_

#include "main.h"
#include "drawFunctions.h"
#include "lcd.h"
#include "button.h"
#include "touch_panel.h"

typedef enum {
    MAIN_PLAY,
    MAIN_OPTION,
	OPTION_DIFF,
	OPTION_CTR,
    MAIN_COUNT
} MainMenuButtonID;

typedef enum {
    END_RETRY,
    END_MENU,
	END_COUNT
} EndMenuButtonID;

extern DMA2D_HandleTypeDef hdma2d;

bool isButtonTouched(Button*);
void drawButton(Button*);

void drawMainMenu(Button*);
void drawMainMenuBG(void);
void handleMainMenuButtons(Button*);

void drawOptionMenu(Button*);
void drawOptionMenuBG(void);
void handleOptionMenuButtons(Button*);

void drawEndMenu(Button*);
void drawEndMenuBG(void);
void handleEndMenuButtons(Button*);

const char* IntToStr(int);


#endif /* INC_GUI_H_ */
