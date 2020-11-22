### Example mixes
Note:
- If the servos are moving in wrong direction after mix, negating the applied weights or reversing the servo direction in Outputs screen corrects this.
- Unless otherwise stated, offset is 0, differential is 0, control switch is none. 

#### Vtail
Left servo in ch2, right servo in ch4
```txt
1. Ch2 =  50%Rud + -50%Ele
2. Ch4 = -50%Rud + -50%Ele
```

#### Elevon
Left servo in Ch1, right servo in Ch2
```txt
1. Ch1 = -50%Ail + -50%Ele
2. Ch2 =  50%Ail + -50%Ele
```

#### Aileron differential
Left aileron in Ch1, right aileron in Ch8
```txt
1. Ch1 = -100%Ail{-25%Diff}
2. Ch8 =  100%Ail{ 25%Diff}
```

#### Flaperon
Left aileron in Ch1, right aileron in Ch8. Using SwC to operate the flaperons. 
<br>When SwC is in upper position, flaperons are off. 
<br>In middle position we deploy half flaperons, and in lower position we deploy full flaperons.
```txt
1. Ch1 = -100%Ail{-25%Diff} + -50%SwC{-50offset}
2. Ch8 =  100%Ail{ 25%Diff} + -50%SwC{-50offset}
```
If desired, the flaperon deployment can be slowed with the 'Slow' function in Inputs screen, specifying 
SwC as the source. When this is done, it is neccessary to manually edit the mix to use SwC* instead.

#### Crow braking 
Left ail servo in Ch1, Right Ail servo in Ch8, left flap servo in Ch5, right flap servo in Ch6.
<br>Using the three position SwC for operation. 
<br>Normal aileron action occurs when SwC is in upper or middle position. 
<br>Half flaps are deployed when SwC is in middle position.
<br>When SwC is in lower position, both ailerons move upward and full flaps are deployed
thus crow braking.
```txt
1. Ch1 = -100%Ail{-25%Diff} + 50%SwC{100%Diff}
2. Ch8 =  100%Ail{ 25%Diff} + 50%SwC{100%Diff}
3. Ch5 = -50%SwC{-50offset}
4. Ch6 =  100%Ch5
```
If desired, the flap/crow deployment can be slowed with the 'Slow' function in Inputs screen, specifying 
SwC as the source. When this is done, it is neccessary to manually edit the mix to use SwC* instead.

#### Differential thrust
Left motor in Ch3, right motor in Ch7. 
<br>We can use a switch e.g SwD to turn the differential thrust on or off.
<br>For safety, we also need to specify Cut and Failsafe for both Ch3 and Ch7 in the 'Outputs' screen.
```txt
1. Ch3  = 100%Thrt + 40%Rud  [SwD_Down]  
2. Ch7  = 100%Thrt + -40%Rud [SwD_Down]
```

#### Elevator throttle mixing
When the throttle is increased, some down elevator can be added. 
```txt
1. Ch2 = -100%Ele + -20%thrt{-20 offset}
```

#### Variable Steering Depending on Throttle Position
Steering servo in Ch6.Suppose we want full steering when the throttle is closed, reducing to no steering at full throttle, we use a Multiply mix for this. 
```txt
1. Ch6 = 100%yaw * -50%thrt{50 offset}
```

#### Adjust throttle idle with knob
Suppose we have a gas model and want to adjust the idle setting of engine with knob without affecting full throttle.
Assuming the throttle servo is in Ch3,
```txt
1. Virt1 = -50%Thrt{50 offset} * 20%knob{20 offset}
2. Ch3 = 100%Thrt + 100%Virt1
```
