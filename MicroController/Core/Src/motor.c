#include "motor.h"
#include "main.h"
#include "tim.h"
#include <stdlib.h>

// ------------------- Private Types ------------------------------
//  Handy little enum for moving commands
enum maneuverType{
    idle = 0,
    move_forward,
    move_backward,
    turn_right,
    turn_left
};

enum direction{
    e_break = 0,
    forward,
    reverse,
};

// motor command struct linked list command queue structure
struct CMDNodeStruct{
    int maneuverLength;
    int power; // 0 - 99
    enum maneuverType maneuverType;
    struct CMDNodeStruct* nextNode;
};
typedef struct CMDNodeStruct cmdNode;

// Command linked queue structure
typedef struct
{
    int size;
    cmdNode* CurrentNode;
    cmdNode* LastNode;   
} cmdBuffer_t;

// ------------------- Private Variable ------------------------------
cmdBuffer_t         motorCmdBuf;
enum maneuverType   CurrentState;
int                 remainingTimeOnCmd;
int16_t             trim;


// ------------------- Private functions ------------------------------
void enqueue(enum maneuverType,int power, int length);
void runNextCmd();
void setMode(enum maneuverType);
void setPower(int power);
// set motor A power 0-100
void motorA_PWM(int);
// set motor B power 0-100
void motorB_PWM(int);

// set motor A dir 0 forward, 1 backward
void motorA_dir(enum direction);
// set motor B dir 0 forward, 1 backward
void motorB_dir(enum direction);

uint8_t motorActive(){
    return (CurrentState != idle);
}

// ------------------- function implementations ------------------------------

void maneuver_init(){
    motorCmdBuf.size        = 0;
    motorCmdBuf.CurrentNode = 0;
    motorCmdBuf.LastNode    = 0;
    CurrentState            = idle;
    remainingTimeOnCmd      = 0;
    trim                    = 0;
    motorA_PWM(0);
    motorB_PWM(0);

    HAL_TIM_PWM_Start(&motor1PWM, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&motor2PWM, TIM_CHANNEL_1);
}

void MoveForward(int power, int ticks){
    enqueue(move_forward,power,ticks);
    motor_tick();
}
void MoveBackward(int power, int ticks){
    enqueue(move_backward,power,ticks);
    motor_tick();
}
void turnRight(int power, int ticks){
    enqueue(turn_right,power,ticks);
    motor_tick();
}
void turnLeft(int power, int ticks){
    enqueue(turn_left,power,ticks);
    motor_tick();
}
void motorIdle(int ticks){
    enqueue(idle,0,ticks);
    motor_tick();
}
uint16_t maxTrim = 20;
void trimMotorsRight(int16_t trimChange){
    trim -= trimChange;
    if(trim > maxTrim){
        trim = maxTrim;
    }else if(trim <-maxTrim){
        trim = -maxTrim;
    }
}
void trimMotorsLeft(int16_t trimChange){
    trim += trimChange;
    if(trim > maxTrim){
        trim = maxTrim;
    }else if(trim <-maxTrim){
        trim = -maxTrim;
    }
}


void enqueue(enum maneuverType mType,int power, int length){
    cmdNode* newCmd = (cmdNode*)malloc(sizeof(cmdNode));
    newCmd->maneuverLength  = length;
    newCmd->maneuverType    = mType;
    newCmd->power           = power;
    newCmd->nextNode        = 0;
    __disable_irq();
    if(motorCmdBuf.size == 0){
        motorCmdBuf.CurrentNode = newCmd;
        motorCmdBuf.LastNode = newCmd;
        motorCmdBuf.size = 1;
        setMode(newCmd->maneuverType);
        setPower(newCmd->power);
        remainingTimeOnCmd = motorCmdBuf.CurrentNode->maneuverLength;
        __enable_irq();
        return;
    }
    motorCmdBuf.size++;
    motorCmdBuf.LastNode->nextNode  = newCmd;
    motorCmdBuf.LastNode            = newCmd;
    __enable_irq();
}

