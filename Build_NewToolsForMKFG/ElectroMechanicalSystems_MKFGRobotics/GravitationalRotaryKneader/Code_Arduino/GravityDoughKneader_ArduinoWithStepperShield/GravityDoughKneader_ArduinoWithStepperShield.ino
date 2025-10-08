// -----------------------------------------------------------
// This is a simplified version of the stepper motor controller
// for MKFG Gravitational Rotary Kneader that can run on any basic
// Arduino board with a basic steper motor shield/chip. When the
// board is powered on, it just runs the program. To restart,
// the program, just hit the reset button on the board.
// -----------------------------------------------------------
// PARAMETERS TO ADJUST
#define GM_STEPPER_STEPS_PER_REV 200       // Number of steps/revolution for the stepper motor used (200 is common)
#define GM_SPEED_RPS_MAX 3.0               // Max/peak stepper speed in routine (revs/second)
#define GM_TAPER_UP_SECONDS (1.50 * 60)    // 1.5 minutes taper up
#define GM_TAPERDOWN_SECONDS (13.50 * 60)  // 13.5 minutes taper down
#define GM_SPEED_END_MULTIPLIER 0.15       // How fast the kneader runs when done (~15% is usually good)
// -----------------------------------------------------------
// Board: Adafruit Feather M0 Express (but this should work with pretty much any standard Arduino board)
// Shield: Adafruit Stepper FeatherWing (but should be simple to modify for other stepper shields/drivers as well).
// (c) Makefast Workshop Ltd - Delaware, Ohio, USA - Oct 2025
// Questions? hello@makefastworkshop.com
// -----------------------------------------------------------
// To use this code, you need the following BOARD (via Board Manager):
// - See: https://learn.adafruit.com/adafruit-feather-m0-express-designed-for-circuit-python-circuitpython/arduino-ide-setup
// - Adafruit SAMD Boards
// -----------------------------------------------------------
// To use this code, you need the following LIBRARIES:
//  -  Adafruit Motor Shield V2 @ v1.1.3+ (and its dependencies)
// -----------------------------------------------------------
// To upload, use Arduino Boards Manager --> Adafruit SAMD Boards --> Adafruit Feather M0 Express
// -----------------------------------------------------------
#include <Adafruit_MotorShield.h>
// --
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_StepperMotor *stepperA = AFMS.getStepper(GM_STEPPER_STEPS_PER_REV, 1);  // (steps, stepper#)
Adafruit_StepperMotor *stepperB = AFMS.getStepper(GM_STEPPER_STEPS_PER_REV, 2);  // (steps, stepper#)
// -----------------------------------------------------------
unsigned long millisStart = 0;
unsigned long millisSpeedCurvePeak = 0;
unsigned long millisSpeedCurveEnd = 0;
int speedMaxX1000 = GM_SPEED_RPS_MAX * GM_STEPPER_STEPS_PER_REV * 1000;
int speedEndX1000 = GM_SPEED_RPS_MAX * GM_STEPPER_STEPS_PER_REV * GM_SPEED_END_MULTIPLIER * 1000;
void setup() {
  delay(2500);  // let power settle 2000ms on boot.
  // --
  AFMS.begin();
  // --
  millisSpeedCurvePeak = GM_TAPER_UP_SECONDS * 1000;
  millisSpeedCurveEnd = (GM_TAPER_UP_SECONDS + GM_TAPERDOWN_SECONDS) * 1000;
  // --
  Serial.printf("%lu \n", 0);
  millisStart = millis();
}
// -----------------------------------------------------------
unsigned long millisLastSerialPrint = 0;
void loop() {
  unsigned long millisIntoSpeedCurve = millis() - millisStart;
  // --
  int speedX1000 = 0;
  int pct = 0;
  if (millisIntoSpeedCurve < millisSpeedCurvePeak) {
    // Speed is ramping up...
    pct = (millisIntoSpeedCurve * 100) / millisSpeedCurvePeak;
    speedX1000 = pct * speedMaxX1000 / 100 + 1000;  // min speed is 1 step/second;
  } else if (millisIntoSpeedCurve < millisSpeedCurveEnd) {
    // Speed is tapering down...
    pct = (millisIntoSpeedCurve - millisSpeedCurvePeak) * 100 / (millisSpeedCurveEnd - millisSpeedCurvePeak);
    speedX1000 = (pct * speedEndX1000 + (100 - pct) * speedMaxX1000) / 100 + 1000;  // min speed is 1 step/second;
  } else {
    // Speed is at END... (continue slowly)
    speedX1000 = speedEndX1000;
  }
  unsigned long microsToDelay = 1000000000 / speedX1000;
  delayMicroseconds(microsToDelay);
  // --
  stepperA->onestep(FORWARD, SINGLE);
  stepperB->onestep(FORWARD, SINGLE);
  // --
  if (millis() - millisLastSerialPrint > 1000) {
    Serial.printf("%lu \n", 1000000 / microsToDelay);  // steps/second, pct
    millisLastSerialPrint = millis();
  }
}
