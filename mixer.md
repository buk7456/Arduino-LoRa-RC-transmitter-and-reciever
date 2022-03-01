## Example mixes
Note:
- When tweaking with mixes especially changing mix inputs/outputs, it is advisable to first disable RF output or remove propellers as a safety precaution. 
- If the servos are moving in wrong direction after mix, negating the applied weights or reversing the servo direction in Outputs screen corrects this.
- Unless otherwise stated, offset is 0, differential is 0, control switch is none. 

### Throttle cut
Using Ch3 as throttle channel, and assigning SwA for cut. When SwA is in the Up position, Ch3 is locked to -100, otherwise the Throttle input is sent to Ch3
```txt
1. Ch3 = 100%Thrt ReplcWith -100%max [SwA_Up]
```
### Vtail
Left servo in Ch2, right servo in Ch4
```txt
1. Ch2 =  50%Rud + -50%Ele
2. Ch4 = -50%Rud + -50%Ele
```
### Elevon
Left servo in Ch1, right servo in Ch2
```txt
1. Ch1 = -50%Ail + -50%Ele
2. Ch2 =  50%Ail + -50%Ele
```
### Aileron differential
Left aileron in Ch1, right aileron in Ch8
```txt
1. Ch1 = -100%Ail{25%Diff}
2. Ch8 =  100%Ail{25%Diff}
```
### Flaperon
Left aileron in Ch1, right aileron in Ch8. Using SwC to operate the flaperons. 
<br>When SwC is in upper position, flaperons are off. 
<br>In middle position we deploy half flaperons, and in lower position we deploy full flaperons.
```txt
1. Ch1 = -100%Ail{25%Diff} + -50%SwC{-50offset}
2. Ch8 =  100%Ail{25%Diff} + -50%SwC{-50offset}
```
If desired, the flaperon deployment can be slowed with the 'Slow' function in Inputs screen, specifying 
SwC as the source. When this is done, it is necessary to manually edit the mix to use SwC* instead.

### Crow braking 
Left ail servo in Ch1, Right Ail servo in Ch8, left flap servo in Ch5, right flap servo in Ch6.
<br>Using the three position SwC for operation. 
<br>Normal aileron action occurs when SwC is in upper or middle position. 
<br>Half flaps are deployed when SwC is in middle position.
<br>When SwC is in lower position, both ailerons move upward and full flaps are deployed
thus crow braking.
```txt
1. Ch1 = -100%Ail{25%Diff} + 50%SwC{100%Diff}
2. Ch8 =  100%Ail{25%Diff} + 50%SwC{100%Diff}
3. Ch5 = -50%SwC{-50offset}
4. Ch6 =  100%Ch5
```
If desired, the flap/crow deployment can be slowed with the 'Slow' function in Inputs screen, specifying 
SwC as the source. When this is done, it is necessary to manually edit the mix to use SwC* instead.

### Differential thrust
Left motor in Ch3, right motor in Ch7. 
<br>We can use a switch e.g SwE to turn the differential thrust off while in the air.
```txt
1. Ch3  = 100%Thrt + 40%yaw  [SwE_Down]  
2. Ch7  = 100%Thrt + -40%yaw [SwE_Down]
``` 
We can also mix in throttle cut controlled with SwA for both channels as follows.
```txt
3. Ch3  = 100%Ch3 ReplcWith -100%max [SwA_Up]  
4. Ch7  = 100%Ch7 ReplcWith -100%max [SwA_Up]
```
For safety, we also need to set Failsafe for both Ch3 and Ch7 in the 'Outputs' screen.

### Elevator throttle mixing
When the throttle is increased, some down elevator can be added. 
```txt
1. Ch2 = -100%Ele + -20%thrt{-20 offset}
```
### Variable Steering Depending on Throttle Position
Steering servo in Ch6. Suppose we want full steering when the throttle is closed, reducing to no steering at full throttle, we use a Multiply mix for this. 
```txt
1. Ch6 = 100%yaw * -50%thrt{50 offset}
```
### Adjust maximum throttle with knob
We can adjust the maximum throttle without affecting the low throttle setting.
Assuming the motor is in Ch3,
```txt
1. Virt1 = 50%Thrt{50 offset} * 30%knob{-30 offset}
2. Ch3 = 100%Thrt + 100%Virt1
```
### Adjust throttle idle with knob
Suppose we have a gas model and want to adjust the idle setting of engine with knob without affecting full throttle.
Assuming the throttle servo is in Ch3,
```txt
1. Virt1 = -50%Thrt{50 offset} * 20%knob{20 offset}
2. Ch3 = 100%Thrt + 100%Virt1
```
### Servo tester
We can program a simple servo tester that repeatedly moves a servo back and forth.
In the inputs screen, navigate to Function generator page, then select desired movement type (sine, square, triangle, or sawtooth) and period, then go back to the Mixer screen.
Assuming the servo is connected to Ch9,
```txt
1. Ch9 = 100%fgen
```
