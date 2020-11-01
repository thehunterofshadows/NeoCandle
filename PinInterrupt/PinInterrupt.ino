// candle for Adafruit NeoPixel
// 8 pixel version
// by Tim Bartlett, December 2013

#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>
#define PIN 4
#define PIXELS 9

//setup using button to wake
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif


int counter = 0;
volatile boolean f_wdt = 1;

// color variables: mix RGB (0-255) for desired yellow
int redPx = 255;
int grnHigh = 100;
int bluePx = 10;

// animation time variables, with recommendations
int burnDepth = 6; //how much green dips below grnHigh for normal burn - 
int flutterDepth = 20; //maximum dip for flutter
int cycleTime = 120; //duration of one dip in milliseconds

// pay no attention to that man behind the curtain
int fDelay;
int fRep;
int flickerDepth;
int burnDelay;
int burnLow;
int flickDelay;
int flickLow;
int flutDelay;
int flutLow;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  flickerDepth = (burnDepth + flutterDepth) / 2.4;
  burnLow = grnHigh - burnDepth;
  burnDelay = (cycleTime / 2) / burnDepth;
  flickLow = grnHigh - flickerDepth;
  flickDelay = (cycleTime / 2) / flickerDepth;
  flutLow = grnHigh - flutterDepth;
  flutDelay = ((cycleTime / 2) / flutterDepth);
  //setup_watchdog(8); // approximately 4 seconds sleep  
  strip.begin();
  strip.show();

  //setup pin1 to be the intrupt
  pinMode(1,INPUT);
  
  sbi(GIMSK,PCIE); // Turn on Pin Change interrupt
  sbi(PCMSK,PCINT1); // Which pins are affected by the interrupt
}

void loop() {
  burn(2);
  
  //after running for a bit goto sleep until button pushed
  strip.fill(strip.Color(0,0,0));
  strip.show();
  system_sleep();

}


// basic fire funciton - not called in main loop
void fire(int grnLow) {
  for (int grnPx = grnHigh; grnPx > grnLow; grnPx--) {
    int halfGrn = grnHigh - ((grnHigh - grnPx) / 2);
    int darkGrn = grnPx - 70;
    darkGrn = constrain(darkGrn, 5, 255);
    strip.setPixelColor(0, redPx-180, darkGrn, 0);
    strip.setPixelColor(1, redPx-180, darkGrn, 0);
    strip.setPixelColor(2, redPx-120, grnPx-50, bluePx-5);
    strip.setPixelColor(3, redPx-60, grnPx-35, bluePx-2);
    strip.setPixelColor(4, redPx, grnPx, bluePx);
    strip.setPixelColor(5, redPx, grnPx, bluePx);
    strip.setPixelColor(6, redPx, halfGrn, bluePx);
    strip.setPixelColor(7, redPx, grnHigh, bluePx);
    strip.setPixelColor(8, redPx, grnHigh, bluePx);
    strip.show();
    delay(fDelay);
  }  
  for (int grnPx = grnLow; grnPx < grnHigh; grnPx++) {
    int halfGrn = grnHigh - ((grnHigh - grnPx) / 2);
    int darkGrn = grnPx-70;
    darkGrn = constrain(darkGrn, 5, 255);
    strip.setPixelColor(0, redPx-180, darkGrn, 0);
    strip.setPixelColor(1, redPx-180, darkGrn, 0);
    strip.setPixelColor(2, redPx-120, grnPx-50, bluePx-5);
    strip.setPixelColor(3, redPx-60, grnPx-35, bluePx-2);
    strip.setPixelColor(4, redPx, grnPx, bluePx);
    strip.setPixelColor(5, redPx, grnPx, bluePx);
    strip.setPixelColor(6, redPx, halfGrn, bluePx);
    strip.setPixelColor(7, redPx, grnHigh, bluePx);
    strip.setPixelColor(8, redPx, grnHigh, bluePx);
    strip.show();
    delay(fDelay);
  }
}

// fire animation
void on(int f) {
  fRep = f * 1000;
  int grnPx = grnHigh - 6;
    strip.setPixelColor(0, redPx-180, grnPx-70, 0);
    strip.setPixelColor(1, redPx-180, grnPx-70, 0);
    strip.setPixelColor(2, redPx-120, grnPx-50, bluePx-5);
    strip.setPixelColor(3, redPx-60, grnPx-35, bluePx-2);
    strip.setPixelColor(4, redPx, grnPx, bluePx);
    strip.setPixelColor(5, redPx, grnPx, bluePx);
    strip.setPixelColor(6, redPx, grnPx, bluePx);
    strip.setPixelColor(7, redPx, grnHigh, bluePx);
    strip.setPixelColor(8, redPx, grnHigh, bluePx);
  strip.show();
  delay(fRep);
}

void burn(int f) {
  fRep = f * 8;
  fDelay = burnDelay;
  for (int var = 0; var < fRep; var++) {
    fire(burnLow);
  }  
}

void flicker(int f) {
  fRep = f * 8;
  fDelay = burnDelay;
  fire(burnLow);
  fDelay = flickDelay;
  for (int var = 0; var < fRep; var++) {
    fire(flickLow);
  }
  fDelay = burnDelay;
  fire(burnLow);
  fire(burnLow);
  fire(burnLow);
}

void flutter(int f) {
  fRep = f * 8;  
  fDelay = burnDelay;
  fire(burnLow);
  fDelay = flickDelay;
  fire(flickLow);
  fDelay = flutDelay;
  for (int var = 0; var < fRep; var++) {
    fire(flutLow);
  }
  fDelay = flickDelay;
  fire(flickLow);
  fire(flickLow);
  fDelay = burnDelay;
  fire(burnLow);
  fire(burnLow);
}




void system_sleep() {
  cbi(ADCSRA,ADEN); // Switch Analog to Digital converter OFF
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Set sleep mode
  sleep_mode(); // System sleeps here
  sbi(ADCSRA,ADEN);  // Switch Analog to Digital converter ON
}

ISR(PCINT0_vect) {
}
