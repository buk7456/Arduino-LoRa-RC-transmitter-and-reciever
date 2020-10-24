# Arduino Lora based Radio Controller

An Arduino powered long range radio controller for use with rc planes, cars, boats, etc.

<p align="center">
<img src="img1.jpg" width="712" height="648"/>
</p>

## Features
- 8 proportional channels with Reverse, Endpoints, Subtrim, Cut, Failsafe
- 2 digital toggle channels
- Dualrate & expo for Ail, Ele, Rud
- Throttle curve and a custom curve with 5 points
- Safety (cut) switch
- 5 model memory
- User definable mixes with 10 mixer slots per model
- Preconfigured mixer templates for Elevon, Vtail, Flaperon, Crow braking, Differential thrust
- 2 timers; Throttle timer and a free stopwatch
- Sticks calibration
- Audio tones
- Receiver binding
- Frequency hopping 

## License
MIT license

## Hardware
#### Transmitter
- 2x Atmega328p microcontrollers
- 1x Semtech SX1276/77/78/79 based rf module 
- 128x64 KS0108 based LCD, or any 128x64 lcd (provide own driver code).
- 3x push buttons
- Additional support components

#### Receiver
- 1x Atmega328p mcu
- 1x Semtech SX1276/77/78/79 based rf module
- Additional support components

The schematics can be found in the 'etc' folder under root directory

## Compiling
The code compiles on Arduino IDE 1.8.x or higher with board set to Arduino Uno. 
The transmitter code is in mtx (master mcu) and stx (slave mcu) folders. The receiver mcu code is in 
the rx folder. No external libraries are required to compile.

## User Interface
Three buttons are used for navigation; Up, Select, Down. Long press Select to go Back. 
<br>Hold the Down key on home screen to access the extra digital channels A & B).
<br>Hold the Select key on home screen to access the trims.
<br>Hold the Select key while powering on to access the hidden boot menu.

<p align="center">
<img src="img2.png" width="816" height="864"/>
</p>

## Binding
To bind the receiver and transmitter, we use the bind option in the system settings. 
Select bind option in transmitter and simultaneously restart the receiver.

## Mixing
This controller implements a free mixer that offers flexiblity with what we want to control. 
Each mixer slot takes two inputs, multiplexes them, and sends the result to the specified output. 
Mixer slots are evaluated sequentially. 
Available multiplex options are Add, Multiply, Replace. We can also assign a switch to turn the mix on or off.
#### Mix sources
- Raw stick inputs (roll, pitch, throttle, yaw, knob)
- Switches (SwA, SwB, SwC, SwD)
- Curves (Aileron, Elevator, Throttle, Rudder, Custom)
- Channels (Ch1 to Ch8)
- Temporary variables (Virt1, Virt2)
 
#### [Example mixes](mixer.md)

## Testing
I have done several tests on this system and found it reliable enough. 
The range I got with the SX1278 modules at the settings I used was more than 1.5km Line of sight in a semi urban area. 
I have also tested the frequency hopping feature and it is stable enough. 
The transmitter to receiver update rate averages at around 40 frames a second which is sufficient to control an RC model. 
There are no issues experienced with the servo control either. 

## Limitations
1. No basic telemetry support. 
2. No dedicated trim buttons. However we can access trim from the home screen by holding the select key.
