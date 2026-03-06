#ifndef INC_GYROSCOPE_H_
#define INC_GYROSCOPE_H_

#include "main.h"

extern SPI_HandleTypeDef hspi5;

void initGyro(void);
void enableGyroIT(void);
void disableGyroIT(void);
void readGyroData(void);
void calibrateGyro(void);
void calculateAngle(float);

#endif
