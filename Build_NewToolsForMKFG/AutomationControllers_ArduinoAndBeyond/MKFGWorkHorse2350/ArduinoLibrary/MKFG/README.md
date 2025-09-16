# Makeufacturing (MKFG) Arduino Helper Library

This is the helper library for all the MKFG boards.

Examples can be found in the [examples directory](./examples/), these can also be loaded from the examples menu in the Arduino IDE.



## Installation

This library can be installed through the Arduino library manager or manually from github by following [the instructions here](https://docs.arduino.cc/software/ide-v1/tutorials/installing-libraries).



## Usage

In your Arduino code, **do these 4 things:**

1. Include this library & create an instance variable of it. 

   - `#include <MKFG.h>`

   - `MKFGWorkHorse2350 mkfg = MKFGWorkHorse2350();`

2. Add `mkfg.setup()` in the first line of the Arduino setup() function.

3. Add `mkfg.loop()` in the Arduino loop() function.

4. Include function implementations for all of the user interaction event handlers:

```cpp
// -----------------------------------------------------
// User Interaction function prototypes TO BE DEFINED IN USER CODE:
// -- Do things quickly and return; i.e. do not block with long functions directly.
void mkfg_onBtn1Changed(bool isPressedNow);
void mkfg_onBtn2Changed(bool isPressedNow);
void mkfg_onBtn3Changed(bool isPressedNow);
void mkfg_onEncoderBtnChanged(bool isPressedNow);
void mkfg_onEncoderTurned(int knobValChange, bool isPressed);
// -- click handlers (slight delay, but allows for multi-click and long press interactions)
void mkfg_onEncoderClick(uint8_t numClicks);
void mkfg_onEncoderPressAndHold();
// -----------------------------------------------------
```

See the [examples directory](./examples/) for functional code and usage demos.



## List of functions

```c++

// Initializes all MKFG board peripherals.
// Should be called once at the start of your code's setup() function.
void mkfg.setup();

// Called as quickly as possible in your program's main() loop.
// This updates the various user interface components and calls handlers accordingly.
void mkfg.loop();

```
