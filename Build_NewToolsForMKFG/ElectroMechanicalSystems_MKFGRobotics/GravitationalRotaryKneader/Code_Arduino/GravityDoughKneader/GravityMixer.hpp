// Gravity Mixer (variables, helpers, utilities, etc.)
// -----------------------------------------------------------
#define GM_STEPPERA_STEPS_PER_REV 200
#define GM_STEPPERB_STEPS_PER_REV 200
// --
#define GM_MUL_MIN 0.10
#define GM_MUL_STEP 0.025
#define GM_INITIAL_VEL_TARGET_RPS 2.0
#define GM_PROG_TAPERDOWN_MIN 20
#define GM_PROG_SINEWAVE_MIN 3.7
// --
bool gm_isPausedByUser = true;  // start paused to be safe.
bool gm_isReversedByUser = false;
float gm_velocityTargetRPS = GM_INITIAL_VEL_TARGET_RPS;  // Programmed velocity to target.
float gm_velocityMultiplier = 0.5;                       // User controllable velocty multiplier [-1,+1].
// --
volatile unsigned long gm_progStartMillis = 0;
enum GMProgType {
  GM_PROG_FIXEDSPEED,
  GM_PROG_TAPERDOWN_N_MIN,
  GM_PROG_SINEWAVE_N_MIN,
};
uint32_t gmProgramNotifyCompleteMinutesByType[] = {
  10, // GM_PROG_FIXEDSPEED
  GM_PROG_TAPERDOWN_MIN, // GM_PROG_TAPERDOWN_N_MIN
  15, // GM_PROG_SINEWAVE_N_MIN
};
volatile GMProgType gm_runningProgram = GM_PROG_FIXEDSPEED;
// -----------------------------------------------------------
float gm_getVelocityTargetAtProgMsOffset(unsigned long progMillis){
  float nextVelocityTargetRPS = 0.0;
  switch (gm_runningProgram) {
    case GM_PROG_FIXEDSPEED:
      {
        nextVelocityTargetRPS = GM_INITIAL_VEL_TARGET_RPS;
        break;
      }
    case GM_PROG_TAPERDOWN_N_MIN:
      {
        float lowRPS = 0.30;
        float highRPS = 1.5;
        const int taperdownBuildupMin = 3; // 3 minutes seems greaet in practice.
        if(progMillis < (taperdownBuildupMin*60000)){ // ramp up first minute or two to let dough settle into sides/front of tub.
          float progPct1 = progMillis / ((float)taperdownBuildupMin*60000.0);
          // nextVelocityTargetRPS = lowRPS + fmaxf(0, (highRPS - lowRPS) * progPct1 * progPct1); 
          nextVelocityTargetRPS = highRPS / 2.0 * (1.0-cosf(progPct1*3.14159));
        }else{
          float progPct1 = (progMillis / 1000.0 - taperdownBuildupMin*60) / (60.0 * (GM_PROG_TAPERDOWN_MIN-taperdownBuildupMin));
          nextVelocityTargetRPS = lowRPS + fmaxf(0, (highRPS - lowRPS) * (1.0 - progPct1)); // taper down over time.
        }
        break;
      }
    case GM_PROG_SINEWAVE_N_MIN:
      {
        float highRPS = 2.0;
        float thetaRad = 3.14159 * 2.0 * (progMillis / 1000.0) / (60.0 * GM_PROG_SINEWAVE_MIN);
        nextVelocityTargetRPS = highRPS * sinf(thetaRad);
        break;
      }
    default:
      {
      }
  }
  return nextVelocityTargetRPS;
}
// -----------------------------------------------------------
bool gm_displayNeedsUpdated = true;
bool gm_programDidComplete = false;
// --
float gm_velocityHist40[40];
void gm_pushCurVelocityToGraphAndRequestRedraw(){
  for(int i=0; i<39; i++) gm_velocityHist40[i] = gm_velocityHist40[i+1];
  gm_velocityHist40[39] = (gm_isPausedByUser?0:1)*(gm_isReversedByUser?-1:1) * gm_velocityTargetRPS * gm_velocityMultiplier; // * gm_velocityMultiplier;
  gm_displayNeedsUpdated = true;
}
void gm_clearVelocityHistory(){
  for(int i=0; i<40; i++) gm_velocityHist40[i] = 0;
  gm_displayNeedsUpdated = true;
}
// -----------------------------------------------------------
void gm_selectAndStartProgramNow(GMProgType progType){
  gm_runningProgram = progType;
  gm_progStartMillis = millis();
  gm_velocityMultiplier = 0.5;  // start at 50% to give user ability to go faster/slower as needed.
  gm_isPausedByUser = false;
  gm_isReversedByUser = false;
  gm_programDidComplete = false;
  gm_displayNeedsUpdated = true;
  gm_clearVelocityHistory();
  mkfg.requestBeep(MKFG_BEEP_1X);
}
// -----------------------------------------------------------
void gm_drawDisplayIfNeeded(){
  // Always update status light each time. This is in-part a safety consideration.
  unsigned long curProgMs = millis() - gm_progStartMillis;
  if(gm_isPausedByUser){
    mkfg.rgbw_paused();
  }else if(curProgMs/60000 >= GM_PROG_TAPERDOWN_MIN){
    mkfg.rgbw_complete();
    if(!gm_programDidComplete){
      gm_programDidComplete = true;
      mkfg.requestBeep(MKFG_BEEP_MED_4X);
      gm_displayNeedsUpdated = true;
    }
  }else{
    mkfg.rgbw_idle();
  }
  // -------------------------------
  if(!gm_displayNeedsUpdated) return;
  // -------------------------------
  gm_displayNeedsUpdated = false;
  Serial.printf("GM: Updating Display...\n");
  // --
  mkfg.display->clearDisplay();      // Clear the buffer
  mkfg.display->setCursor(0,0);      // set the coordinates to start writing text
  if(gm_isPausedByUser){
    mkfg.display->print("** PAUSED **"); // print the characters at location x,y  
  }else{
    switch (gm_runningProgram) {
    case GM_PROG_FIXEDSPEED:
      {
        mkfg.display->print("Fixed");
        break;
      }
    case GM_PROG_TAPERDOWN_N_MIN:
      {
        mkfg.display->print("Taper");
        break;
      }
    case GM_PROG_SINEWAVE_N_MIN:
      {
        mkfg.display->print("Sine");
        break;
      }
    default:
      {
        mkfg.display->print("Unknown?");
      }
    }
    // --
    mkfg.display->setCursor(41,0);      // set the coordinates to start writing text
    mkfg.display->printf("%d /%dm", curProgMs/60000, gmProgramNotifyCompleteMinutesByType[gm_runningProgram]); // print the characters at location x,y  
  }
  // --
  mkfg.display->setCursor(MKFG_DISPLAY_WIDTH-30,0);      // set the coordinates to start writing text
  mkfg.display->printf("x%1.2f", gm_velocityMultiplier * gm_velocityMultiplier); // print the characters at location x,y  
  // --
  for(int i=0; i<MKFG_DISPLAY_WIDTH/8; i++){ // dotted horizaontal line at v=0
    mkfg.display->drawPixel(i*8,MKFG_DISPLAY_HEIGHT-25, WHITE);
  }
  for(int i=0; i<12; i++){ // dotted vertical line at t=0
    mkfg.display->drawPixel(40,MKFG_DISPLAY_HEIGHT-i*4-1, WHITE);
  }
  for(int i=0; i<40; i++){ // historical velocity graph
    float v = gm_velocityHist40[i];
    v = fminf(1.5, fmaxf(-1.5, v));
    mkfg.display->drawPixel(i,MKFG_DISPLAY_HEIGHT-25-v*16, WHITE);
  }
  for(int i=0; i<MKFG_DISPLAY_WIDTH-40; i++){ // future velocity graph, 3x oversample.
    float v = (gm_isReversedByUser?-1:1)*gm_getVelocityTargetAtProgMsOffset(curProgMs + i*15000)* gm_velocityMultiplier; // * gm_velocityMultiplier;
    v = fminf(1.5, fmaxf(-1.5, v));
    mkfg.display->drawPixel(i+40,MKFG_DISPLAY_HEIGHT-25-v*16, WHITE);
    v = (gm_isReversedByUser?-1:1)*gm_getVelocityTargetAtProgMsOffset(curProgMs + i*15000 + 5000)* gm_velocityMultiplier; // * gm_velocityMultiplier;
    v = fminf(1.5, fmaxf(-1.5, v));
    mkfg.display->drawPixel(i+40,MKFG_DISPLAY_HEIGHT-25-v*16, WHITE);
    v = (gm_isReversedByUser?-1:1)*gm_getVelocityTargetAtProgMsOffset(curProgMs + i*15000 + 10000)* gm_velocityMultiplier; // * gm_velocityMultiplier;
    v = fminf(1.5, fmaxf(-1.5, v));
    mkfg.display->drawPixel(i+40,MKFG_DISPLAY_HEIGHT-25-v*16, WHITE);
  }
  // --
  mkfg.display->display(); // Show the display buffer on the screen. You MUST call display() after drawing commands to make them visible on screen!
}