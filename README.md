# Arduino Lora based RC system
Control just about anything RC

## Features
- 9 RC channels. All channels are transmitted with 10 bit resolution
- Configurable RC channel output signal. Servo PWM, Digital on-off, or 'normal' PWM
- Reverse, Subtrim, Endpoints, Failsafe
- Dual rates and expo for Ail, Ele, Rud
- Throttle curve
- Flexible mixer system
- Adaptable timer and a stopwatch
- Model memory. Create, delete, copy, rename, and reset a model
- Sticks calibration
- Alarms, warnings
- Adjustable RF power
- Receiver binding
- Frequency hopping
- External voltage telemetry
- Transmitter to receiver update rate up to 40x

## Hardware
<p align="center">
<img src="img1.jpg" width="785" height="560"/>
</p>

#### Transmitter
- 2x Atmega328p microcontrollers
- 1x Semtech SX1276/77/78/79 based RF module 
- 128x64 KS0108 based LCD, or any 128x64 LCD (provide own driver code).
- 2x Joysticks, 5x two position switches, 1x three position switch, 1x potentiometer
- 3x push buttons
- Additional support components

#### Receiver
- 1x Atmega328p microcontroller
- 1x Semtech SX1276/77/78/79 based RF module
- Additional support components

The schematics can be found in the 'etc' folder under root directory

## Compiling
The code compiles on Arduino IDE 1.8.x or higher with board set to Arduino Uno. 
<br>The transmitter code is in mtx (master mcu) and stx (slave mcu) folders. The receiver mcu code is in 
the rx folder. No external libraries are required to compile.
<br>I am using the 433MHz band with the SX1278 modules. If using other modules or frequency band, it is 
necessary to edit the frequency lists in the stx.ino and rx.ino files. 

## User Interface
- Three buttons are used for navigation; Up, Select, Down. Long press Select to go Back. 
- Holding the Up key on home screen accesses the trims.
- Holding the Select key when booting opens the start-up menu (for stick recalibration, format eeprom, etc.)

<p align="center">
<img src="img2.png" width="828" height="1036"/>
</p>

## Binding to a receiver 
To bind the receiver and transmitter, use the bind option in the system setup screen. 
Select bind option and restart the receiver. The LED in receiver blinks on successful bind.

## Mixing
This controller implements a free mixer that offers flexibility with what we want to control. 
Each mixer slot takes two inputs, multiplexes them, and sends the result to the specified output. 
Available multiplex options are Add, Multiply, Replace. We can also assign a switch to turn the mix on or off.
Mixer slots are evaluated sequentially. 
<br>
<br> Mixer sources can be any of the following
- Raw stick inputs (roll, pitch, thrt, yaw, knob)
- Constants (max)
- Switches (SwA, SwB, SwC, SwD, SwE, SwF)
- Slowed input (appears with an asterisk)
- Curves (Ail, Ele, Thrt, Rud)
- Channels (Ch1 to Ch9)
- Temporary variables (Virt1, Virt2)

The default mapping is Ail to Ch1, Ele to Ch2, Thrt to Ch3, Rud to Ch4, unless overridden in the mixer.

##### [Example mixes](mixer.md)

## Configuring RC channel outputs
The receiver outputs can be configured from the transmitter to any of the three signal types; servo PWM, digital on-off, or 'normal' PWM.
For instance 'normal' PWM output could be used to control brushed DC motors without complex electronics. Also directly controlling 
components such as electromechanical relays and lights is made simple with the digital on-off output.
<br>Note: 
1. These settings are stored in the receiver, never in the transmitter.
2. 'Normal' PWM is only supported on a few select pins, depending on the receiver pin mapping. 
3. For safety, any changes made will only be effected upon rebooting the receiver.
4. In digital on-off mode, output value range of -100 to -50 maps to LOW, -49 to 49 is ignored, 50 to 100 maps to HIGH.

