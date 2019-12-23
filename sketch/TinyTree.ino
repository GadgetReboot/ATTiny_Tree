/*
 * ATTiny Tree December 2019
 * For ATTiny85 on a Christmas Tree shaped PCB with 8 Neopixels
 * Has X/Y PWM outputs for displaying a Christmas Tree on oscilloscope
 * ATTiny85 is set for 8MHz internal osc to save current
 * 
 *	Oscilloscope Christmas Tree based on a project
 * 	Created: Dec 10, 2011    Author: John M. De Cristofaro
 *  
 *  Neopixel code hacked from Adafruit Neopixel examples
 *  
 *  Everything smushed together by Gadget Reboot
 *  Hopefully this will be overhauled at some point.  
 *  Maybe by Christmas 2020
 *  
*/


#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> // Required if running at 16 MHz 
#endif

#define pixelPin        3   // neopixel output
#define ledButton       4   // LED button input
#define xyButton        2   // xy mode button input
#define debounce     8000   // delay for crude button debounce
#define ledNextDelay 1500   // delay for stepping through leds
#define ledFadeDelay  400   // delay for fading between led levels
#define NUMPIXELS       8   // 8 on-board pixels

Adafruit_NeoPixel pixels(NUMPIXELS, pixelPin, NEO_GRB + NEO_KHZ800);


// for drawing a tree on a scope in x/y mode
#define TRACE_DELAY	 2000   // used when plotting x/y points
#define NUM_POINTS	   19   // tree drawing has 19 x/y points
#define X               0   // scope channel 1 (X) to ATTiny85 pin 5
#define Y               1   // scope channel 2 (y) to ATTiny85 pin 6

// x/y coordinate points
unsigned char x_points[NUM_POINTS] = {	110, 110, 50, 80, 65, 95, 80, 110, 95, 125, 155, 140, 170, 155, 185, 170, 200, 140, 140 };
unsigned char y_points[NUM_POINTS] = {	15, 35, 35, 85, 85, 135, 135, 185, 185, 235, 185, 185, 135, 135, 85, 85, 35, 35, 15 };

// control the current display mode between scope or on-board pixel patterns
enum displayOptions { scopetree, leds1, leds2, leds3, leds4};
displayOptions displayPattern = leds1;


void setup() {
  // when using 16 MHz ATTiny85
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  // init neopixels
  pixels.begin(); 
  pixels.setBrightness(10);
  pixels.show();   

  // init scope outputs and button inputs
  pinMode(X, OUTPUT);
  pinMode(Y, OUTPUT);
  pinMode(ledButton, INPUT_PULLUP);
  pinMode(xyButton, INPUT_PULLUP);

  // timer/counter setup copied over from a different project
  // if it works, don't mess with it!

  TCCR0A = (	1 << COM0A1 | 0 << COM0A0 |		
              1 << COM0B1 | 0 << COM0B0 |		
              1 << WGM01  | 1 << WGM00);		

  TCCR0B = (	0 << FOC0A | 0 << FOC0B |		
              0 << WGM02 |			
              0 << CS02	 | 0 << CS01 |		
              1 << CS00 );
}


void loop() {

  /*** MODE 1 ***/
  // draw a tree on oscilloscope in xy mode
  while (displayPattern == scopetree)  {
    for (byte t = 0; t < NUM_POINTS; t++) {    // run through the points in x & y
      analogWrite(X, x_points[t]);
      analogWrite(Y, y_points[t]);
      delayMicroseconds(TRACE_DELAY);          // wait some time before next coordinate advance
    }
    if (digitalRead(ledButton) == LOW) {       // horrible HORRIBLE button handling (for now)
      delay(debounce);
      displayPattern = leds1;
      break;
    }
  }

   /*** MODE 2 ***/
  // fill in all pixels one by one for a given color, then repeat with new color
  while (displayPattern == leds1)  {
    for (int i = 0; i < NUMPIXELS; i++) {
      if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
      pixels.setPixelColor(i, pixels.Color(25, 0, 0));
      pixels.show();
      for (int i = 0; i < 10; i++) {
        if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
        delay(ledNextDelay);
        if (digitalRead(xyButton) == LOW) {
          pixels.clear();
          pixels.show();
          displayPattern = scopetree;
        }
        if (digitalRead(ledButton) == LOW) {
          delay(debounce);
          displayPattern = leds2;
        }
      }
    }

    for (int i = 0; i < NUMPIXELS; i++) {
      if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
      pixels.setPixelColor(i, pixels.Color(0, 25, 0));
      pixels.show();
      for (int i = 0; i < 10; i++) {
        if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
        delay(ledNextDelay);
        if (digitalRead(xyButton) == LOW) {
          pixels.clear();
          pixels.show();
          displayPattern = scopetree;
        }
        if (digitalRead(ledButton) == LOW) {
          delay(debounce);
          displayPattern = leds2;
        }
      }
    }

    for (int i = 0; i < NUMPIXELS; i++) {
      if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
      pixels.setPixelColor(i, pixels.Color(25, 25, 25));
      pixels.show();
      for (int i = 0; i < 10; i++) {
        if ((displayPattern == leds2) | (displayPattern == scopetree)) break;
        delay(ledNextDelay);
        if (digitalRead(xyButton) == LOW) {
          pixels.clear();
          pixels.show();
          displayPattern = scopetree;
        }
        if (digitalRead(ledButton) == LOW) {
          delay(debounce);
          displayPattern = leds2;
        }
      }
    }

  }

  /*** MODE 3 ***/
  // fade all pixels up and down
  while (displayPattern == leds2)  {

    for (int i = 0; i < 170; i++) { 
      if ((displayPattern == leds3) | (displayPattern == scopetree)) break;
      pixels.fill(pixels.Color(i, i, i));
      pixels.show();
      delay(ledFadeDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds3;
      }
    }

    for (int i = 170; i >= 0; i--) { 
      if ((displayPattern == leds3) | (displayPattern == scopetree)) break;
      pixels.fill(pixels.Color(i, i, i));
      pixels.show();
      delay(ledFadeDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds3;
      }
    }
  }

  /*** MODE 4 ***/
  // light up all pixels with same color and repeat with different color
  while (displayPattern == leds3)  {
    pixels.fill(pixels.Color(25, 0, 0));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds4) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds4;
      }
    }

    pixels.fill(pixels.Color(0, 25, 0));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds4) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds4;
      }
    }

    pixels.fill(pixels.Color(0, 25, 25));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds4) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds4;
      }
    }

    pixels.fill(pixels.Color(0, 0, 25));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds4) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds4;
      }
    }

    pixels.fill(pixels.Color(25, 25, 25));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds4) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds4;
      }
    }
  }

  /*** MODE 5 ***/
  // light individual random pixels with random colors
  while (displayPattern == leds4)  {
    pixels.setPixelColor(random(0, NUMPIXELS), pixels.Color(random(0, 255), random(0, 255), random(0, 255)));
    pixels.show();
    for (int i = 0; i < 10; i++) {
      if ((displayPattern == leds1) | (displayPattern == scopetree)) break;
      delay(ledNextDelay);
      if (digitalRead(xyButton) == LOW) {
        pixels.clear();
        pixels.show();
        displayPattern = scopetree;
      }
      if (digitalRead(ledButton) == LOW) {
        delay(debounce);
        displayPattern = leds1;
      }
    }
  }

}
