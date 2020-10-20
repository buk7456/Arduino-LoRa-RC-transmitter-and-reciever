# Arduino Lora based Radio Controller

An Arduino powered long range radio controller for use with rc planes, cars, boats, etc.

<p align="center">
<img src="img1.jpg" width="712" height="648"/>
</p>

## Features:
- 8 proportional channels with Reverse, Endpoints, Subtrim, Cut, Failsafe
- 2 digital toggle channels
- 5 model memory
- Dualrate & expo for Ail, Ele, Rud
- Throttle curve and Custom curve with 5 points
- Safety (cut) switch
- 10 free mixer slots per model
- Mixer templates; Elevon, Vtail, Flaperon, Crow braking, Differential thrust
- 2 timers; Throttle timer and free stopwatch
- Sticks calibration
- Audio tones
- Bind, Frequency hopping

## License:
MIT license

## Hardware:
#### Transmitter
- 2x Atmega328p microcontrollers
- 1x Semtech SX1276/77/78/79 based rf module 
- 128x64 KS0108 based LCD, or any 128x64 lcd (provide own driver code). See the schematics
- 3x push buttons
- Additional support components. See the schematics

#### Receiver
- 1x Atmega328p mcu
- 1x Semtech SX1276/77/78/79 based rf module
- Additional support components

## Firmware:
The code compiles on Arduino IDE 1.8.x or higher with board set to Arduino Uno. 
The transmitter code is in mtx (master mcu) and stx (slave mcu) folders. The receiver mcu code is in 
the rx folder. No external libraries are required to compile.

### User Interface
Three buttons are used for navigation; Up, Select, Down. Long press Select to go Back. 
<br>Hold the Down key on home screen to access the extra digital channels A & B).
<br>Hold the Select key on home screen to access the trims.
<br>Hold the Select key while powering on to access the hidden boot menu.

<p align="center">
<img src="img2.png" width="816" height="864"/>
</p>

### Binding
To bind the receiver and transmitter, we use the bind option in the system settings. 
Select bind option in transmitter and immediately restart the receiver.

### The Mixer
This controller implements a free mixer that offers flexiblity with what we want to control. 
Each mixer slot takes two inputs, adds or multiplies them, then sends the result to the specified output. 
Mixer slots are evaluated sequentially.

##### Mix sources:
- Raw stick inputs (roll, pitch, throttle, yaw, knob)
- Switches (SwA, SwB, SwC, SwD)
- Curves (Aileron, Elevator, Throttle, Rudder, Custom)
- Channels (Ch1 to Ch8)
- Temporary variables (Virt1, Virt2)

#### Example mixes
Note:
1. Default mapping is Ail->Ch1, Ele->Ch2, Thrt->Ch3, Rud->Ch4, unless overridden in mixer.
2. In case after mix, the servos are moving in wrong direction, it can be solved by negating 
  the applied weights, or reversing the servo direction in Outputs screen. 

Unless otherwise specified, offset is 0, differential is 0

##### Vtail
Left servo in ch2, right servo in ch4
1. Ch2 =  50%Rud + -50%Ele
2. Ch4 = -50%Rud + -50%Ele

##### Elevon
Left servo in Ch1, right servo in Ch2
1. Ch1 = -50%Ail + -50%Ele
2. Ch2 =  50%Ail + -50%Ele

##### Aileron differential
Left aileron in Ch1, right aileron in Ch8
1. Ch1 = -100%Ail{-25%Diff}
2. Ch8 =  100%Ail{ 25%Diff}

##### Crow braking
We would like to setup crow braking on our plane. 
Left ail servo in Ch1, Right Ail servo in Ch8,
left flap servo in Ch5, right flap servo in Ch6.
Using the three position SwC to control the mix. 
Normal aileron action occurs when SwC is in upper or middle position. Half flaps are
deployed when SwC is in middle position.
When SwC is in lower position, both ailerons move upward and full flaps are deployed
thus providing crow braking feature.
1. Ch1 = -100%Ail{-25%Diff} + 50%SwC{100%Diff}
2. Ch8 =  100%Ail{ 25%Diff} + 50%SwC{100%Diff}
3. Ch5 = -50%SwC{-50offset}
4. Ch6 =  100%Ch5

##### Flaperon
Left aileron in Ch1, right aileron in Ch8. Let's use SwC to activate the flaperons. 
When SwC is in upper position, flaperons are off. In middle position we deploy half flaperons, 
and in lower position we deploy full flaperons.
1. Ch1 = -100%Ail{-25%Diff} + -50%SwC{-50offset}
2. Ch8 =  100%Ail{25%Diff} + -50%SwC{-50offset}

##### Differential thrust (twin motor)
Left motor in Ch3, right motor in Ch7. 
We can use a switch e.g SwD to turn the mix on or off. 
1. Virt1 = 40%Rud * 100%SwD{100%Diff}
1. Ch3  = 100%Thrt + 100%Virt1
2. Ch7  = 100%Thrt + -100%Virt1

##### Elevator throttle mixing
When the throttle is increased, some down elevator can be added. 
1. Ch2 = -100%Ele + -20%thr{-20 offset}

##### Variable Steering Depending on Throttle Position
This reduces the sensitivity of nosewheel steering as the throttle setting increases.
Suppose we want a steering rate of 100% when the throttle is closed, 
reducing to zero rate (no nosewheel steering) at full throttle. 
What is required is a Multiply mix added to the nosewheel servo channel.
1. Ch6 = 100%yaw * -50%thr{50 offset}

##### Trim with the knob
Example: We would like to trim our planes elevator with the knob while in flight
1. Ch2 = -100%Ele + -30%knob
Note: This doesnt change the subtrim so we have to remove this mix later
and adjust the subtrim when not in flight. 

## Testing:
I have done several tests on this system and found it reliable enough. 
The range I got with the SX1278 modules at the settings I used was more than 1.5km Line of sight. 
I have also tested the frequency hopping feature and it is stable enough. 
The transmitter to receiver update rate averages at around 40 frames a second which is sufficient to control an RC model. 
There are no issues experienced with the servo control either. 

## Limitations:
1. No basic telemetry support. 
2. No dedicated trim buttons. However we can access trim from the home screen by holding the select key.
