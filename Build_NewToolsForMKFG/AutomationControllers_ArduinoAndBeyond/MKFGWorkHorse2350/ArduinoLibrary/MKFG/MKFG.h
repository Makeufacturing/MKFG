#ifndef MKFG_h
#define MKFG_h
#include <Arduino.h>
// -----------------------------------------------------
// This library makes initializing & running the MKFG board simple. DO THESE 4 THINGS:
//  1. Include this library & create an instance variable of it. 
//     > #include <MKFG.h> // See the MKFG.h file for all PIN definions and function prototypes to use in your code.
//     > MKFGWorkHorse2350 mkfg = MKFGWorkHorse2350();
//  2. Add mkfg.setup() in the first line of the Arduino setup() function.
//  3. Add mkfg.loop() in the Arduino loop() function.
//  4. Include function implementations for all of the user interaction event handlers:
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
#include <hardware/gpio.h>
#include <hardware/pwm.h>
#include <hardware/clocks.h>
#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TMCStepper.h>
#include <AccelStepper.h>
#include "RPi_Pico_TimerInterrupt.h"
// -----------------------------------------------------
#define MKFG_ERRORS_BEEP false
// See blinky orange lights on boot? Here are the ERROR CODES:
// -- 2 blinks: Stepper A driver uninitialized (likely 12V power is not connected; connect it and reboot).
// -- 3 blinks: Stepper B driver uninitialized (likely 12V power is not connected; connect it and reboot).
// -----------------------------------------------------
// Pin Definitions
#define PIN_STEPPER_TX 0  // TMC2209 uses a single UART serial port (UART0 TX/RX = Serial1 in Arduino)
#define PIN_STEPPER_RX 1
// --
#define PIN_I2C0_SDA 4  // I2C port (SDA/SCL) for display + any other I2C peripherals needed (via Qwiic connector).
#define PIN_I2C0_SCL 5  // Both SDA and SCL have 4.7k pullups already included.
// --
#define PIN_BTN_1 2
#define PIN_BTN_2 3
#define PIN_BTN_3 6
#define PIN_ENCODER_A 7
#define PIN_ENCODER_B 8
#define PIN_ENCODER_BTN 9
// --
#define PIN_SPKR_PIEZO 10  // Piezo speaker, via 1k resistor. Play simple tones via PWM. Nicer sound can be output via PDM.
#define PIN_LED_RGBW 11    // SKC6812 RGBW LED.
// --
#define PIN_MOT_A_1 14  // DC Motor control lines.
#define PIN_MOT_A_2 13
#define PIN_MOT_A_PWM 12
#define PIN_MOT_B_1 15
#define PIN_MOT_B_2 20
#define PIN_MOT_B_PWM 21
// --
#define PIN_STEPPER_A_STEP 25  // Stepper Motor 1 (TMC2209) control lines.
#define PIN_STEPPER_A_DIR 24
#define PIN_STEPPER_B_STEP 23  // Stepper Motor 2 (TMC2209) control lines.
#define PIN_STEPPER_B_DIR 22
// --
#define PIN_SERVO_1 18  // Shared with GPIO availabe to user (NOT forced as output by MKFG on startup).
#define PIN_SERVO_2 19  // Shared with GPIO availabe to user (NOT forced as output by MKFG on startup).
// -----------------------------------------------------
// Frequencies for playing beeps/notes.
#define MKFG_SPEAKER_FREQ_A0 1760 / 2
// --
#define MKFG_SPEAKER_FREQ_A 1760
#define MKFG_SPEAKER_FREQ_B 1976
#define MKFG_SPEAKER_FREQ_C 2093
#define MKFG_SPEAKER_FREQ_D 2349
#define MKFG_SPEAKER_FREQ_E 2637
#define MKFG_SPEAKER_FREQ_F 2794
#define MKFG_SPEAKER_FREQ_G 3136
// --
#define MKFG_SPEAKER_FREQ_A2 3520
#define MKFG_SPEAKER_FREQ_B2 3951
#define MKFG_SPEAKER_FREQ_C2 4186
// -----------------------------------------------------
// Beep patterns for alarms/notifications/etc.
#define MKFG_BEEP_1X 1
#define MKFG_BEEP_2X 2
#define MKFG_BEEP_3X 3
#define MKFG_BEEP_4X 4
#define MKFG_BEEP_5X 5
#define MKFG_BEEP_MED_1X 11
#define MKFG_BEEP_MED_2X 12
#define MKFG_BEEP_MED_3X 13
#define MKFG_BEEP_MED_4X 14
#define MKFG_BEEP_MED_5X 15
#define MKFG_BEEP_LONG 100
#define MKFG_BEEP_INIT 200
#define MKFG_BEEP_INITb 201
#define MKFG_BEEP_INITc 202
#define MKFG_BEEP_INITd 203
#define MKFG_BEEP_INITe 204
// -----------------------------------------------------
// RGBW Status LED
#define MKFG_NUM_RGBW_PIXELS 1
static Adafruit_NeoPixel _mkfg_pixel(MKFG_NUM_RGBW_PIXELS, PIN_LED_RGBW, NEO_GRBW + NEO_KHZ800);
// -----------------------------------------------------
// OLED Display (0.96", 128x64 monochrome)
#define MKFG_DISPLAY_WIDTH 128  // OLED display width, in pixels
#define MKFG_DISPLAY_HEIGHT 64  // OLED display height, in pixels
// -- Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
static Adafruit_SSD1306 _mkfg_display(MKFG_DISPLAY_WIDTH, MKFG_DISPLAY_HEIGHT, &Wire);
// -- Macros for making writing text to display just 1 line of code.
#define mkfg_display_clearAndPrintTxt(txtA) \
  _mkfg_display.clearDisplay(); \
  _mkfg_display.setCursor(0, 0); \
  _mkfg_display.print(txtA); \
  _mkfg_display.display();
