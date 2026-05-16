# CS211 Ultrasonic Motor Control Project

## Project Overview
This project implements an obstacle detection and motor control system using an Mbed-enabled microcontroller. The system uses an ultrasonic sensor to measure distance and controls DC motors via PWM signals. The motors are driven forward unless an obstacle is detected within a specific minimum distance threshold, in which case the motors are stopped.

**Video Demonstration:** [Google Drive Link](https://drive.google.com/drive/folders/1mUJnBdD9e2O7ln-maTfzr3B0_-KyYGPp)

## Code Structure
The application is entirely contained within `main.cpp` and relies on the Mbed OS API. It defines several modular functions to handle different tasks:

1. **Hardware Abstraction:**
   - `trig` (D6): Digital output to trigger the ultrasonic sensor.
   - `echo` (D7): Interrupt input to measure the pulse width returned by the sensor.
   - `in2`, `in4` (D4, D5): Forward pins (connected to In2 and In4 on the H-bridge for forward-only movement, no direction control).
   - `motorSpeed`, `motorSpeed1` (D3, D9): PWM outputs for motor speed control.

2. **Core Logic Functions:**
   - `ultrasonic_trigger()`: Sends a 10µs pulse to the sensor's trigger pin to start a measurement.
   - `should_move(float d)`: Evaluates whether the calculated distance allows for safe forward movement. It stops movement if the distance is below 30cm and resumes if above 55cm.
   - `motor_forward()`: Sets the direction pins and drives both PWM pins at 32.5% duty cycle.
   - `motor_stop()`: Sets the PWM duty cycles to 0 to halt the motors.

## Main Function
The `main()` function serves as the application's entry point:
- First, it attaches the rising and falling edge interrupts (`echo_rise` and `echo_fall`) to the echo pin.
- Then, it configures the PWM period to 1ms (1kHz frequency) for smooth motor operation.
- An infinite `while(true)` loop acts as the main scheduling block:
  - It triggers the ultrasonic sensor.
  - It decides whether the robot should move or stop based on the most recently calculated distance `distance0`.
  - It sleeps for 100ms using `ThisThread::sleep_for()` to allow other threads and ISRs to execute and to prevent sensor echoing overlap.

## Interrupt Service Routines (ISRs)
The distance measurement uses hardware interrupts to ensure precise timing without blocking the main CPU thread:
- **`echo_rise()`:** Triggered when the ultrasonic echo pin goes HIGH. It resets and starts a hardware `Timer`.
- **`echo_fall()`:** Triggered when the echo pin goes LOW. It stops the timer. The elapsed time represents the sound wave's round trip, which is immediately converted into centimeters (`distance0`) using the speed of sound formula: `(time * 0.0343) / 2`.

## Startup Code Explanation
Before the `main()` function is executed, the microcontroller relies on low-level **startup code** (typically written in Assembly and C, found in Mbed OS's target-specific directories). 
The startup code is responsible for:
1. **Vector Table Initialization:** Setting up the interrupt vector table in memory so the CPU knows exactly where to jump when hardware interrupts (like our `echo_rise` and `echo_fall`) occur.
2. **Reset Handler:** The very first code executed upon power-up.
3. **Memory Initialization:** Copying initialized data from Flash to RAM and zeroing out the uninitialized data section (BSS).
4. **System Clock Configuration:** Setting up the PLLs to ensure the CPU and peripheral buses run at the correct frequencies.
5. **Stack Pointer Setup:** Initializing the stack pointer for the main thread.
Once these low-level initializations are complete, the startup code branches into the C/C++ runtime and finally calls `main()`.