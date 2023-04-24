#include "main.h"
#include "tim.h"
#include "ultra_sonic.h"
#include <stdlib.h>

// ------------------- Private Variable ------------------------------
uint8_t IS_FIRST_CAPTURED[2];
uint8_t distance[2];	// in cms
uint32_t first_val[2];	// first time stamp
uint32_t second_val[2]; // second time stamp
uint32_t difference[2]; // in microseconds

uint8_t freshData = 0;
uint8_t activeSensor;
uint8_t readInProgress;

uint16_t startRead;
uint16_t Timeout = 500;


uint16_t frontDist = 0;
uint16_t rightDist = 0;


// ------------------- Private functions ------------------------------
void delay(uint16_t time)
{
	__HAL_TIM_SET_COUNTER(&htim15, 0);
	while (__HAL_TIM_GET_COUNTER(&htim15) < time)
	{
	};
}


void USTick(){
	if(!readInProgress){
		// No read in progress
		if(activeSensor){ // If just read 1, read 2 this time
			TriggerUSA();
		}else{
			TriggerUSB();
		}
	}
	if(HAL_GetTick()- startRead > Timeout){
		readInProgress = 0;
		IS_FIRST_CAPTURED[0] = 0;
		IS_FIRST_CAPTURED[1] = 0;
	}
}


// initialize ultra sonic sensor
void ultra_init()
{
	IS_FIRST_CAPTURED[0] = 0;
	IS_FIRST_CAPTURED[1] = 0;
	distance[0] 	= 0;
	distance[1] 	= 0;
	first_val[0] 	= 0;
	first_val[1] 	= 0;
	second_val[0] 	= 0;
	second_val[1] 	= 0;
	difference[0] 	= 0;
	difference[1] 	= 0;
	activeSensor 	= 0;
	readInProgress  = 0;
	startRead		= 0;
}

void HAL_TIM17_CAPTURE(TIM_HandleTypeDef *htim17_p)
{
	if(!activeSensor){
		return;
	}
	// if(htim15_p->Channel == HAL_TIM_ACTIVE_CHANNEL_2){//if channel 2
	if (!IS_FIRST_CAPTURED[1])
	{ // if first val is not captured -> capture value
		first_val[1] = HAL_TIM_ReadCapturedValue(htim17_p, TIM_CHANNEL_1);
		IS_FIRST_CAPTURED[1] = 1;
		// __HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);/*change to falling edge detection for second value*/
	}
	else
	{ // if first val is captured -> capture second value
		second_val[1] = HAL_TIM_ReadCapturedValue(htim17_p, TIM_CHANNEL_1);
		// calculate difference
		if (second_val[1] > first_val[1])
			difference[1] = second_val[1] - first_val[1];
		else
			difference[1] = 0xFFFF - first_val[1] + second_val[1];

		// calculate distance
		// distance = difference *.034/2;//got this from internet might need to adjust
		if (difference[1] < 10000)
		{
			distance[1] = difference[1];
			frontDist = difference[1];
			freshData |= 1;
		}
		IS_FIRST_CAPTURED[1] = 0;
		readInProgress = 0;
		// reverse back to rising edge detection for first value
		//  __HAL_TIM_DISABLE_IT(&htim15, TIM_IT_CC1);
	}

	//}
}

// calculates data
void HAL_TIM15_CAPTURE(TIM_HandleTypeDef *htim15_p)
{
	if(activeSensor){
		return;
	}
	// if(htim15_p->Channel == HAL_TIM_ACTIVE_CHANNEL_2){//if channel 2
	if (!IS_FIRST_CAPTURED[0])
	{ // if first val is not captured -> capture value
		first_val[0] = HAL_TIM_ReadCapturedValue(htim15_p, TIM_CHANNEL_2);
		IS_FIRST_CAPTURED[0] = 1;
		// __HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);/*change to falling edge detection for second value*/
	}
	else
	{ // if first val is captured -> capture second value
		second_val[0] = HAL_TIM_ReadCapturedValue(htim15_p, TIM_CHANNEL_2);
		// calculate difference
		if (second_val[0] > first_val[0])
			difference[0] = second_val[0] - first_val[0];
		else
			difference[0] = 0xFFFF - first_val[0] + second_val[0];

		// calculate distance
		// distance = difference *.034/2;//got this from internet might need to adjust
		if (difference[0] < 10000)
		{
			distance[0] = difference[0];
			rightDist = difference[0];
			freshData |= 2;
		}
		IS_FIRST_CAPTURED[0] = 0;
		readInProgress = 0;
		// reverse back to rising edge detection for first value
		//  __HAL_TIM_DISABLE_IT(&htim15, TIM_IT_CC1);
	}

	//}
}

// captures data
void HCSR04_READ(GPIO_TypeDef *gpioPort, uint16_t gpioPin, TIM_HandleTypeDef *TimRef)
{
	readInProgress = 1;
	HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_SET); // set
	// HAL_GPIO_WritePin(gLED_GPIO_Port, gLED_Pin, GPIO_PIN_SET);//set high
	delay(10);											  // delay 10 us per documentation
	HAL_GPIO_WritePin(gpioPort, gpioPin, GPIO_PIN_RESET); // set low
	__HAL_TIM_SET_COUNTER(TimRef, 0);
	// HAL_GPIO_WritePin(gLED_GPIO_Port, gLED_Pin, GPIO_PIN_RESET);//set high
	//  __HAL_TIM_ENABLE_IT(TimRef, TIM_IT_);
}
// ------------------- Public functions ------------------------------

void TriggerUSA()
{
	// __HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
	HCSR04_READ(TriggerA_GPIO_Port, TriggerA_Pin, &htim15);
	activeSensor = 0;
}

void TriggerUSB()
{
	// __HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);
	HCSR04_READ(TriggerB_GPIO_Port, TriggerB_Pin, &htim17);
	activeSensor = 1;
}

uint8_t FreeUS()
{
	return (!readInProgress);
}

uint8_t ReadReadyUSA()
{
	// if the sensor is actively reading, and the active sensor is sensorA
	return (!readInProgress && !activeSensor);
}

uint8_t ReadReadyUSB()
{
	// if the sensor is actively reading, and the active sensor is sensorB
	return (!readInProgress && activeSensor);
}

uint8_t getValueUSA()
{
	return distance[0];
}

uint8_t getValueUSB()
{
	return distance[1];
}
