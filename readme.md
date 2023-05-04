# CS-5780 Final Project
 
 ## Team Name
 Team Crimson
 
 
## Purpose
The purpose of this project was to create a rover capable of traversing a maze without direct knowledge of the landscape/possible obstructions. We were also tasked with implementing at least 3 techniques from our labs this semester. We chose to use GPIO, timers, interrupts and USART(not used in final implementation).

## Functionality
Our rover has two ultra sonic sensors, one on the front and the other 90 degrees to the right of the first. The rover's autonomous movement is based on the position of obstructions/walls in front of the sensors. Depending on which sensor(s) detect an obstruction, the rover makes the decision to move in a direction which will allow the rover to keep its right side close to a wall/object. The rover also has the ability to back up and turn left in case it becomes stuck. If nothing is detected by the rover, it will turn 360 degrees, then move forward until an object is detected.

## Flow charts
![State Machine Example](/5780_stateDesign.PNG "State Machine Example")

## Build Instructions
### Generate Source
In order to generate the required source files, you must use [STMCube](https://www.st.com/en/development-tools/stm32cubemx.html) to open ```/MicroController/MicroController.ioc``` then select the "Generate Code" Button in the top left.
![CubeMX layout Example](/CubeMX.PNG "CubeMX layout Example")

### Build MicroController image
[STM32-for-VScode](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) is a required Extension for this repository, if you do not have it installed follow installation and configuration instructions [here](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode).

With [STM32-for-VScode](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) installed, select the stm extension on the left panel. Then first select the "build" option then "Flash STM32" or "Debug STM32" depending on if you wish to debug changes or simply run the robot.
![STM32-for-VScode](/STM32-for-vscode.PNG "STM32-for-VScode layout Example")

## Wiring Diagrams
The rover has a very simple wiring system, for the most part components are connected to a direct breakout of a STM32F075 processor. With the exception of the 2 DC motor to control the wheels, that are connected to the motor controller with the connections shown below. Connections to the micro controller can be seen below as well. 
![STM32-Connections](/ControllerConnections.PNG "STM32-wiring")

In addition to the controller, there are 2 ultrasonic sensor, 1 USART bluetooth unit, and a Isaac's Dual motor controller. Pin connections are listed bellow for clarification. 

![SensorA Wiring](/SensorAWire.PNG "Ultra sonic sensor A Wiring") 
![SensorB Wiring](/SensorBWire.PNG "Ultra sonic sensor B Wiring")
![Motor Controller Diagram](/MotorControllerWire.PNG "Motor Controller Wiring diagram")



## Optional Debug feature
Note: It is highly recommended to use the built in debugger built into the stm VSCode. For usage instructions for this method see [here](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode). 

### Reading serial Debug
For our optional additional debug option, general debug information will be sent over serial bluetooth to a connected device. To read this data, connect the robots bluetooth device, then read the serial data using your favorite serial monitor (for example [Serial Monitor](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vscode-serial-monitor)). 

### Adding Serial Debug statements
If additional information is need to properly debug the functionality of the rover, simply import ```charBuffer.h``` at the top of the relevant c File. Then use the command ```AppendCharBuffer(UsartWriteQueue,char[] msg,int msgLen);``` Always use ```UsartWriteQueue``` but then set msg to be some string you wish to send over serial and msgLen as the length of the message being sent. 


Team Members
+ Isaac Christensen
+ Sam Refahiyat
+ Carlos Carvajal
