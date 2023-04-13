#ifndef __ultra_sonic
#define __ultra_sonic

#ifdef __cplusplus
extern "C" {
#endif

// initialize ultra sonic sensor
void ultra_init();

//calculates data
void HAL_TIM_CAPTURE(TIM_HandleTypeDef* tim_han_p);

//reads in data to be captured
void HCSR04_READ();


#ifdef __cplusplus
}
#endif

#endif /* __ultra_sonic */