#define mkfg_display_clearAndShowError_2lines(txtA, txtB) \
  _mkfg_display.clearDisplay(); \
  _mkfg_display.setCursor(0, 0); \
  _mkfg_display.print(txtA); \
  _mkfg_display.setCursor(0, 12); \
  _mkfg_display.print(txtB); \
  _mkfg_display.display();
// -----------------------------------------------------
// Custom PWM (for writing tones to speaker, and controlling DC motor/solenoid/vibe/etc. speed).
#define MKFG_BEEPER_PWM_MAXVAL 1024
#define MKFG_BEEPER_PWM_MIDVAL (MKFG_BEEPER_PWM_MAXVAL / 2)
// static uint64_t _pwmInitted = 0;
// static pwm_config _speakerPWMConfig;
// static bool _speakerIsInit = false;
// static unsigned long _beepOffMs = 0;
// static uint8_t _beepRequestNextType = 0;
// static unsigned long _beepRequestAtMs = 0;
// -----------------------------------------------------
// STEPPER MOTORS (via TMC2209 silent stepper driver chips)
#define MKFG_TMC_STEPPER_AUTO_CAL true     // When true, runs a short motor ramp up to see what values are best for tuning it...
#define MKFG_TMC_STALL_VALUE 100           // [0..255] higher is less sensitive
#define MKFG_TMC_SERIAL_PORT Serial1       // TMC2209 HardwareSerial port (on RP2350, Serial1 ==> UART0).
#define MKFG_TMC_R_SENSE 0.11f             // Match to your driver (0.11Ohm for BigTreeTech TMC2209 v1.2)
#define MKFG_STEPPERA_DRIVER_ADDRESS 0b00  // TMC2209 Driver address according to MS1 and MS2
#define MKFG_STEPPERB_DRIVER_ADDRESS 0b01  // TMC2209 Driver address according to MS1 and MS2
#define MKFG_STEPPER_MICROSTEPS 8          // higher is smoother, but limits how fast we can step... (power of 2, upto 256).
// --
static TMC2209Stepper _mkfg_stepperA_tmc(&MKFG_TMC_SERIAL_PORT, MKFG_TMC_R_SENSE, MKFG_STEPPERA_DRIVER_ADDRESS);
static TMC2209Stepper _mkfg_stepperB_tmc(&MKFG_TMC_SERIAL_PORT, MKFG_TMC_R_SENSE, MKFG_STEPPERB_DRIVER_ADDRESS);
static AccelStepper _mkfg_stepperA_as = AccelStepper(_mkfg_stepperA_as.DRIVER, PIN_STEPPER_A_STEP, PIN_STEPPER_A_DIR);
static AccelStepper _mkfg_stepperB_as = AccelStepper(_mkfg_stepperB_as.DRIVER, PIN_STEPPER_B_STEP, PIN_STEPPER_B_DIR);
// -----------------------------------------------------
// Encoder knob handler
#define MKFG_BTN_MULTI_CLICK_TIMEOUT_MS 400  // multi-click timout (~300-500ms is good here)
#define MKFG_BTN_VERY_LONG_CLICK_MS 800      // very long click / press-and-hold (~600-1000ms is good here)
// -----------------------------------------------------
// Timers (fast interrupts for driving motors and flagging user interaction)
#define MKFG_TIMER0_INTERVAL_US (30)
#define MKFG_TIMER1_INTERVAL_US (4000)
static RPI_PICO_Timer _mkfg_ITimer0(0);  // #0 => AccelSteppers -- Init RPI_PICO_Timer, can use any from 0-15 pseudo-hardware timers
static RPI_PICO_Timer _mkfg_ITimer1(1);  // #1 => UI -- Init RPI_PICO_Timer, can use any from 0-15 pseudo-hardware timers
// static RPI_PICO_Timer _mkfg_ITimer2(2); // #2 => User if needed -- Init RPI_PICO_Timer, can use any from 0-15 pseudo-hardware timers
static volatile bool _mkfg_ui_needsUpdated = true;
// -----------------------------------------------------
class MKFG {
public:
  // -- static var convenience definitions --
  Adafruit_NeoPixel *pixel        = &_mkfg_pixel;
  Adafruit_SSD1306  *display      = &_mkfg_display;
  TMC2209Stepper    *stepperA_tmc = &_mkfg_stepperA_tmc;
  TMC2209Stepper    *stepperB_tmc = &_mkfg_stepperB_tmc;
  AccelStepper      *stepperA_as  = &_mkfg_stepperA_as;
  AccelStepper      *stepperB_as  = &_mkfg_stepperB_as;
  // --
  Servo servo1;
  Servo servo2;
  // --
  bool btn1IsDown = false;
  bool btn2IsDown = false;
  bool btn3IsDown = false;
  bool encoderBtnIsDown       = false;
  bool encoderTurnedWhileDown = false;
  bool encoderIsHeldDownLong  = false;
  // --
  volatile uint8_t _encVal         = 0xFF;
  volatile int8_t  _encDelta       = 0;
  unsigned long    _encBtnLastDnMs = 0;
  uint8_t          _encBtnTapCount = 0;
  // --
  pwm_config    _speakerPWMConfig;
  uint64_t      _pwmInitted          = 0;
  bool          _speakerIsInit       = false;
  unsigned long _beepOffMs           = 0;
  uint8_t       _beepRequestNextType = 0;
  unsigned long _beepRequestAtMs     = 0;
  unsigned int  _speakerVolPWM       = MKFG_BEEPER_PWM_MIDVAL;
  // --
  /// MKFG Constructor. This creates an instance that can be used to initialize the MKFG board, drive motors, and handle user interactions.
  MKFG(float speakerVolume) {
    // --
    if(speakerVolume >= 0.0 && speakerVolume <= 1.0) _speakerVolPWM = MKFG_BEEPER_PWM_MIDVAL*speakerVolume;
  }
  MKFG(){
    MKFG(1.0);
  }
  // --
  void rgbw(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
    _mkfg_pixel.setPixelColor(0, _mkfg_pixel.Color(r, g, b, w));
    _mkfg_pixel.show();
  }
  void rgbw_idle() {
    rgbw(0, 0, 0, 24);
  }
  void rgbw_paused() {
    rgbw(0, 32, 0, 0);
  }
  void rgbw_complete() {
    rgbw(0, 0, 50, 0);
  }
  void rgbw_warning() {
    rgbw(128, 32, 0, 0);
  }
  void _init_led_rgbw_allOff() {
    _mkfg_pixel.begin();
    _mkfg_pixel.setBrightness(32);
    _mkfg_pixel.setPixelColor(0, 0);
    _mkfg_pixel.show();
  }
  void _anim_led_rgbw_1000ms() {
    for (int i = 0; i < 100; i++) {
      _mkfg_pixel.setPixelColor(0, _mkfg_pixel.Color(i * 2, 0, i * 2, i));
      _mkfg_pixel.show();
      delay(5);
    }
    for (int i = 0; i < 50; i++) {
      _mkfg_pixel.setPixelColor(0, _mkfg_pixel.Color(200 - i * 4, i * 4, 200 - i * 4, 100));
      _mkfg_pixel.show();
      delay(5);
    }
    for (int i = 0; i < 50; i++) {
      _mkfg_pixel.setPixelColor(0, _mkfg_pixel.Color(0, 200 - i * 4, 0, 100 - i));
      _mkfg_pixel.show();
      delay(5);
    }
    _mkfg_pixel.setPixelColor(0, _mkfg_pixel.Color(0, 0, 0, 24));
    _mkfg_pixel.show();
  }
  // --
  void _speaker_change_freq(uint32_t frequency) {
    float clkDivFloat = (clock_get_hz(clk_sys) / ((float)MKFG_BEEPER_PWM_MAXVAL * frequency));
    if (clkDivFloat > 255.0) {
      Serial.printf("MKFG - WARNING! - PWM[%d] (clk_sys / (maxVal * frequency)) = %f > 255.0. Choose higher frequency or maxVal.\n", PIN_SPKR_PIEZO, clkDivFloat);
    }
    if (_speakerIsInit) {
      pwm_config_set_clkdiv(&_speakerPWMConfig, clkDivFloat);
      pwm_init(pwm_gpio_to_slice_num(PIN_SPKR_PIEZO), &_speakerPWMConfig, true);
    }
  }
  void _pwm_init_with_specs(uint gpio, uint32_t frequency, uint32_t maxVal, uint16_t val) {
    // ref: https://github.com/earlephilhower/arduino-pico/blob/master/cores/rp2040/wiring_analog.cpp
    // --
    // Note: Lowest possible PWM frequency determined by maxVal.
    // maxVal: 255=>3kHz, 511=>1.5kHz, 1023=>800Hz, 2047=>400Hz, 4095=>200Hz, 8191=>100Hz, 16383=>50Hz, 32767=>25Hz
    float clkDivFloat = (clock_get_hz(clk_sys) / ((float)maxVal * frequency));
    if (clkDivFloat > 255.0) {
      Serial.printf("MKFG - WARNING! - PWM[%d] (clk_sys / (maxVal * frequency)) = %f > 255.0. Choose higher frequency or maxVal.\n", gpio, clkDivFloat);
    }
    if (!(_pwmInitted & (1LL << pwm_gpio_to_slice_num(gpio)))) {
      pwm_config c = pwm_get_default_config();
      pwm_config_set_clkdiv(&c, clkDivFloat);
      pwm_config_set_wrap(&c, maxVal - 1);
      pwm_init(pwm_gpio_to_slice_num(gpio), &c, true);
      _pwmInitted |= 1LL << pwm_gpio_to_slice_num(gpio);
      if (gpio == PIN_SPKR_PIEZO) {
        _speakerPWMConfig = c;
        _speakerIsInit = true;
      }
      // --
      gpio_set_function(gpio, GPIO_FUNC_PWM);
      pwm_set_gpio_level(gpio, val);
      Serial.printf("MKFG - PWM on GPOIO %d init okay (%d Hz, %d maxVal, %d initial val, clkDivFloat=%f)!\n", gpio, (uint32_t)frequency, (uint16_t)maxVal, val, clkDivFloat);
    } else {
      Serial.printf("MKFG - PWM on GPIO %d already init or PIN is invalid.\n", gpio);
    }
  }
  // --
  void _init_speaker() {
    digitalWrite(PIN_SPKR_PIEZO, LOW);
    pinMode(PIN_SPKR_PIEZO, OUTPUT);
    _pwm_init_with_specs(PIN_SPKR_PIEZO, MKFG_SPEAKER_FREQ_A, 1024, 0);
  }
  // --
  void requestBeep(uint8_t beepType) {
    switch (beepType) {
      case MKFG_BEEP_1X:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_C);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 85) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_2X:
      case MKFG_BEEP_3X:
      case MKFG_BEEP_4X:
      case MKFG_BEEP_5X:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_C);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 70) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType - 1;
          _beepRequestAtMs = (millis() + 160) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_MED_1X:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_C);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 500) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_MED_2X:
      case MKFG_BEEP_MED_3X:
      case MKFG_BEEP_MED_4X:
      case MKFG_BEEP_MED_5X:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_B);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 475) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType - 1;
          _beepRequestAtMs = (millis() + 1000) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_LONG:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_A);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 1000) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_INIT:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_E);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 75) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType + 1;
          _beepRequestAtMs = (millis() + 125) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_INITb:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_G);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 75) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType + 1;
          _beepRequestAtMs = (millis() + 125) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_INITc:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_A2);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 75) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType + 1;
          _beepRequestAtMs = (millis() + 125) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_INITd:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_B);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 75) | 0x01;  // do not allow a value of zero.
          _beepRequestNextType = beepType + 1;
          _beepRequestAtMs = (millis() + 150) | 0x01;  // do not allow a value of zero.
          break;
        }
      case MKFG_BEEP_INITe:
        {
          _speaker_change_freq(MKFG_SPEAKER_FREQ_C);
          pwm_set_gpio_level(PIN_SPKR_PIEZO, _speakerVolPWM);
          _beepOffMs = (millis() + 150) | 0x01;  // do not allow a value of zero.
          // _beepRequestNextType = beepType+1;
          // _beepRequestAtMs = (millis() + 150) | 0x01; // do not allow a value of zero.
          break;
        }
    }
  }
  void _warn_errorCode(uint8_t beeps) {
    if (MKFG_ERRORS_BEEP) _speaker_change_freq(MKFG_SPEAKER_FREQ_A0);
    else pwm_set_gpio_level(PIN_SPKR_PIEZO, 0);
    // --
    for (int i = 0; i < beeps; i++) {
      if (MKFG_ERRORS_BEEP) pwm_set_gpio_level(PIN_SPKR_PIEZO, 512);
      rgbw(64, 16, 0, 0);
      delay(100);
      if (MKFG_ERRORS_BEEP) pwm_set_gpio_level(PIN_SPKR_PIEZO, 0);
      rgbw(0, 0, 0, 0);
      delay(150);
    }
    delay(850);
  }
  // -------
  void _display_init() {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!_mkfg_display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("MKFG I2C OLED - SSD1306 allocation failed."));
    }
    // _mkfg_display.display();        // the library initializes this with an Adafruit splash screen.
    // _mkfg_display.drawPixel(40, 40, WHITE); // Draw a single pixel in white
    _mkfg_display.setTextSize(1);       // set the font size, supports sizes from 1 to 8
    _mkfg_display.setTextColor(WHITE);  // Draw white text
    // --
    mkfg_display_clearAndPrintTxt("MKFG Initializing...");
  }
  // -------
  void _init_servos() {
    // mkfg_servo1.setPeriodHertz(50); // PWM frequency for Servo
    servo1.attach(PIN_SERVO_1, 1000, 2000, 90);  // Minimum and maximum pulse width (in µs) to go from 0° to 180°.
    // --
    // mkfg_servo2.setPeriodHertz(50); // PWM frequency for Servo
    servo2.attach(PIN_SERVO_2, 1000, 2000, 90);  // Minimum and maximum pulse width (in µs) to go from 0° to 180°.
    // --
    // if(true){ // Servo test...
    //   mkfg_servo1.write(0);
    //   mkfg_servo2.write(0);
    //   delay(1000);
    //   for(int i=0; i<180; i+=5){
    //     mkfg_servo1.write(i);
    //     mkfg_servo2.write(i);
    //     delay(20);
    //   }
    //   for(int i=180; i>=90; i-=2){
    //     mkfg_servo1.write(i);
    //     mkfg_servo2.write(i);
    //     delay(20);
    //   }
    // }
  }
  void _init_pwm_motor_control() {
    // -- PWM DC MOTOR CONTROL: A
    digitalWrite(PIN_MOT_A_1, LOW);
    pinMode(PIN_MOT_A_1, OUTPUT);
    digitalWrite(PIN_MOT_A_2, LOW);
    pinMode(PIN_MOT_A_2, OUTPUT);
    digitalWrite(PIN_MOT_A_PWM, LOW);
    pinMode(PIN_MOT_A_PWM, OUTPUT);
    // _mkfg_pwm_init_with_specs(PIN_MOT_A_PWM, 80, 32767, 0); // 40Hz Bass vibe test unit.
    _pwm_init_with_specs(PIN_MOT_A_PWM, 80000, 1024, 0);  // 80kHz DC motor speed control test.
    // -- PWM DC MOTOR CONTROL: B
    digitalWrite(PIN_MOT_B_1, LOW);
    pinMode(PIN_MOT_B_1, OUTPUT);
    digitalWrite(PIN_MOT_B_2, LOW);
    pinMode(PIN_MOT_B_2, OUTPUT);
    digitalWrite(PIN_MOT_B_PWM, LOW);
    pinMode(PIN_MOT_B_PWM, OUTPUT);
    _pwm_init_with_specs(PIN_MOT_B_PWM, 80000, 1024, 0);  // 80kHz DC motor speed control test.
    // --
    // if(true){ // TESTING DC MOTOR OUTPUT...
    //   mkfg_rgbw(128,0,0,0);
    //   digitalWrite(PIN_MOT_A_1, HIGH);
    //   pwm_set_gpio_level(PIN_MOT_A_PWM, 16384);
    //   delay(1000);
    //   pwm_set_gpio_level(PIN_MOT_A_PWM, 0);
    //   digitalWrite(PIN_MOT_A_1, LOW);
    //   // --
    //   delay(500);
    //   // --
    //   mkfg_rgbw(128,128,0,0);
    //   digitalWrite(PIN_MOT_B_1, HIGH);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 255);
    //   delay(1000);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 0);
    //   digitalWrite(PIN_MOT_B_1, LOW);
    //   delay(500);
    //   digitalWrite(PIN_MOT_B_2, HIGH);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 255);
    //   delay(1000);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 0);
    //   digitalWrite(PIN_MOT_B_2, LOW);
    //   delay(500);
    //   digitalWrite(PIN_MOT_B_1, HIGH);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 128);
    //   delay(1000);
    //   pwm_set_gpio_level(PIN_MOT_B_PWM, 0);
    //   digitalWrite(PIN_MOT_B_1, LOW);
    //   // --
    //   mkfg_rgbw_idle();
    // }
  }
  // ------
  void _steppers_ensureConnectedAndSetToIdle() {
    MKFG_TMC_SERIAL_PORT.begin(115200);  // can be upto 115200*4, but going 1/4 speed while debugging to ensure everything is solid.
    // --
    digitalWrite(PIN_STEPPER_A_STEP, LOW);
    pinMode(PIN_STEPPER_A_STEP, OUTPUT);
    digitalWrite(PIN_STEPPER_A_DIR, LOW);
    pinMode(PIN_STEPPER_A_DIR, OUTPUT);
    // --
    digitalWrite(PIN_STEPPER_B_STEP, LOW);
    pinMode(PIN_STEPPER_B_STEP, OUTPUT);
    digitalWrite(PIN_STEPPER_B_DIR, LOW);
    pinMode(PIN_STEPPER_B_DIR, OUTPUT);
    // --
    _mkfg_stepperA_tmc.begin();  // Do "begin" for each. Then pause 500ms and re-begin/init driver chips? (this also calls: "pdn_disable(true); mstep_reg_select(true);"")
    uint8_t connErr = _mkfg_stepperA_tmc.test_connection();
    while (connErr != 0) {
      Serial.printf("MKFG mkfg_stepperA Failed to connect... Error: %d\n", connErr);
      mkfg_display_clearAndShowError_2lines("Stepper A Error", "-> Is 12V connected?");
      _warn_errorCode(2);
      delay(3000);
    }
    _mkfg_stepperA_tmc.rms_current(1);  // Set max current to 1 mA
    _mkfg_stepperA_tmc.freewheel(1);    // 0 = normal mode. 1 = freewheeling. 2 & 3 are various ways to short the coils (i.e. brake)
    _mkfg_stepperA_tmc.toff(0);         // DISABLE DRIVER...
    // --
    _mkfg_stepperB_tmc.begin();  // Do "begin" for each. Then pause 500ms and re-begin/init driver chips? (this also calls: "pdn_disable(true); mstep_reg_select(true);"")
    connErr = _mkfg_stepperB_tmc.test_connection();
    while (connErr != 0) {
      Serial.printf("MKFG mkfg_stepperB Failed to connect... Error: %d\n", connErr);
      mkfg_display_clearAndShowError_2lines("Stepper B Error", "-> Is 12V connected?");
      _warn_errorCode(3);
      delay(3000);
    }
    _mkfg_stepperB_tmc.rms_current(1);  // Set max current to 1 mA
    _mkfg_stepperB_tmc.freewheel(1);    // 0 = normal mode. 1 = freewheeling. 2 & 3 are various ways to short the coils (i.e. brake)
    _mkfg_stepperB_tmc.toff(0);         // DISABLE DRIVER...
    // --
    // Configure AccelStepper!!
    _mkfg_stepperA_as.setMaxSpeed(2000 * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperA_as.setAcceleration(2400 * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperA_as.setEnablePin(0xff);
    _mkfg_stepperA_as.setPinsInverted(false, false, true);
    // _mkfg_stepperA_as.enableOutputs(); // already enabled.
    // --
    _mkfg_stepperB_as.setMaxSpeed(2000 * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperB_as.setAcceleration(2400 * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperB_as.setEnablePin(0xff);
    _mkfg_stepperB_as.setPinsInverted(false, false, true);
  }
  void stepper_initDriver(TMC2209Stepper tmc, uint16_t maxCurrentMa, uint8_t pwm_ofs, uint8_t pwm_grad) {
    // defaults
    // GCONF_register.i_scale_analog = 1;
    // GCONF_register.internal_rsense = 0; // OTP
    // GCONF_register.en_spreadcycle = 0; // OTP
    // GCONF_register.multistep_filt = 1; // OTP
    // IHOLD_IRUN_register.iholddelay = 1; // OTP
    // TPOWERDOWN_register.sr = 20;
    // CHOPCONF_register.sr = 0x10000053;
    // PWMCONF_register.sr = 0xC10D0024;
    // --
    // tmc.begin(); // Do "begin" for each. Then pause 500ms and re-begin/init driver chips? (this also calls: "pdn_disable(true); mstep_reg_select(true);"")
    // uint8_t connErr = tmc.test_connection();
    // if(connErr != 0){
    //   Serial.printf("MKFG TMC[%d] Failed to connect... Error: %d\n", tmcAddr, connErr);
    //   return false;
    // }
    // delay(500);
    // tmc.begin();
    // --
    tmc.defaults();  // load defaults into memory. Not written yet.
    // tmc.push();
    // --
    tmc.I_scale_analog(false);  // Use intetrnal reference (not external VRef) for max current calculations.
    // --
    tmc.pwm_lim(12);
    // --
    tmc.freewheel(0);                     // 0 = normal mode. 1 = freewheeling. 2 & 3 are various ways to short the coils (i.e. brake)
    tmc.rms_current(maxCurrentMa, 0.35);  // mA --> [150 min, 800 is a lot] this has a major impact on the NOISE of the motor (go small if possible, but need to check for stalling?).
    // tmc.mstep_reg_select(true); // microsteps set by register (not physical pins on chip). ALREADY SET IN tmc.begin().
    tmc.microsteps(MKFG_STEPPER_MICROSTEPS);
    // tmc.pwm_autoscale(true); // needed for stealthchop?
    // --
    tmc.toff(4);         // Enables driver in software
    tmc.blank_time(24);  //tmc.tbl(1);  // equivalent to "tmc.blank_time(24);"
    // tmc.hstrt(4);
    // tmc.hend(1);
    // --
    tmc.TCOOLTHRS(400);  // if TSTEP is smaller (i.e. faster) than this, cool step is used.
    // tmc.TCOOLTHRS(0xFFFFF); // 20bit max
    tmc.TPWMTHRS(100);  // TPWM_THRS=500 yields a switching velocity about 35000 = ca. 30RPM
    // tmc.TPOWERDOWN(0x0000000A);  // TPOWERDOWN=10, 20 is default.
    tmc.semin(5);
    tmc.semax(2);
    tmc.sedn(0b01);  // 01: For each 8 StallGuard4 values decrease by one
    // tmc.en_spreadCycle(true); // We can enable spread cycle, but it's noisier for slow speeds.
    tmc.SGTHRS(MKFG_TMC_STALL_VALUE);
    // --
    tmc.en_spreadCycle(false);  // disable spread cycle for calibration.
    tmc.intpol(true);           // interpolate to 256 microsteps, seems to greatly reduce sound.
    // --
    if (pwm_ofs > 0 && pwm_grad > 0) {
      // MANUAL CAL (using values derived from auto-cal with desired motor connected).
      tmc.pwm_autoscale(false);  // "automatic tuning mode (AT) using current feedback (pwm_autoscale = 1, pwm_autograd = 1)"
      tmc.pwm_autograd(false);
      tmc.pwm_ofs(pwm_ofs);    // ~0x4C for single motor. ~0x2B for 2 in parallel.
      tmc.pwm_grad(pwm_grad);  // ~0x0C for single motor. ~0x1B for 2 in parallel.
    } else {
      tmc.pwm_autoscale(true);  // "automatic tuning mode (AT) using current feedback (pwm_autoscale = 1, pwm_autograd = 1)"
      tmc.pwm_autograd(true);
    }
    // --
    // tmc.pwm_freq(0b10);         // 2/512 = fastest PWM for internal clock recomended. ~46kHz
    // --
    // We can re-enable spread cycle now that we've calibrated, but it's noisier for slow speeds.
    // tmc.en_spreadCycle(true);
    // --
    // tmc.en_spreadCycle(false);  // disable spread cycle for calibration.
    // tmc.pwm_autoscale(true);  // "automatic tuning mode (AT) using current feedback (pwm_autoscale = 1, pwm_autograd = 1)"
    // tmc.pwm_autograd(1);
    // --
    // Serial.printf("MKFG TMC[%d] Stepper initialized.\n", tmcAddr);
  }
  void stepper_initDriver(TMC2209Stepper tmc, uint16_t maxCurrentMa) {
    stepper_initDriver(tmc, maxCurrentMa, 0, 0);  // use autoscale/autograd if no PWM_OFS/PWM_GRAD values are supplied.
  }
  void _stepper_autocal(TMC2209Stepper tmc, uint32_t rpsToStepsMul = 4480) {
    // Serial.println("- - - - - - - - - - - - - - - - - - - -");
    Serial.println("* * * AUTO CALIBRATION TESTING * * *");
    rgbw(0, 0, 128, 0);
    // Serial.print("PWM_OFS_AUTO: 0x");
    // Serial.println(tmc.pwm_ofs_auto(), HEX);
    // Serial.print("PWM_GRAD_AUTO: 0x");
    // Serial.println(tmc.pwm_grad_auto(), HEX);
    // --
    tmc.en_spreadCycle(false);  // disable spread cycle for calibration.
    tmc.pwm_autoscale(true);    // "automatic tuning mode (AT) using current feedback (pwm_autoscale = 1, pwm_autograd = 1)"
    tmc.pwm_autograd(1);
    // --
    // tmc.pwm_lim(200);
    // --
    // NOTE: TPWMTHRESH is CRITICAL --> "Above this velocity the driver switches to SpreadCycle; disabling CoolStep and StallGuard."
    // --
    // Serial.println("Optimizing...");
    tmc.VACTUAL(0);
    delay(250);  // standstill
    for (int i = 0; i < 100; i++) {
      tmc.VACTUAL((rpsToStepsMul * i) / 100);
      delay(15);
    }
    delay(250);
    for (int i = 100; i >= 0; i--) {
      tmc.VACTUAL((rpsToStepsMul * i) / 100);
      delay(15);
    }
    delay(500);
    tmc.shaft(false);
    tmc.VACTUAL(rpsToStepsMul / 4);
    delay(500);
    tmc.shaft(true);
    delay(500);
    tmc.VACTUAL(rpsToStepsMul / 2);
    delay(500);
    tmc.shaft(false);
    delay(500);
    tmc.VACTUAL(0);
    delay(1000);
    Serial.print(" >> PWM_OFS_AUTO: 0x");
    uint8_t pwm_ofs = tmc.pwm_ofs_auto();
    Serial.println(pwm_ofs, HEX);
    Serial.print(" >> PWM_GRAD_AUTO: 0x");
    uint8_t pwm_grad = tmc.pwm_grad_auto();
    Serial.println(pwm_grad, HEX);
    // --
    tmc.pwm_autoscale(false);  // "automatic tuning mode (AT) using current feedback (pwm_autoscale = 1, pwm_autograd = 1)"
    tmc.pwm_autograd(false);
    tmc.pwm_ofs(pwm_ofs);    // ~0x4C for single motor. ~0x2B for 2 in parallel.
    tmc.pwm_grad(pwm_grad);  // ~0x0C for single motor. ~0x1B for 2 in parallel.
    // tmc.en_spreadCycle(false);  // We can enable spread cycle, but it's noisier for slow speeds.
    // --
    rgbw_idle();
    Serial.println("- - - - - - - - - - - - - - - - - - - -");
  }
  void steppers_runAutoCaliberation() {
    Serial.println("- - - - - - - - - - - - - - - - - - - -");
    Serial.println("MKFG Stepper A Calibration:");
    _stepper_autocal(_mkfg_stepperA_tmc);
    Serial.println("MKFG Stepper B Calibration:");
    _stepper_autocal(_mkfg_stepperB_tmc);
  }
  bool steppers_checkStatusReportWarnings() {
    uint32_t statusA = _mkfg_stepperA_tmc.DRV_STATUS();
    uint32_t statusB = _mkfg_stepperB_tmc.DRV_STATUS();
    bool aTemp120 = (statusA >> 8) & 0x01;
    bool aTemp143 = (statusA >> 9) & 0x01;
    bool aTemp150 = (statusA >> 10) & 0x01;
    bool aTemp157 = (statusA >> 11) & 0x01;
    uint8_t aCS_ACTUAL = (statusA >> 16) & 0x1f;
    uint16_t aCurrentMa = _mkfg_stepperA_tmc.cs2rms(aCS_ACTUAL);
    // --
    bool bTemp120 = (statusB >> 8) & 0x01;
    bool bTemp143 = (statusB >> 9) & 0x01;
    bool bTemp150 = (statusB >> 10) & 0x01;
    bool bTemp157 = (statusB >> 11) & 0x01;
    uint8_t bCS_ACTUAL = (statusB >> 16) & 0x1f;
    uint16_t bCurrentMa = _mkfg_stepperB_tmc.cs2rms(bCS_ACTUAL);
    // --
    if (aTemp120 || aTemp143 || aTemp150 || aTemp157 || bTemp120 || bTemp143 || bTemp150 || bTemp157) {
      Serial.printf("(!) MKFG: Stepper A: 0x%08lx -> Temps:%d,%d,%d,%d, mA:%d\n", statusA, aTemp120, aTemp143, aTemp150, aTemp157, aCurrentMa);
      Serial.printf("(!) MKFG: Stepper B: 0x%08lx -> Temps:%d,%d,%d,%d, mA:%d\n", statusB, bTemp120, bTemp143, bTemp150, bTemp157, bCurrentMa);
      return true;
    }
    // else{
    //   Serial.printf("MKFG: Steppers Status A: 0x%08lx -> Temps:%d,%d,%d,%d, mA:%d\n", statusA, aTemp120, aTemp143, aTemp150, aTemp157, aCurrentMa);
    //   Serial.printf("MKFG: Steppers Status B: 0x%08lx -> Temps:%d,%d,%d,%d, mA:%d\n", statusB, bTemp120, bTemp143, bTemp150, bTemp157, bCurrentMa);
    // }
    // --
    return false;
  }
  // ------
  void _updateEncoderDeltaNow() {
    uint8_t encAVal = gpio_get(PIN_ENCODER_A);
    uint8_t encBVal = gpio_get(PIN_ENCODER_B);
    uint8_t encValNew = 2 * encAVal + encBVal;
    if (encValNew != _encVal) {
      switch (encValNew) {
        case 0x00:
          {
            if (_encVal == 0x01) _encDelta++;
            else if (_encVal == 0x02) _encDelta--;
            // else encDelta = (encDelta < -2) ? encDelta -= 2 : ((encDelta > 2) ? encDelta += 2 : encDelta = encDelta);
            break;
          }
        case 0x01:
          {
            if (_encVal == 0x00) _encDelta--;
            else if (_encVal == 0x03) _encDelta++;
            // else encDelta = (encDelta < 2) ? encDelta -= 2 : ((encDelta > 2) ? encDelta += 2 : encDelta = encDelta);
            break;
          }
        case 0x02:
          {
            if (_encVal == 0x00) _encDelta++;
            else if (_encVal == 0x03) _encDelta--;
            // else encDelta = (encDelta < 2) ? encDelta -= 2 : ((encDelta > 2) ? encDelta += 2 : encDelta = encDelta);
            break;
          }
        case 0x03:
          {
            if (_encVal == 0x01) _encDelta--;
            else if (_encVal == 0x02) _encDelta++;
            // else encDelta = (encDelta < 2) ? encDelta -= 2 : ((encDelta > 2) ? encDelta += 2 : encDelta = encDelta);
            break;
          }
        default:
          {
            //Serial.printf("Unhandled encoder transition. Val = %02X, NewVal = %02X\n", encVal, encValNew);
          }
      }
    }
    _encVal = encValNew;
  }
  void _init_encoder() {
    _updateEncoderDeltaNow();
    delay(10);
    _updateEncoderDeltaNow();
    _encDelta = 0;
  }
  // ------
  static bool _timerHandler0_accelSteppers(struct repeating_timer *t) {  // runs every 30us (very fast)
    (void)t;
    // static bool started = false; // create local function variables in timers like this if needed.
    // --
    _mkfg_stepperA_as.run();
    _mkfg_stepperB_as.run();
    // --
    return true;  // return true when done.
  }
  void stepper_initAccelStepperRunner(float maxSpeed_A, float maxAccel_A, float maxSpeed_B, float maxAccel_B) {
    // --
    _mkfg_stepperA_as.setMaxSpeed(maxSpeed_A * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperA_as.setAcceleration(maxAccel_A * MKFG_STEPPER_MICROSTEPS);
    // --
    _mkfg_stepperB_as.setMaxSpeed(maxSpeed_B * MKFG_STEPPER_MICROSTEPS);
    _mkfg_stepperB_as.setAcceleration(maxAccel_B * MKFG_STEPPER_MICROSTEPS);
    // --
    if (_mkfg_ITimer0.attachInterruptInterval(MKFG_TIMER0_INTERVAL_US, _timerHandler0_accelSteppers)) {  // Interval in microsecs
      Serial.print(F("Starting ITimer0 OK, millis() = "));
      Serial.println(millis());
    } else {
      Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
    }
  }
  // -- TIMER INTERRUPT for USER INTERACTION --
  // Note: We don't actually want to handle the UI in an interrupt because sometimes actions needs to block (Serial, display update, etc.).
  // volatile bool _mkfg_ui_needsUpdated = true;
  static bool _timerHandler1_userInteraction(struct repeating_timer *t) {  // runs every 4ms
    (void)t;
    // static bool started = false; // create local function variables in timers like this if needed.
    // --
    _mkfg_ui_needsUpdated = true;
    // --
    return true;  // return true when done.
  }
  // ------------
  void setup(float speakerVolume) {
    if(speakerVolume >= 0.0 && speakerVolume <= 1.0) _speakerVolPWM = MKFG_BEEPER_PWM_MIDVAL*speakerVolume*speakerVolume;
    // --
    delay(100);               // let power settle 100msec on boot.
    _init_led_rgbw_allOff();  // init LED so we can use error code blinks for feedback if motor/display fails.
    _init_speaker();
    _display_init();
    // --
    _steppers_ensureConnectedAndSetToIdle();
    Serial.begin(1000000);  // Init USB serial port -- Wait 2 seconds after creating the USB Serial port to allow Serial console to see all messages during boot...
    // We have 1000ms+ to boot for Serial... So lets create a calm light fade moment here... :)
    _anim_led_rgbw_1000ms();
    // --------------------
    Serial.println("");
    Serial.println("MKFG Initializing...");
    // -- INPUTS --
    pinMode(PIN_BTN_1, INPUT_PULLUP);
    pinMode(PIN_BTN_2, INPUT_PULLUP);
    pinMode(PIN_BTN_3, INPUT_PULLUP);
    pinMode(PIN_ENCODER_A, INPUT_PULLUP);
    pinMode(PIN_ENCODER_B, INPUT_PULLUP);
    pinMode(PIN_ENCODER_BTN, INPUT_PULLUP);
    // -- OUTPUTS
    // _mkfg_init_servos(); // Servos are not initialized by default, retaining GPIO 18/19 for arbitrary use.
    requestBeep(MKFG_BEEP_INIT);
    _init_pwm_motor_control();
    // -- ENCODER (initialize last so we initialize with the latest state).
    _init_encoder();
    // --
    if (_mkfg_ITimer1.attachInterruptInterval(MKFG_TIMER1_INTERVAL_US, _timerHandler1_userInteraction)) {  // Interval in microsecs
      Serial.print(F("Starting ITimer1 OK, millis() = "));
      Serial.println(millis());
    } else {
      Serial.println(F("Can't set ITimer1. Select another freq. or timer"));
    }
    // --
    mkfg_display_clearAndPrintTxt("MKFG Ready.");
    // --
    Serial.println("(ok) MKFG Initialized.");
  }
  void setup(){
    setup(1.0);
  }
  // ------
  void loop() {
    if (_mkfg_ui_needsUpdated) {      // flag is updated via timer interrupt every 4ms to indicate that UI buttons/encoders should be checked.
      _mkfg_ui_needsUpdated = false;  // clear update flag until we need to check the UI again.
      // --
      if (_beepOffMs > 0 && ((millis() - _beepOffMs) < 1000)) {  // we've passed the off time (by upto 1000ms).
        _beepOffMs = 0;
        pwm_set_gpio_level(PIN_SPKR_PIEZO, 0);
      }
      if (_beepRequestAtMs > 0 && ((millis() - _beepRequestAtMs) < 1000)) {  // we've passed the off time (by upto 1000ms).
        _beepRequestAtMs = 0;
        requestBeep(_beepRequestNextType);
      }
      // --
      bool btn1IsDownNow = !gpio_get(PIN_BTN_1);
      bool btn2IsDownNow = !gpio_get(PIN_BTN_2);
      bool btn3IsDownNow = !gpio_get(PIN_BTN_3);
      if (btn1IsDownNow != btn1IsDown) {
        btn1IsDown = btn1IsDownNow;
        mkfg_onBtn1Changed(btn1IsDown);
      }
      if (btn2IsDownNow != btn2IsDown) {
        btn2IsDown = btn2IsDownNow;
        mkfg_onBtn2Changed(btn2IsDown);
      }
      if (btn3IsDownNow != btn3IsDown) {
        btn3IsDown = btn3IsDownNow;
        mkfg_onBtn3Changed(btn3IsDown);
      }
      // --
      bool encoderBtnIsDownNow = !gpio_get(PIN_ENCODER_BTN);
      if (encoderBtnIsDownNow != encoderBtnIsDown) {
        encoderBtnIsDown = encoderBtnIsDownNow;
        mkfg_onEncoderBtnChanged(encoderBtnIsDown);
        // --
        if (encoderBtnIsDown) {  // handle multi-click counting.
          if (millis() - _encBtnLastDnMs < MKFG_BTN_MULTI_CLICK_TIMEOUT_MS) _encBtnTapCount++;
          else _encBtnTapCount = 1;
          _encBtnLastDnMs = millis();
        } else {
          encoderTurnedWhileDown = false;
          encoderIsHeldDownLong = false;
        }
      }
      _updateEncoderDeltaNow();
      if (_encDelta > 1 || _encDelta < -1) {
        mkfg_onEncoderTurned(_encDelta / 2, encoderBtnIsDown);
        _encDelta = 0;
        if (encoderBtnIsDown) encoderTurnedWhileDown = true;
      }
      // --
      if (_encBtnLastDnMs != 0 && millis() - _encBtnLastDnMs > MKFG_BTN_MULTI_CLICK_TIMEOUT_MS) {
        unsigned long encBtnLastDnMsPrev = _encBtnLastDnMs;
        _encBtnLastDnMs = 0;
        switch (_encBtnTapCount) {
          case 1:
            {
              if (encoderBtnIsDown) {
                //  CLICK and still HELD...
                if (millis() - encBtnLastDnMsPrev > MKFG_BTN_VERY_LONG_CLICK_MS) {
                  // LONG PRESS HERE!
                  if (!encoderTurnedWhileDown) {
                    encoderIsHeldDownLong = true;
                    mkfg_onEncoderPressAndHold();  // only allow "long press" if knob was not turned while holding down.
                  }
                } else {
                  _encBtnLastDnMs = encBtnLastDnMsPrev;  // set down time back to orig so we can check back later for long press...
                }
              } else {
                // CLICK x1
                mkfg_onEncoderClick(_encBtnTapCount);
              }
              break;
            }
          case 2:
          case 3:
          case 4:
          case 5:
            {
              // CLICK xN
              mkfg_onEncoderClick(_encBtnTapCount);
              break;
            }
          default:
            {
              Serial.printf("MKFG Encoder CLICK x %d --> Unspecified. Ignoring.\n", _encBtnTapCount);
            }
        }
      }
      // --
    }
  }
};


class MKFGWorkHorse2350 : public MKFG {
  // For now, we only have 1 board variant and it's all in MKFG. Using this extended class for future proofing.
};

#endif