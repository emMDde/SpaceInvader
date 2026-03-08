#ifndef INC_GAME_MANAGER_HH_
#define INC_GAME_MANAGER_HH_

#include <S_Inv.hh>
extern "C"{
#include "main.h"
#include "gui.h"
#include "gyroscope.h"
#include "touch_panel.h"
#include "flash.h"

extern float angle;
extern float dps_x;
extern bool isHard;
extern bool isTouch;
extern float touchX;
extern float touchY;
extern int lastButtonTouched;
extern uint32_t lastTouchTime;
}

bool isInputLeft(void);
bool isInputRight(void);
void calculateGameInput(float);
void operateMenu(void);
void operateGame(void);
void operateEndMenu(void);

#endif /* INC_GAME_MANAGER_HH_ */
