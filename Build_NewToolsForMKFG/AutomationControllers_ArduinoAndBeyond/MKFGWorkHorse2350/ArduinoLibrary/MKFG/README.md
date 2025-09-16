# Makeufacturing (MKFG) Arduino Helper Library

This is the helper library for all the MKFG boards.

Examples can be found in the [examples directory](./examples/), these can also be loaded from the examples menu in the Arduino IDE.

## Installation

This library can be installed through the Arduino library manager or manually from github by following [the instructions here](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries).

## List of functions

```c++

// Initializes all MKFG board peripherals.
// Should be called once at the start of your code's setup() function.
void mkfg_setup();

// Called as quickly as possible in your program's main() loop.
// This updates the various user interface components and calls handlers accordingly.
void mkfg_loop();

```
