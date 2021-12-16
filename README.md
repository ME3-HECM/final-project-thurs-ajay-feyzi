# Course project - Mine navigation search and rescue



## Motor Calibrations

CalibrateTurns function is used to modify the global variable turning_time to achieve accurate turns
Procedure:
 -Lights flash to indicate start of calibration
 -The buggy then performs a 180 turn - User has 4 options
 -Car turns too much -> Hold Button RF2: Decrease turning_time
 -Car turns too little -> Hold Button RF3: Increase turning_time
 -Want to see again? -> Hold Button RF2 and RF3 simultaneously
 -Buttons must be held till respective LED flashes
 -Hold Button RF2 and RF3 until back lights flash to exit calibration
 -Lights flash to indicate end of calibration



CalibrateReverseSquare function is used to modify the global variable reverse_time to achieve accurate turns
Procedure:
 -Function to calibrate reverse_time to ensure accurate distance
 -Lights flash to indicate start of calibration
 -The buggy then performs a reverse_square move - User has 4 options
 -Car moves too much -> Hold Button RF2: Decrease turning_time
 -Car moves too little -> Hold Button RF3: Increase turning_time
 -Want to see again? -> Hold Button RF2 and RF3 simultaneously
 -Buttons must be held till respective LED flashes
 -Hold Button RF2 and RF3 until back lights flash to exit calibration
 -Lights flash to indicate start of calibration

## Memory Operations
The buggy relies on two arrays to return home when needed
 - time_list :  A list of TMR0 readings ( converted from bits to ms)
 - funcPtrList : A list of pointers to functions that take only left and right motor structs as input

When the buggy is told to return home it first performs a 180 turn
Then it loops through the two lists in memory, starting from the latest entry
- First it moves the buggy forward by the amount of time in the time_list
- Then it calls the function in the function list

### Complement Functions:
The complement function is the function that needs to be executed for the buggy, coming from the opposite direciton, needs to perform to undo the move.

These can intiutively be mapped as follows:
- Red and Green are inverse of each other
- Orange and Lightblue are inverse of eachother
- Blue is inverse with itself
- For the yellow and pink card reverse_yellow_move and reverse_pink_move functions
    - These first turn the buggy 90 degrees, then move forward one square

### Color Function
Each card has a move function assocaited with it in CardMoves.c (pink_move, red_move etc)
These functions are called when that color is read and the color value (1-9) is passed to pick_move()
Before that card function is called the complement is stored in the funciton pointer list


## Challenge brief

Develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## "Mine" environment specification

A "mine" is contstructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Colour | Instruction
---------|---------
Red | Turn Right 90
Green | Turn Left 90
Blue | Turn 180
Yellow | Reverse 1 square and turn right 90
Pink | Reverse 1 square and turn left 90
Orange | Turn Right 135
Light blue | Turn Left 135 
White | Finish (return home)
Black | Maze wall colour

Mine courses will vary in difficulty, with the simplest requiring 4 basic moves to navigate. More advanced courses may require 10 or moves to navigate. The mines may have features such as dead ends but colour cards will always direct you to the end of the maze. Once the end of the maze has been reached, you must return to the starting position. An example course to navigate is shown below. You do not know in advance which colours will be in the course or how many.

![Navi Diagram](gifs/maze.gif)