void setMode(enum maneuverType maneuver){
    if(CurrentState == maneuver){
        return;
    }
    CurrentState = maneuver;
    switch (maneuver)
    {
    case move_forward:
        motorA_dir(forward);
        motorB_dir(forward);
        break;
    case move_backward:
        motorA_dir(reverse);
        motorB_dir(reverse);
        break;
    case turn_left:
        motorA_dir(forward);
        motorB_dir(reverse);
        break;
    case turn_right:
        motorA_dir(reverse);
        motorB_dir(forward);
        break;
    case idle:
    default:
        break;
    }
}
void setPower(int power){
    motorA_PWM(power + trim/2);
    motorB_PWM(power - trim/2);
}

void runNextCmd(){
    cmdNode* complete = motorCmdBuf.CurrentNode;
    motorCmdBuf.CurrentNode = complete->nextNode;
    motorCmdBuf.size--;
    free(complete);
    if (motorCmdBuf.size == 0)
    {
        motorCmdBuf.CurrentNode = 0;
        motorCmdBuf.LastNode    = 0;
        setMode(idle);
        motorA_PWM(0);
        motorB_PWM(0);
        return;
    }
    setMode(motorCmdBuf.CurrentNode->maneuverType);
    setPower(motorCmdBuf.CurrentNode->power);
    remainingTimeOnCmd = motorCmdBuf.CurrentNode->maneuverLength;
}

void motor_tick(){
    if(remainingTimeOnCmd > 0){
        remainingTimeOnCmd--;
        return;
    }
    // Command time remaining <= 0 
    if(motorCmdBuf.size > 0){
        runNextCmd();
        return;
    }

    // Command is complete, and no new commands in queue
    setMode(idle);
    motorA_PWM(0);
    motorB_PWM(0);
}

void SetPWMLevel(TIM_HandleTypeDef* timDef,int percent){
    long total = timDef->Instance->ARR<<16;   // Expand Range for precision (no floats But basically making a fixed point)
    long step = total/100;
    long finalVal = (percent * step)>>16; // Removed the precision bits
    timDef->Instance->CCR1 = finalVal;
}

void motorA_PWM(int power){
    SetPWMLevel(&motor1PWM,power);
}

void motorB_PWM(int power){
    SetPWMLevel(&motor2PWM,power);
}

void motorA_dir(enum direction dir){
    switch(dir){
        case(forward):
            HAL_GPIO_WritePin(Motor1A_GPIO_Port, Motor1A_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor1B_GPIO_Port, Motor1B_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(rLED_GPIO_Port, rLED_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(oLED_GPIO_Port, oLED_Pin,GPIO_PIN_RESET);
        break;
        case(reverse):
            HAL_GPIO_WritePin(Motor1A_GPIO_Port, Motor1A_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor1B_GPIO_Port, Motor1B_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(rLED_GPIO_Port, rLED_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(oLED_GPIO_Port, oLED_Pin,GPIO_PIN_SET);
        break;
        case(e_break):
            HAL_GPIO_WritePin(Motor1A_GPIO_Port, Motor1A_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor1B_GPIO_Port, Motor1B_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(rLED_GPIO_Port, rLED_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(oLED_GPIO_Port, oLED_Pin,GPIO_PIN_RESET);
        break;
    }
}

void motorB_dir(enum direction dir){
        switch(dir){
        case(forward):
            HAL_GPIO_WritePin(Motor2A_GPIO_Port, Motor2A_Pin,GPIO_PIN_SET);
            HAL_GPIO_WritePin(Motor2B_GPIO_Port, Motor2B_Pin,GPIO_PIN_RESET);
        break;
        case(reverse):
            HAL_GPIO_WritePin(Motor2A_GPIO_Port, Motor2A_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor2B_GPIO_Port, Motor2B_Pin,GPIO_PIN_SET);
        break;
        case(e_break):
            HAL_GPIO_WritePin(Motor2A_GPIO_Port, Motor2A_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(Motor2B_GPIO_Port, Motor2B_Pin,GPIO_PIN_RESET);
        break;
    }
}


