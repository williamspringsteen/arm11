ldr r0, =0x20200000 ;)KEEP r0 UNCHANGED, this holds 'base', add offsets to this to get to different registers


mov r2, #1
lsl r2, #12

mov r3, #1
lsl r3, #15

mov r4, #1
lsl r4, #24

mov r5, #0
add r5, r5, r2
add r5, r5, r3
add r5, r5, r4

str r5, [r0, #4] ;)Set up pins 14, 15 and 18 to be outputs [Pin 15 is the green light, other 5 are red]


mov r2, #1
lsl r2, #9

mov r3, #1
lsl r3, #12

mov r4, #1
lsl r4, #15

mov r5, #0
add r5, r5, r2
add r5, r5, r3
add r5, r5, r4

str r5, [r0, #8] ;)Set up pins 23, 24 and 25 to be outputs

mov r2, #1
lsl r2, #6

mov r3, #1
lsl r3, #9

mov r4, #1
lsl r4, #12

mov r5, #0
add r5, r5, r2
add r5, r5, r3
add r5, r5, r4

str r5, [r0] ;)Set up pins 2, 3 and 4 to be outputs [For the score counter]


ldr r6, =0xFFFFFFFC    ;)~3, could also subract 3 from FFFFFFFF
ldr r7, [r0, #148]     ;)148 is offset to PULL UP/DOWN ENABLE
and r7, r7, r6         ;)And these contents we put in r7 with the negation of 3
orr r7, r7, #1         ;)Or with 1 [1 is pull down], so last 2 bits go to 01
str r7, [r0, #148]     ;)Put this back in the PULL UP/DOWN ENABLE register

ldr r2, =0x200         ;)Waits while it does [0x200]^2 decrements by 1
wait_1_8:          
cmp r2, #0
blt end_wait_1_8
ldr r3, =0x200
wait_2_8:
cmp r3, #0
blt end_wait_2_8
sub r3, r3, #1
b wait_2_8
end_wait_2_8:
sub r2, r2, #1
b wait_1_8
end_wait_1_8:

mov r1, #1
lsl r1, #17
str r1, [r0, #152]     ;)Set bit 17 in PULL UP/DOWN ENABLE CLOCK 0 to 1

ldr r2, =0x200         ;)Waits while it does [0x200]^2 decrements by 1
wait_1_9:          
cmp r2, #0
blt end_wait_1_9
ldr r3, =0x200
wait_2_9:
cmp r3, #0
blt end_wait_2_9
sub r3, r3, #1
b wait_2_9
end_wait_2_9:
sub r2, r2, #1
b wait_1_9
end_wait_1_9:

ldr r6, =0xFFFFFFFC    ;)~3, could also subract 3 from FFFFFFFF
ldr r7, [r0, #148]     ;)148 is offset to PULL UP/DOWN ENABLE
and r7, r7, r6         ;)And these contents we put in r7 with the negation of 3
str r7, [r0, #148]     ;)Resets last 2 bits of PULL UP/DOWN ENABLE register

mov r1, #0
str r1, [r0, #152]     ;)Reset PULL UP/DOWN ENABLE CLOCK 0 to 0

ldr r6, =0xFF1FFFFF    ;)0xFF1FFFFF = ~[7<<21], mask to preserve all bits except pin 17 bits
ldr r7, [r0, #4]       ;)Control bits for 10 - 19
and r7, r7, r6         ;)Only sets pin 17 control bits to 0 [INPUT]
str r7, [r0, #4]       ;)Store this back in function select registers

;)Pin 17 is not set up as pull down input, so a 1 in bit 17 of pin level register means input is detected

;)DO NOT TOUCH r8 OR r9 [Remember r0 is still out of bounds too]

mov r8, #0 ;)Initial score [Can go up to 7]
ldr r9, =0x500 ;)Initial difficulty is 0x500 [Quite slow], decrease by 0x50 = 80 in decimal

main_loop:    ;)Loop for main game

;)PIN 15 WILL HAVE GREEN LIGHT CONNECTED TO IT!!!!!!!!!!!!!!!!!

