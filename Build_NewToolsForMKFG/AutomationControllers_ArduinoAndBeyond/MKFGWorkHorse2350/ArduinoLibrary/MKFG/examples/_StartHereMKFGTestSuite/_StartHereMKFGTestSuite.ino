// -----------------------------------------------------------
// Board: MKFG WorkHorse RP2350 - v1.2.x 
// (c) Makefast Workshop Ltd - Delaware, Ohio, USA - Sept 2025
// Quesytion? hello@makefastworkshop.com
// -----------------------------------------------------------
// To use this MKFG code, you need the following LIBRARIES: 
//  -  TMCStepper @ v0.7.3+
//  -  AccelStepper @ v1.64+
//  -  RPI_PICO_TimerInterrupt @ v1.3.1+
//  -  Adafruit_NeoPixel @ v1.15.1+
//  -  Adafruit_SSD1306 @ 2.5.15+ (and dependencies)
// -----------------------------------------------------------
// To upload, use Arduino Boards Manager --> Raspberry Pi Pico/RP2040/RP2350 @ v4.5.1+).
// Then use the following SETTINGS (under Tools at the top):
//  -  Board: Generic RP2350
//  -  CPU Architecture: ARM
//  -  Flash Size 16MB (no FS)
//  -  CPU Speed: 150MHz
//  -  Optimize: Small
//  -  Profiling: Disabled
//  -  PSRAM Size: 0MByte PSRAM
//  -  RTTI: Disabled
//  -  Stack Protector: Disabled
//  -  Upload Method: Default (UF2)
//  -  USB Stack: Pico SDK
//  -  Chip Variant: RP2350A
// -----------------------------------------------------------
// Important notes for RP2350 in Arduino! :)
// * USB Serial is set to 1000000 baud. Use this in Arduino to see messages in the Serial Monitor!
// * Serial ==> USB, Serial1 ==> UART0, Serial2 ==> UART1, etc.)
// * Most GPIO are hooked up to hardware! But some are still available for custom use:
//   - GPIO16 / SPI0_RX
//   - GPIO17 / SPI0_CSn
//   - GPIO18 / SPI0_SCK / SERVO 1 (included on servo port)
//   - GPIO19 / SPI0_TX  / SERVO 2 (included on servo port)
//   - - - - - - - - -
//   - GPIO26 / ADC0
//   - GPIO27 / ADC1
//   - GPIO28 / ADC2
//   - GPIO29 / ADC3
// -----------------------------------------------------------
#include <MKFG.h> // See the MKFG.h file for all PIN definions and function prototypes to use in your code.
MKFGWorkHorse2350 mkfg = MKFGWorkHorse2350();
// --
#include "LUTs.hpp" // Look-up Tables (for waveform generation if needed)
// -----------------------------------------------------------
void setup() {
  mkfg.setup(); // DO THIS FIRST! Initialize MKFG board (pin configuration, motor chips, etc.)

  // -- SERVOS --
  // * Servos are NOT initializd by default, but you can initialize/use them via:
  //   - mkfg.servo1.attach(PIN_SERVO_1, 1000, 2000, 90); // Minimum and maximum pulse width (in µs) to go from 0° to 180°.
  //   - mkfg.servo2.attach(PIN_SERVO_2, 1000, 2000, 90); // Minimum and maximum pulse width (in µs) to go from 0° to 180°.
  //   - mkfg.servo1.write(45);  // move servo1 to  45 degrees.
  //   - mkfg.servo2.write(135); // move servo2 to 135 degrees.
  mkfg.servo1.attach(PIN_SERVO_1, 1000, 2000, 90); // Minimum and maximum pulse width (in µs) to go from 0° to 180°.
  mkfg.servo2.attach(PIN_SERVO_2, 1000, 2000, 90); // Minimum and maximum pulse width (in µs) to go from 0° to 180°.

  // -- STEPPER MOTORS --
  // * Steppers are initialized to be freewheeling/low-power at boot, until you fully initialize them:
  //   - mkfg.stepper_initDriver(*mkfg.stepperA_tmc, <STEPPER_A_MAX_CURRENT_MA>); 
  //   - mkfg.stepper_initDriver(*mkfg.stepperB_tmc, <STEPPER_B_MAX_CURRENT_MA>);
  //   - mkfg.steppers_runAutoCaliberation();  // just use this to get (PWM_OFS_AUTO,PWM_GRAD_AUTO) values.
  //   - mkfg.stepper_initAccelStepperRunner(<maxSpeed_A>,<maxAccel_A>,<maxSpeed_B>,<maxAccel_B>);
  // * You can also supply PWM_OFS/PWM_GRAD to mkfg_stepper_initDriver() if you know them via mkfg_steppers_runAutoCaliberation();
  // mkfg.stepper_initDriver(*mkfg.stepperA_tmc, 1000); //, 0x82, 0x30);
  // mkfg.stepper_initDriver(*mkfg.stepperB_tmc, 1000); //, 0x82, 0x30);
  // mkfg.steppers_runAutoCaliberation();
  mkfg.stepper_initDriver(*mkfg.stepperA_tmc, 800, 0x48, 0x24);
  mkfg.stepper_initDriver(*mkfg.stepperB_tmc, 800, 0x48, 0x24);
  mkfg.stepper_initAccelStepperRunner(1000, 8000/16, 1000, 8000/16); // maxSpeed <= 1000 step/sec (=5rev/sec for 200 step motor). maxAccel <= 16000
}
// -----------------------------------------------------------
// Interaction Handlers (do things quickly and return; i.e. do not block with long functions directly)
// --
void mkfg_onBtn1Changed(bool isPressedNow){ // STEPPER A & B TEST
  Serial.printf("Btn1: %d\n",isPressedNow); 
  if(isPressedNow){
    mkfg.stepperA_as->moveTo(10*200*MKFG_STEPPER_MICROSTEPS);
    mkfg.stepperB_as->moveTo(10*200*MKFG_STEPPER_MICROSTEPS);
    // mkfg.stepperA_tmc->VACTUAL((RPSToStepsMul * 50) / 100);
    // mkfg.stepperB_tmc->VACTUAL((RPSToStepsMul * 50) / 100);
  }else{
    mkfg.stepperA_as->moveTo(0);
    mkfg.stepperB_as->moveTo(0);
    // mkfg.stepperA_tmc->VACTUAL(0);
    // mkfg.stepperB_tmc->VACTUAL(0);
  }
}
void mkfg_onBtn2Changed(bool isPressedNow){ // MOTOR B PWM TEST; MOTOR A creates a VIBE test in loop() below
  Serial.printf("Btn2: %d\n",isPressedNow);
  if(isPressedNow){
    // digitalWrite(PIN_MOT_A_1, HIGH);
    // pwm_set_gpio_level(PIN_MOT_A_PWM, 512);
    digitalWrite(PIN_MOT_B_1, HIGH);
    pwm_set_gpio_level(PIN_MOT_B_PWM, 512);
  }else{
    // pwm_set_gpio_level(PIN_MOT_A_PWM, 0);
    // digitalWrite(PIN_MOT_A_1, LOW);
    pwm_set_gpio_level(PIN_MOT_B_PWM, 0);
    digitalWrite(PIN_MOT_B_1, LOW);
  }
}
void mkfg_onBtn3Changed(bool isPressedNow){ // SERVOS
  Serial.printf("Btn3: %d\n",isPressedNow);
  if(isPressedNow){
    mkfg.servo1.write(0);
    mkfg.servo2.write(180);
  }else{
    mkfg.servo1.write(90);
    mkfg.servo2.write(90);
  }
}
void mkfg_onEncoderBtnChanged(bool isPressedNow){
  Serial.printf("EncoderBtn: %d\n",isPressedNow);
  if(isPressedNow){
  }else{
  }
}
void mkfg_onEncoderTurned(int knobValChange, bool isPressed){
  Serial.printf("EncoderKnob: %d, isPressed: %d\n", knobValChange, isPressed);
}
void mkfg_onEncoderClick(uint8_t numClicks){
  Serial.printf("EncoderBtn: Click x %d\n",numClicks);
  switch(numClicks){
    case 1: mkfg.requestBeep(MKFG_BEEP_1X); break;
    case 2: mkfg.requestBeep(MKFG_BEEP_2X); break;
    case 3: mkfg.requestBeep(MKFG_BEEP_3X); break;
    case 4: mkfg.requestBeep(MKFG_BEEP_4X); break;
    case 5: mkfg.requestBeep(MKFG_BEEP_5X); break;
    default: {
      // no click handler for other clicks...
    }
  }
}
void mkfg_onEncoderPressAndHold(){
  Serial.println("EncoderBtn: Press & Hold");
  mkfg.requestBeep(MKFG_BEEP_LONG);
}
// -----------------------------------------------------------
void loop() {
  mkfg.loop(); // DO THIS EVERY LOOP -- handle user interface updates.
  // --
  if(mkfg.btn2IsDown){ // MOTOR A creates a VIBE test when btn2 is down.
    digitalWrite(PIN_MOT_A_1, HIGH);
    // use lookup table with factors of 2 to get a nice sinewave at the desired low frequency for bass vibe motor test.
    pwm_set_gpio_level(PIN_MOT_A_PWM, lookupTable_sine1024[(micros()/16)%1024]); 
  }else{
    pwm_set_gpio_level(PIN_MOT_A_PWM, 0);
    digitalWrite(PIN_MOT_A_1, LOW);
  }
}
