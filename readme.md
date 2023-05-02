# CS-5780 Final Project


## Build Instructions

### Generate Source
In order to generate the required source files, you must use [STMCube](https://www.st.com/en/development-tools/stm32cubemx.html) to open ```/MicroController/MicroController.ioc``` then select the "Generate Code" Button in the top left.
![CubeMX layout Example](/CubeMx.PNG "CubeMX layout Example")

### Build MicroController image
[STM32-for-VScode](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) is a required Extension for this repository, if you do not have it installed follow installation and configuration instructions [here](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode).

With [STM32-for-VScode](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) installed, select the stm extention on the left panel. Then first select the "build" option then "Flash STM32" or "Debug STM32" depending on if you wish to debug changes or simply run the robot.
![STM32-for-VScode](/STM32-for-vscode.PNG "STM32-for-VScode layout Example")

## Wiring Diagrams




## Optional Debug feature
Note: It is highly recommended to use the built in debugger built into the stm VSCode. For usage instructions for this method see [here](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode). 

For our optional additional debug option, general debug information will be sent over serial bluetooth to a connected device. To read this data, connect the robots bluetooth device, then read the serial data using your favorite serial monitor (for example [Serial Monitor](https://marketplace.visualstudio.com/items?itemName=ms-vscode.vscode-serial-monitor)). 


Team Members
+ Isaac Christensen
+ Sam Refahiyat
+ Carlos Carvajal
