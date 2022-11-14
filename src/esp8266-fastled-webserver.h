#include <FastLED.h>

void nextPattern();
void rainbow();
void white();
void black();
void cylon();
void rainbowWithGlitter();
void addGlitter( fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void juggle();
void Fire2012();
void discostrobe();
void discoWorker(uint8_t dashperiod, uint8_t dashwidth, int8_t dashmotionspeed, uint8_t stroberepeats, uint8_t huedelta);
static void drawRainbowDashes(uint8_t startpos, uint16_t lastpos, uint8_t period, uint8_t width, uint8_t huestart, uint8_t huedelta, uint8_t saturation, uint8_t value);
static void delayToSyncFrameRate( uint8_t framesPerSecond);