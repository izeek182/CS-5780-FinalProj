#include "main.h"
#include "tim.h"
#include "ultra_sonic.h"
#include <stdlib.h>

// ------------------- Private Variable ------------------------------
	uint8_t IS_FIRST_CAPTURED;
	uint8_t distance;//in cms
	uint32_t first_val;//first time stamp
	uint32_t second_val;//second time stamp
	uint32_t difference;//in microseconds
	TIM_HandleTypeDef htim15;


// ------------------- Private functions ------------------------------
void delay(uint16_t time){
	__HAL_TIM_SET_COUNTER(&htim15, 0);
	while(__HAL_TIM_GET_COUNTER(&htim15) < time){};
}

//initialize ultra sonic sensor
void ultra_init(){
	IS_FIRST_CAPTURED = 0;
	distance = 0;
	first_val = 0;
	second_val = 0;
	difference = 0;
}

//calculates data
void HAL_TIM_CAPTURE(TIM_HandleTypeDef* htim15_p){
	if(htim15_p->Channel == HAL_TIM_ACTIVE_CHANNEL_2){//if channel 2
		if(!IS_FIRST_CAPTURED){//if first val is not captured -> capture value
			first_val = HAL_TIM_ReadCapturedValue(htim15_p, TIM_CHANNEL_2);
			IS_FIRST_CAPTURED = 1;
			__HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);/*change to falling edge detection for second value*/
		}
		else if(IS_FIRST_CAPTURED){//if first val is captured -> capture second value
			second_val = HAL_TIM_ReadCapturedValue(htim15_p, TIM_CHANNEL_2);
			__HAL_TIM_SET_COUNTER(htim15_p,0);
			//calculate difference
			if(second_val > first_val)
				difference = second_val - first_val;
			else
				difference = 0xFFFF - first_val + second_val;

		//calculate distance
		distance = difference *.034/2;//got this from internet might need to adjust
		IS_FIRST_CAPTURED = 0;
		//reverse back to rising edge detection for first value
		__HAL_TIM_SET_CAPTUREPOLARITY(htim15_p, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
		__HAL_TIM_DISABLE_IT(&htim15, TIM_IT_CC1);

		}
		

	}
}

//captures data
void HCSR04_READ(){
	HAL_GPIO_WritePin(TriggerA_GPIO_Port, TriggerA_Pin, GPIO_PIN_SET);//set high
	delay(10);//delay 10 us per documentation
	HAL_GPIO_WritePin(TriggerA_GPIO_Port, TriggerA_Pin, GPIO_PIN_RESET);//set low
	__HAL_TIM_ENABLE_IT(&htim15, TIM_IT_CC1);
}

