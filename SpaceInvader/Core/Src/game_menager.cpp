#include <game_menager.hh>

static uint32_t last_tick=0;

bool isInputLeft()
{
	if(isTouch)
	{
		return (touchX <= LEFT_ARROW_X && touchX >= (LEFT_ARROW_X - BUTTON_ARROW_WIDTH) && touchY >= ARROW_Y  && touchY <= (ARROW_Y + BUTTON_ARROW_HEIGHT));
	}
	else return (angle>10.f);
}

bool isInputRight()
{
	if(isTouch)
	{
		return (touchX <= RIGHT_ARROW_X && touchX >= (RIGHT_ARROW_X - BUTTON_ARROW_WIDTH) && touchY >= ARROW_Y   && touchY <= (ARROW_Y + BUTTON_ARROW_HEIGHT));
	}
	else return (angle<-10.f);
}

void calculateGameInput(float dt)
{
	if(isTouch)
	{
		if(TouchDetected)
		{
			TouchDetected=0;
			readTouchData();
			clearTouchIT();
		}
		else
		{
			touchX=0;
			touchY=0;
		}
	}
	else calculateAngle(dt);
}

void operateMenu()
{
	Button buttons[MAIN_COUNT] = {
		[MAIN_PLAY] = {.x = 100, .y = 100, .isPressed = false, .label = "GRAJ", .pressed_label = "GRAJ", .text_color = 0x00000000},
		[MAIN_OPTION] = {.x = 300, .y = 100, .isPressed = false, .label = "OPCJE", .pressed_label = "OPCJE", .label_offset=-1, .text_color = 0x00000000},
		[OPTION_DIFF] = {.x = 200, .y = 60, .isPressed = isHard, .label = "EASY", .pressed_label = "HARD", .label_xShift=2, .text_color=0xFFFFFFFF},
		[OPTION_CTR] = {.x = 200, .y = 150, .isPressed = isTouch, .label = "OBROT", .pressed_label = "DOTYK", .label_offset=-2, .text_color=0xFFFFFFFF}
	};

	drawMainMenu(buttons);
	enableTouchIT();
	while(State!=GAME)
	{
		if(TouchDetected)
		{
			TouchDetected=0;
			readTouchData();
			if(State==MENU)
			{
				handleMainMenuButtons(buttons);
			}
			if(State==OPTIONS)
			{
				lastTouchTime=HAL_GetTick();
				handleOptionMenuButtons(buttons);
			}
			clearTouchIT();
		}
		else if(HAL_GetTick()-lastTouchTime>BUTTON_DEBOUNCE) lastButtonTouched=-1;
	}
	disableTouchIT();
}

void operateGame()
{
	S_Inv gra(isHard);
	if(isTouch) enableTouchIT();
	angle=0.0f;
	dps_x=0.0f;
	float accumulator=0.0f;
	const float PHYSICS_STEP=0.01f;
	last_tick=HAL_GetTick();
	while(!gra.isOver())
	{
	    uint32_t current_tick = HAL_GetTick();
	    float dt=(current_tick-last_tick)/1000.0f; // FrameTime
	    last_tick=current_tick;

		calculateGameInput(dt);
		accumulator+=dt;
		while(accumulator>=PHYSICS_STEP)
		{
			gra.updatePhysics(PHYSICS_STEP, isInputLeft(), isInputRight());
			accumulator-=PHYSICS_STEP;
		}

		swapBuffer();
		gra.renderGame();
		if(isTouch) drawArrowButtons();
		displayFrame();
	}
	isWon=gra.getResult();
	points=gra.getPoints();
	State=END;
	if(isTouch) disableTouchIT();
}

void operateEndMenu()
{
	Button buttons [END_COUNT] = {
		[END_RETRY] = {.x = 120, .y = 140, .isPressed = false, .label = "PONOW", .pressed_label = "PONOW", .label_offset=-3, .label_xShift=-1, .text_color = 0x00FFFFFF},
		[END_MENU] = {.x = 280, .y = 140, .isPressed = false, .label = "MENU", .pressed_label = "MENU", .label_offset=0, .label_xShift=3, .text_color = 0x00FFFFFF}
	};
	saveBuffer(SAVE_BUFFER, CURRENT_BACK_BUFFER); //Widac moment uderzenia, gdy jest back buffer
	readRecord();
	drawEndMenu(buttons);
	enableTouchIT();
	while(State==END)
	{
		if(TouchDetected)
		{
			TouchDetected=0;
			readTouchData();
			handleEndMenuButtons(buttons);
			clearTouchIT();
		}
	}
	disableTouchIT();
	if(points>record && isWon) saveRecordToFlash();
}

