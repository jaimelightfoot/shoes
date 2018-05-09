
#include "Adafruit_NeoPixel.h"
#ifdef __AVR__
#include <avr/power.h>
#endif

/*********** LED-related constants ***********/
#define HEEL_LED_PIN 0
#define HEEL_LED_NUM 17

#define TOE_LED_PIN 1
#define TOE_LED_NUM 5

Adafruit_NeoPixel toeLEDStrip = Adafruit_NeoPixel(HEEL_LED_NUM, HEEL_LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel heelLEDStrip = Adafruit_NeoPixel(TOE_LED_NUM, TOE_LED_PIN, NEO_GRB + NEO_KHZ800);
/*********************************************/

/********** Piezo-related constants **********/
#define PIEZO_PIN 2 // analog pins are numbered differently than the digital pins!

#define THRESHOLD 10
/*********************************************/

uint8_t glimmer_upper_limit = 100;
uint8_t glimmer_lower_limit = 50;
uint8_t middle_bright = 75;

#define GROUP_LOW 3
#define GROUP_HIGH 10


uint16_t brightness = 15;
uint16_t diff = 512;
/*************************/

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

void setup() {
  toeLEDStrip.begin();
  heelLEDStrip.begin();

  // Initialize pixels to off
  toeLEDStrip.show();
  heelLEDStrip.show();

  randomSeed(analogRead(PIEZO_PIN));

  interruptSetup();
}

void loop() {
  pinkToPurple();
  purpleToBlue();
  blueToPink();
}

bool decrease_flag = false;

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  uint8_t piezoADC = analogRead(PIEZO_PIN);

  if (THRESHOLD < piezoADC) { // if above threshold
    // if first time, jump up to 255
    if (!decrease_flag) {
      brightness = 255;
      decrease_flag = true;
    }
  }

  if (decrease_flag) {
    brightness -= 2;
  }
}

void interruptSetup()
{
  // initialize Timer1
  noInterrupts(); // disable all interrupts
  TCCR1 = 0;                  //stop the timer
  TCNT1 = 0;                  //zero the timer
  GTCCR = _BV(PSR1);          //reset the prescaler
  OCR1A = 243;                //set the compare value
  OCR1C = 243;
  TIMSK = _BV(OCIE1A);        //interrupt on Compare Match A
  //start timer, ctc mode, prescaler clk/16384
  TCCR1 = _BV(CTC1) | _BV(CS13) | _BV(CS11);
  sei();

  interrupts(); // enable all interrupts
}

void pinkToPurple()
{
  // pink (255, 0, 128) to purple (163, 73, 164)

  int i;
  int diff = 512;
  uint8_t r = 255;
  uint8_t g = 0;
  uint8_t b = 128;

  for (i = 0; i < diff; i++)
  {
    if (i % 5 == 0) {
      // time to adjust R and G
      r--;
    }
    if (i % 7 == 0) {
      // time to adjust R and G
      g++;
    }
    if (i % 14 == 0) {
      // time to adjust B
      b++;
    }

    uint8_t group = random(GROUP_LOW, GROUP_HIGH);
    for (uint16_t j = 0; j < toeLEDStrip.numPixels(); j++) {
      setToeLEDColorWithBrightness(j, r, g, b, group);
      if (j < heelLEDStrip.numPixels()) {
        setHeelLEDColorWithBrightness(j, r, g, b, group);
      }
    }
  }
}

void setToeLEDColorWithBrightness(int j, uint8_t r, uint8_t g, uint8_t b, uint8_t group) {
  if (brightness < middle_bright) {
    decrease_flag = false;
    if (j % group == 0) {
      brightness = random(glimmer_lower_limit, glimmer_upper_limit);
    } else {
      brightness = middle_bright;
    }
  }

  toeLEDStrip.setPixelColor(j, (brightness * r / 255) , (brightness * g / 255), (brightness * b / 255));
  toeLEDStrip.show();
}

void setHeelLEDColorWithBrightness(int j, uint8_t r, uint8_t g, uint8_t b, uint8_t group) {
  uint16_t brightness;
  if (brightness < middle_bright) {
    decrease_flag = false;
    if (j % group == 0) {
      brightness = random(glimmer_lower_limit, glimmer_upper_limit);
    } else {
      brightness = middle_bright;
    }
  }

  heelLEDStrip.setPixelColor(j, (brightness * r / 255) , (brightness * g / 255), (brightness * b / 255));
  heelLEDStrip.show();
}


void purpleToBlue()
{
  // purple (163, 73, 164) to blue (0, 0, 255)

  int i;
  int diff = 512;
  unsigned int r = 163;
  unsigned int g = 73;
  int b = 164;

  for (i = 0; i < diff; i++)
  {
    if (i % 3 == 0) {
      // time to adjust R
      if (r > 0) {
        r--;
      }
    }
    if (i % 7 == 0) {
      // time to adjust G
      if (g > 0) {
        g--;
      }
    }
    if (i % 6 == 0) {
      // time to adjust B
      b++;
    }

    uint8_t group = random(GROUP_LOW, GROUP_HIGH);

    for (uint16_t j = 0; j < toeLEDStrip.numPixels(); j++) {
      setToeLEDColorWithBrightness(j, r, g, b, group);
      if (j < heelLEDStrip.numPixels()) {
        setHeelLEDColorWithBrightness(j, r, g, b, group);
      }
    }
  }
}

void blueToPink()
{
  // blue (0, 0, 255) to pink (255, 0, 128)

  int i;
  int diff = 512;
  int r = 0;
  int g = 0;
  int b = 255;

  for (i = 0; i < diff; i++)
  {
    if (i % 2 == 0) {
      // time to adjust R
      if (r < 255) {
        r++;
      }
    }
    if (i % 4 == 0) {
      // time to adjust B
      b--;
    }

    uint8_t group = random(GROUP_LOW, GROUP_HIGH);

    for (uint16_t j = 0; j < toeLEDStrip.numPixels(); j++) {
      setToeLEDColorWithBrightness(j, r, g, b, group);
      if (j < heelLEDStrip.numPixels()) {
        setHeelLEDColorWithBrightness(j, r, g, b, group);
      }
    }
  }
}

