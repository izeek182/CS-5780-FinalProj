#ifndef __ultra_sonic
#define __ultra_sonic

#ifdef __cplusplus
extern "C" {
#endif

// initialize ultra sonic sensor
void ultra_init();

//calculates data
void HAL_TIM_CAPTURE(TIM_HandleTypeDef* tim_han_p);


// Triggers a Read of ultra sonic sensor A
void TriggerUSA();
// Triggers a Read of ultra sonic sensor B
void TriggerUSB();
// Returns the true if the ultra sonic sensor is free. 
uint8_t FreeUS();
// Returns if the read process is complete for sensor A
uint8_t ReadReadyUSA();
// Returns if the read process is complete for sensor B
uint8_t ReadReadyUSB();
// Returns the Last Valid Result for Ultra Sonic sensor A
uint8_t getValueUSA();
// Returns the Last Valid Result for Ultra Sonic sensor B
uint8_t getValueUSB();



#ifdef __cplusplus
}
#endif

#endif /* __ultra_sonic */