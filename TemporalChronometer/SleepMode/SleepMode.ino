/*
  Button
 
 Turns on and off a light emitting diode(LED) connected to digital  
 pin 13, when pressing a pushbutton attached to pin 2. 
 
 
 The circuit:
 * LED attached from pin 13 to ground 
 * pushbutton attached to pin 2 from +5V
 * 10K resistor attached to pin 2 from ground
 
 * Note: on most Arduinos there is already an LED on the board
 attached to pin 13.
 
 
 created 2005
 by DojoDave <http://www.0j0.org>
 modified 30 Aug 2011
 by Tom Igoe
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Button
 */

// constants won't change. They're used here to 
// set pin numbers:
#include <Adafruit_NeoPixel.h>
#include <JeelLib.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 0, NEO_GRB + NEO_KHZ800);

// Button pin information
const int buttonPin = 3;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

#define BODS 7                   //BOD Sleep bit in MCUCR
#define BODSE 2                  //BOD Sleep enable bit in MCUCR
uint8_t mcucr1, mcucr2;

void setup() {
  // initialize the LED pin as an output:
  // initialize the pushbutton pin as an input:
  strip.begin();
    strip.setPixelColor(2, strip.Color(0, 255, 0));
  strip.show(); // Initialize all pixels to 'off'
  pinMode(buttonPin, INPUT);
}

void sleepNow() {
  strip.setPixelColor(1, strip.Color(255, 255, 0));
  strip.show();
  GIMSK |= _BV(INT0);                       //enable INT1
  MCUCR &= ~(_BV(ISC01) | _BV(ISC00));      //INT0 on low level
  ACSR |= _BV(ACD);                         //disable the analog comparator
  ADCSRA &= ~_BV(ADEN);                     //disable ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  //turn off the brown-out detector.
  //must have an ATtiny45 or ATtiny85 rev C or later for software to be able to disable the BOD.
  //current while sleeping will be <0.5uA if BOD is disabled, <25uA if not.
  cli();
  mcucr1 = MCUCR | _BV(BODS) | _BV(BODSE);  //turn off the brown-out detector
  mcucr2 = mcucr1 & ~_BV(BODSE);
  MCUCR = mcucr1;
  MCUCR = mcucr2;
  sei();                         //ensure interrupts enabled so we can wake up again
  sleep_cpu();                   //go to sleep
  cli();                         //wake up here, disable interrupts
  GIMSK = 0x00;                  //disable INT0
  sleep_disable();
  sei();                         //enable interrupts again (but INT0 is disabled from above)
  }ISR(INT0_vect) {}                  //nothing to actually do here, the interrupt just wakes us up!

void loop(){
  // read the state of the pushbutton value:
    strip.setPixelColor(1, strip.Color(0, 0, 255));
    strip.show();
  delay(1000);
  if( digitalRead(buttonPin) == LOW) {
    sleepNow();
  }
}

