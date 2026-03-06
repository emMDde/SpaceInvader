#ifndef INC_TP_H_
#define INC_TP_H_

#include "main.h"

extern I2C_HandleTypeDef hi2c3;
extern float touchX;
extern float touchY;

void initTouchPanel(void);
void enableTouchIT(void);
void disableTouchIT(void);
void clearTouchIT(void);
void readTouchData(void);


#endif /* INC_TP_H_ */