mov r1, #1
lsl r1, #14
str r1, [r0, #40]

mov r1, #1
lsl r1, #15
str r1, [r0, #40]  

mov r1, #1
lsl r1, #18
str r1, [r0, #40]  

mov r1, #1
lsl r1, #23
str r1, [r0, #40]  

mov r1, #1
lsl r1, #24
str r1, [r0, #40]  

mov r1, #1         
lsl r1, #25
str r1, [r0, #40]        ;)CLEAR ALL REGISTERS AT START OF MAIN GAME LOOP [Need to clear all as this is where we will loop back to after button presed]

mov r1, #1
lsl r1, #14
str r1, [r0, #28]        ;)Turn pin 14 on  
mov r10, #14             ;)r10 holds value of pin that is on - SO DO NOT TOUCH r10 UNLESS TURNING NEXT LIGHT ON

mov r2, r9           ;)Wait while [0x250]^2 si decremented to 0 [Nested for loops]
wait_1_2:          
cmp r2, #0
blt end_wait_1_2
mov r3, r9           ;)Can't touch r2 or r3 inside one of these nested for loops
wait_2_2:

ldr r11, [r0, #52]  ;)Offset to pin level register
mov r1, #1
lsl r1, #17
and r11, r11, r1    ;)r11 now holds all 0s, except possibly in bit 17, which is the same as in the pin level register
cmp r11, #0         ;)If the result is non-zero [1 in bit 17], we can jump to the piece of code that handles pausing 
bne pressed         ;)the current light, updating score, and changing difficulty

cmp r3, #0
blt end_wait_2_2
sub r3, r3, #1
b wait_2_2
end_wait_2_2:
sub r2, r2, #1
b wait_1_2
end_wait_1_2:

mov r1, #1         
lsl r1, #14
str r1, [r0, #40]    ;)Turn pin 14 off

mov r1, #1
lsl r1, #15
str r1, [r0, #28]    ;)Turn pin 15 on
mov r10, #15             ;)r10 holds value of pin that is on

;)REPEAT ABOVE CODE WITH DIFFERENT LIGHTS BEING TURNED ON AND OFF

mov r2, r9      
wait_1_3:          
cmp r2, #0
blt end_wait_1_3
mov r3, r9
wait_2_3:
ldr r11, [r0, #52]  ;)From here
mov r1, #1
lsl r1, #17
and r11, r11, r1
cmp r11, #0
bne pressed       ;)To here
cmp r3, #0
blt end_wait_2_3
sub r3, r3, #1
b wait_2_3
end_wait_2_3:
sub r2, r2, #1
b wait_1_3
end_wait_1_3:

mov r1, #1         
lsl r1, #15
str r1, [r0, #40]  

mov r1, #1
lsl r1, #18
str r1, [r0, #28] 
mov r10, #18             ;)r10 holds value of pin that is on

mov r2, r9    
wait_1_4: 
cmp r2, #0
blt end_wait_1_4
mov r3, r9
wait_2_4:
ldr r11, [r0, #52]  ;)From here
mov r1, #1
lsl r1, #17
and r11, r11, r1
cmp r11, #0
bne pressed       ;)To here
cmp r3, #0
blt end_wait_2_4
sub r3, r3, #1
b wait_2_4
end_wait_2_4:
sub r2, r2, #1
b wait_1_4
end_wait_1_4:

mov r1, #1    
lsl r1, #18
str r1, [r0, #40] 

mov r1, #1
lsl r1, #23
str r1, [r0, #28] 
mov r10, #23             ;)r10 holds value of pin that is on

mov r2, r9    
wait_1_5:     
cmp r2, #0
blt end_wait_1_5
mov r3, r9
wait_2_5:
ldr r11, [r0, #52]  ;)From here
mov r1, #1
lsl r1, #17
and r11, r11, r1
cmp r11, #0
bne pressed       ;)To here
cmp r3, #0
blt end_wait_2_5
sub r3, r3, #1
b wait_2_5
end_wait_2_5:
sub r2, r2, #1
b wait_1_5
end_wait_1_5:

mov r1, #1  
lsl r1, #23
str r1, [r0, #40]

mov r1, #1
lsl r1, #24
str r1, [r0, #28]
mov r10, #24             ;)r10 holds value of pin that is on

mov r2, r9  
wait_1_6:    
cmp r2, #0
blt end_wait_1_6
mov r3, r9
wait_2_6:
ldr r11, [r0, #52]  ;)From here
mov r1, #1
lsl r1, #17
and r11, r11, r1
cmp r11, #0
bne pressed       ;)To here
cmp r3, #0
blt end_wait_2_6
sub r3, r3, #1
b wait_2_6
end_wait_2_6:
sub r2, r2, #1
b wait_1_6
end_wait_1_6:

mov r1, #1       
lsl r1, #24
str r1, [r0, #40] 

mov r1, #1
lsl r1, #25
str r1, [r0, #28] 
mov r10, #25             ;)r10 holds value of pin that is on

mov r2, r9      
wait_1_1:          
cmp r2, #0
blt end_wait_1_1
mov r3, r9
wait_2_1:
ldr r11, [r0, #52]  ;)From here
mov r1, #1
lsl r1, #17
and r11, r11, r1
cmp r11, #0
bne pressed       ;)To here
cmp r3, #0
blt end_wait_2_1
sub r3, r3, #1
b wait_2_1
end_wait_2_1:
sub r2, r2, #1
b wait_1_1
end_wait_1_1:

b main_loop

pressed:  ;)START OF CODE WHEN BUTTON PRESSED - Waits [Leaves current light on], updates score [Has to check what light is lit], changes difficulty

;)If correct light is lit [r10 holds value of pin that is on], increase score [r8] by 1 and increase
;)difficulty [decrease r9 by 0x50 = 80], otherwise reset score and difficulty
;)If score is 8, quit and go to victory code [FLASHING SCORE COUNTER?]
;)Otherwise, update score counter, either by masking each of first 3 bits of score number, or by saying if[0] {do this}, if[1] {do that}, etc.

cmp r10, #15
beq correct    ;)Will increment score and increase difficulty
mov r8, #0     ;)0 is initial score
ldr r9, =0x500 ;)0x500 is initial difficulty
end_correct:

cmp r8, #8
beq victory

mov r1, #1
and r2, r8, r1
cmp r2, #0
beq turn_off_first  ;)Will turn first light of score counter off
mov r1, #1
lsl r1, #2
str r1, [r0, #28]        ;)Will turn on first light [Pin 2]
end_turn_off_first: 

mov r1, #1
lsl r1, #1
and r2, r8, r1
cmp r2, #0
beq turn_off_second  ;)Will turn second light of score counter off
mov r1, #1
lsl r1, #3
str r1, [r0, #28]        ;)Will turn on second light [Pin 3]
end_turn_off_second: 

mov r1, #1
lsl r1, #2
and r2, r8, r1
cmp r2, #0
beq turn_off_third  ;)Will turn third light of score counter off
mov r1, #1
lsl r1, #4
str r1, [r0, #28]        ;)Will turn on third light [Pin 4]
end_turn_off_third: 

ldr r2, =0x1000     ;)LONGER WAIT THAN IN MAIN LOOP
wait_1_7:          
cmp r2, #0
blt end_wait_1_7
ldr r3, =0x1000
wait_2_7:
cmp r3, #0
blt end_wait_2_7
sub r3, r3, #1
b wait_2_7
end_wait_2_7:
sub r2, r2, #1
b wait_1_7
end_wait_1_7:

b main_loop

correct:

add r8, r8, #1      ;)Increment score by 1
sub r9, r9, #137    ;)Decrementing by this number [in decimal] means after 5 decrements we will be at 0x250 [ish], which was fairly hard

b end_correct

turn_off_first:
mov r1, #1
lsl r1, #2
str r1, [r0, #40]
b end_turn_off_first

turn_off_second:
mov r1, #1
lsl r1, #3
str r1, [r0, #40]
b end_turn_off_second

turn_off_third:
mov r1, #1
lsl r1, #4
str r1, [r0, #40]
b end_turn_off_third

victory:         ;)Flashes all lights

mov r1, #1
lsl r1, #2
str r1, [r0, #28]

mov r1, #1
lsl r1, #3
str r1, [r0, #28]

mov r1, #1
lsl r1, #4
str r1, [r0, #28]

mov r1, #1
lsl r1, #14
str r1, [r0, #28]

mov r1, #1
lsl r1, #15
str r1, [r0, #28]

mov r1, #1
lsl r1, #18
str r1, [r0, #28]

mov r1, #1
lsl r1, #23
str r1, [r0, #28]

mov r1, #1
lsl r1, #24
str r1, [r0, #28]

mov r1, #1
lsl r1, #25
str r1, [r0, #28]

ldr r2, =0x300     
wait_1_10:          
cmp r2, #0
blt end_wait_1_10
ldr r3, =0x300
wait_2_10:
cmp r3, #0
blt end_wait_2_10
sub r3, r3, #1
b wait_2_10
end_wait_2_10:
sub r2, r2, #1
b wait_1_10
end_wait_1_10:

mov r1, #1
lsl r1, #2
str r1, [r0, #40]

mov r1, #1
lsl r1, #3
str r1, [r0, #40]

mov r1, #1
lsl r1, #4
str r1, [r0, #40]

mov r1, #1
lsl r1, #14
str r1, [r0, #40]

mov r1, #1
lsl r1, #15
str r1, [r0, #40]

mov r1, #1
lsl r1, #18
str r1, [r0, #40]

mov r1, #1
lsl r1, #23
str r1, [r0, #40]

mov r1, #1
lsl r1, #24
str r1, [r0, #40]

mov r1, #1
lsl r1, #25
str r1, [r0, #40]

ldr r2, =0x300
wait_1_11:          
cmp r2, #0
blt end_wait_1_11
ldr r3, =0x300
wait_2_11:
cmp r3, #0
blt end_wait_2_11
sub r3, r3, #1
b wait_2_11
end_wait_2_11:
sub r2, r2, #1
b wait_1_11
end_wait_1_11:

b victory
