# EMP Final project

## Requirements
- 20 floors (excluding 13th) ✅
    - Main floor - floor 0 ✅
    - Elevator start - floor 2 ✅

<br>

- Call elevator to main with long press (switch 1) ✅
    - LCD will display current floor level after activation ✅
    - Doors open at floor 0 ✅

<br>

- 4-digit code must be entered (divisible by 8) ✅
    - On correct password, choose destinated floor with rotary encoder (press to confirm)✅

<br>

- LED functions ✅
    - Green LED - Steady ON: The elevator is stopped and the doors are open at a floor ✅
    - Yellow LED - Blinking: The elevator is moving and currently in the acceleration phase (The first 50% of the total moving time) ✅
    - Red LED - Blinking: The elevator is moving and currently in the deceleration phase (The last 50% of the total moving time) ✅

<br>

- Quicks
  - Elevator breaks on every 4th trip (indicated by all LEDs flashing at the same time)✅
  - Restart the elevator by setting the potentiometer to a random value displayed on the screen✅
    - The rotary encoder must also be turned 360° from it's starting position (Direction changes every time)✅
    - If the rotary encoder is turned the wrong way an error message will be displayed✅
    - Press encoder to confirm✅

- Extra
  - UART logging
    - Log each trip of the elevator✅
  - PC control 
    - Request logs
    - Control acceleration with PC✅
  - Blinking frequency should increase and decrease with the acceleration✅
  
---

## Module implementation
- LCD :white_check_mark:
  - Setup :white_check_mark:
  - Task structure :white_check_mark:
<br>

- Matrix keypad :white_check_mark:
  - Setup :white_check_mark:
  - Task structure :white_check_mark:
  - Display on LCD :white_check_mark:
  - Bounce prevention :white_check_mark:
<br>

- UART (Ready for implementation)
  - Setup :white_check_mark:
  - Task structure :white_check_mark:
  - Logging 
<br>  

- Rotary encoder (Working on: Simon)
  - Setup
  - Task structure
<br>

- Elevator control (Working on: Karl)
  - Setup
  - Task structure
<br>  

- Potentiometer (Working on: Daniel)
  - Setup
  - Task struture

