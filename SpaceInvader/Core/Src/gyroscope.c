#include "gyroscope.h"

float angle=0.f;
float dps_x=0.f;
static float dps_offset=0.f;

static void Gyro_Write_Reg(uint8_t reg, uint8_t value)
{
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, &reg, 1, 100);
	HAL_SPI_Transmit(&hspi5, &value, 1, 100);
	HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
}

void initGyro()
{
	Gyro_Write_Reg(0x24, 0x80); // Reset
	HAL_Delay(10);
	Gyro_Write_Reg(0x20, 0x5F);  // Ctrl_reg1: Power ON, XYZ enabled, LPF Cutoff 25 Hz, ODR 200 Hz
	Gyro_Write_Reg(0x21, 0x00);  // Ctrl_reg2: Settings: Filter HPF 0.5 Hz
	Gyro_Write_Reg(0x22, 0x00);  // Ctrl_reg3: Interrupt system enabled
	Gyro_Write_Reg(0x23, 0x00);  // Ctrl_reg4: Data range settings
	Gyro_Write_Reg(0x24, 0x00);  // Ctrl_reg5: HPF off
	HAL_Delay(50);
	readGyroData();
}

void enableGyroIT()
{
	Gyro_Write_Reg(0x22, 0x08);  // Ctrl_reg3: Interrupt system enabled
}

void disableGyroIT(void)
{
	Gyro_Write_Reg(0x22, 0x00);  // Ctrl_reg3: Interrupt system disabled
}

void readGyroData()
{
    uint8_t reg_adresses[3]={0xE8,0,0}; // Data address in OX. 0xE8=0x80(READ)+0x40(INCREMENT)+0x28(ADRESS)
    uint8_t reg_values[3]={0,0,0};
    int16_t mesurement;
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi5, reg_adresses, reg_values, 3, 100);
    HAL_GPIO_WritePin(GYRO_CS_GPIO_Port, GYRO_CS_Pin, GPIO_PIN_SET);
    mesurement=(int16_t)((reg_values[2] << 8) | reg_values[1]);
    dps_x=mesurement*0.00875f;
}

void calibrateGyro()
{
	int samples=100;
	float sum=0.00f;
	for(int i=0; i<samples; ++i)
	{
		HAL_Delay(10);
		readGyroData();
		sum+=dps_x;
	}
	dps_offset=(float)sum/samples;
}

void calculateAngle(float dt)
{
    readGyroData();
    dps_x-=dps_offset;
    if(dps_x<1.f && dps_x>-1.f) return;
    angle+=dps_x*dt;
    //angle*=0.99f;
    if(angle>45.f) angle=45.f;
    if(angle<-45.f) angle=-45.f;
    //printf("Interrupt: x:%f, angle:%f\r\n",dps_x,angle);
}

