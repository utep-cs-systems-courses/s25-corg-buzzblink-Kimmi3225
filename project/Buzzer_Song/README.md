# Blinky-Buzzy Toy 

 The toy plays a 4-tone melody 20 times while the green LED blinks with each tone. It also supports interaction through button presses that allow pausing, resuming, and stopping the melody.

## Description

- The toy plays a 4-tone melody repeatedly (20 times), getting slightly softer each round.
- The **green LED** blinks with each tone.
- The **red LED** is used to indicate an interrupt or pause.

## Button Controls
P2.0: **Hold** to play a white noise tone (interrupt); resumes on release   
P2.1: **Tap** to pause the melody; red light stays on                        
P2.2: **Tap** to resume the melody after a pause                             
P2.3: **Tap** to stop the melody completely and shut down the toy            

## How to Use

The Makefile in this direcory contains rules to run the Makefile in the project directory. Use **make** in this directory to build the project program and the timer library. Once the program is built, you can load the program onto the MSP430 by changing into the Buzzer_Song directory and using **make load**.

You may use **mspdebug rf2500 "erase"** to clear a program from the MSP430.
