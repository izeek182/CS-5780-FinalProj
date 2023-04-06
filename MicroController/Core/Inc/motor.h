#ifndef __CMD_BUFFER_1
#define __CMD_BUFFER_1

#ifdef __cplusplus
extern "C" {
#endif

// motor code initiation function call before starting tick timer or queuing move commands
void maneuver_init();


//  Enqueues a move Forward command at given power, and length of time
void MoveForward(int power, int ticks);
//  Enqueues a move Backward command at given power, and length of time
void MoveBackward(int power, int ticks);
//  Enqueues a turn Right command at given power, and length of time
void turnRight(int power, int ticks);
//  Enqueues a turn Left forward command at given power, and length of time
void turnLeft(int power, int ticks);
//  Enqueues a Idle command for given length of time
void motorIdle(int ticks);

// This method stops the motor, and empties the command queue completely (E-stop kinda)
void motor_STOP();

// Preforms motor related maintainence, (pin enable/disable, PWM config, advancing cmd queue)
void motor_tick();



#ifdef __cplusplus
}
#endif

#endif /* __CMD_BUFFER_1 */