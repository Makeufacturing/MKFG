// -----------------------------------------------------------
// Board: MKFG WorkHorse RP2350 - v1.2.x 
// (c) Makefast Workshop Ltd - Delaware, Ohio, USA - Sept 2025
// Quesytion? hello@makefastworkshop.com
// -----------------------------------------------------------
// To use this MKFG code, you need the following LIBRARIES: 
//  -  MKFG.h -> MKFGWorkHorse2350 -- This supports all of the functionality of the board, which includes:
//    -  TMCStepper @ v0.7.3+
//    -  AccelStepper @ v1.64+
//    -  RPI_PICO_TimerInterrupt @ v1.3.1+
//    -  Adafruit_NeoPixel @ v1.15.1+
//    -  Adafruit_SSD1306 @ 2.5.15+ (and dependencies)
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
#include "GravityMixer.hpp" // Gravity Mixer (variables, helpers, utilities, etc.)
// -----------------------------------------------------------
void setup() {
  mkfg.setup(); // DO THIS FIRST! Initialize MKFG board (pin configuration, motor chips, etc.)
  // --
  mkfg.stepper_initDriver(*mkfg.stepperA_tmc, 800, 0x48, 0x24);
  mkfg.stepper_initDriver(*mkfg.stepperB_tmc, 800, 0x48, 0x24);
  mkfg.stepper_initAccelStepperRunner(500, 8000/128, 500, 8000/128); // maxSpeed <= 1000 step/sec (=5rev/sec for 200 step motor). maxAccel <= 16000
}
// -----------------------------------------------------------
// Interaction Handlers (do things quickly and return; i.e. do not block with long functions directly)
// --
void mkfg_onBtn1Changed(bool isPressedNow){
  Serial.printf("Btn1: %d\n",isPressedNow);
  if(isPressedNow){
    gm_selectAndStartProgramNow(GM_PROG_FIXEDSPEED);
  }else{
  }
}
void mkfg_onBtn2Changed(bool isPressedNow){
  Serial.printf("Btn2: %d\n",isPressedNow);
  if(isPressedNow){
    gm_selectAndStartProgramNow(GM_PROG_TAPERDOWN_N_MIN);
  }else{
  }
}
void mkfg_onBtn3Changed(bool isPressedNow){
  Serial.printf("Btn3: %d\n",isPressedNow);
  if(isPressedNow){
    gm_selectAndStartProgramNow(GM_PROG_SINEWAVE_N_MIN);
  }else{
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
  if(!isPressed){
    // knob turned (not pressed) --> ADJUST VELOCITY MULTIPLIER
    gm_velocityMultiplier = fmaxf(GM_MUL_MIN, fminf(1.00, gm_velocityMultiplier + (knobValChange > 0 ? GM_MUL_STEP : -GM_MUL_STEP)));
    gm_displayNeedsUpdated = true;
    Serial.printf("Vel Mul: %f\n", gm_velocityMultiplier);
  }else{
    // knob turned while pressed (i.e. press-and-turn) --> ADJUST PROGRAM TIME OFFSET
    unsigned long newStartTimeMs = gm_progStartMillis + (knobValChange > 0 ? -30000 : 30000);
    if((millis() - newStartTimeMs) < 24*60*60*1000){ // less than 1 day... not a wrap.
      gm_progStartMillis = newStartTimeMs;
    }
    gm_displayNeedsUpdated = true; // redraw the graph.
  }
}
void mkfg_onEncoderClick(uint8_t numClicks){
  Serial.printf("EncoderBtn: Click x %d\n",numClicks);
  switch(numClicks){
    case 1: mkfg.requestBeep(MKFG_BEEP_1X); gm_isPausedByUser = !gm_isPausedByUser; gm_displayNeedsUpdated = true; break;
    case 2: mkfg.requestBeep(MKFG_BEEP_2X); gm_isReversedByUser = !gm_isReversedByUser; gm_displayNeedsUpdated = true; break;
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
  static uint32_t last_updateTime = 0;
  static uint32_t updateCount = 0;
  static bool steppersHaveWarning = false;
  // --
  mkfg.loop(); // DO THIS EVERY LOOP -- handle user interface updates.
  // --
  if(mkfg.encoderIsHeldDownLong){
    digitalWrite(PIN_MOT_A_1, HIGH);
    // digitalWrite(PIN_MOT_A_PWM, (micros()%25000) < 12500 ? HIGH : LOW); 
    pwm_set_gpio_level(PIN_MOT_A_PWM, 1023/4); 
  }else{
    pwm_set_gpio_level(PIN_MOT_A_PWM, 0);
    digitalWrite(PIN_MOT_A_1, LOW);
  }
  // --
  uint32_t ms = millis();
  unsigned long progMillis = ms - gm_progStartMillis;
  // --
  if ((ms - last_updateTime) >= 20) {  //run every 0.02s
    last_updateTime = ms;
    updateCount++;
    if(updateCount%750 == 0) gm_pushCurVelocityToGraphAndRequestRedraw(); // push data every 15 seconds.
    if(updateCount%250 == 1){
      steppersHaveWarning = mkfg.steppers_checkStatusReportWarnings(); // check steppers every 5 seconds.
    }
    // --
    gm_velocityTargetRPS = gm_getVelocityTargetAtProgMsOffset(progMillis);
    bool velTargetIsNeg = gm_velocityTargetRPS < 0;
    gm_velocityTargetRPS = fabs(gm_velocityTargetRPS);
    float velocityGoalRPS = gm_velocityTargetRPS * gm_velocityMultiplier * gm_velocityMultiplier;
    // -- Run at constant speed by setting MaxSpeed and moving to a far position.
    mkfg.stepperA_as->setMaxSpeed(fmaxf(0.1, velocityGoalRPS * GM_STEPPERA_STEPS_PER_REV * MKFG_STEPPER_MICROSTEPS)); // speed must always be > 0.
    mkfg.stepperB_as->setMaxSpeed(fmaxf(0.1, velocityGoalRPS * GM_STEPPERB_STEPS_PER_REV * MKFG_STEPPER_MICROSTEPS)); // speed must always be > 0.
    // --
    mkfg.stepperA_as->moveTo(mkfg.stepperA_as->currentPosition()+(gm_isPausedByUser?0:(gm_isReversedByUser?-1000000:1000000))*(velTargetIsNeg?1:-1));
    mkfg.stepperB_as->moveTo(mkfg.stepperB_as->currentPosition()+(gm_isPausedByUser?0:(gm_isReversedByUser?-1000000:1000000))*(velTargetIsNeg?1:-1));
    // --
    // // Check for stalling?
    if(false){
      if(updateCount%2 == 0){
        uint32_t stallGuardTorqueHeadroom = mkfg.stepperA_tmc->SG_RESULT();
        if(!gm_isPausedByUser && velocityGoalRPS > 0.35 && stallGuardTorqueHeadroom > 0 && stallGuardTorqueHeadroom < 32){
          Serial.printf("Stalling? HeadroomA = %d, velocityGoalRPS = %f\n", stallGuardTorqueHeadroom, velocityGoalRPS);
        }
      }
      if(updateCount%2 == 1){
        uint32_t stallGuardTorqueHeadroom = mkfg.stepperB_tmc->SG_RESULT();
        if(!gm_isPausedByUser && velocityGoalRPS > 0.35 && stallGuardTorqueHeadroom > 0 && stallGuardTorqueHeadroom < 32){
          Serial.printf("Stalling? HeadroomB = %d, velocityGoalRPS = %f\n", stallGuardTorqueHeadroom, velocityGoalRPS);
        }
      }
    }
    gm_drawDisplayIfNeeded();
  }
  // --
  if(steppersHaveWarning) mkfg.rgbw_warning();
}
