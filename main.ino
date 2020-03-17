// CPSC 581 Project 2: Solving an Everyday Problem with Arduino
// Sam Wong & Katie Tieu

#include <CapacitiveSensor.h>
#include <FastLED.h>
#include "pitches.h"

/*
   CapitiveSense Library Demo Sketch
   Paul Badger 2008
   Uses a high value resistor e.g. 10M between send pin and receive pin
   Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
   Receive pin is the sensor pin - try different amounts of foil/metal on this pin
*/
CapacitiveSensor cs_3_2 = CapacitiveSensor(3, 2);       // 330-ohmn resistor between pins 4 & 2, pin 2 is sensor pin, add a wire and or foil if desired
int treshold = 150; // the minimum value for turning the vibrations and buzzer on
int vibPin1 = 13;
int vibPin2 = 12;
int vibPin3 = 11;
int buzzerPin1 = 9;
bool alarmStarted = false;  // to run through the LEDs before actual alarm part starts

// FastLED variables
#define DATA_PIN 7
#define NUM_LEDS 17
CRGB leds[NUM_LEDS];

// notes in the buzzer melody:
// Blinding Lights by The Weeknd
int melody[] = {
  NOTE_F4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, \
  NOTE_F4, NOTE_F4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, \
  NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_DS4, \
  NOTE_AS4, NOTE_G4, NOTE_F4, NOTE_DS4, NOTE_F4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  2.45, 2.99999, 7, 7, 4, 4, 3, \
  2.45, 2.99999, 7, 7, 4, 4, 3, \
  7, 4, 4, 3, \
  7, 4, 4, 4, 2
};


void setup()
{
  cs_3_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(9600);
  
  pinMode(vibPin1, OUTPUT);
  pinMode(vibPin2, OUTPUT);
  pinMode(vibPin3, OUTPUT);
  pinMode(buzzerPin1, OUTPUT);
 
  FastLED.addLeds<WS2812B, DATA_PIN, RGB>(leds, NUM_LEDS);
}

void loop()
{
  // after lights go off to show alarm is set
  if (alarmStarted == true) {
    long start = millis();
    long sensorValue =  cs_3_2.capacitiveSensor(30);
    Serial.print(millis() - start);        // check on performance in milliseconds
    Serial.print("\t");                    // tab character for debug windown spacing
    Serial.print(sensorValue);                  // print sensor output 1
    Serial.print("\n");
    if (sensorValue > treshold) {
      // turn on vibrating disks
      digitalWrite(vibPin1, HIGH);
      digitalWrite(vibPin2, HIGH);
      digitalWrite(vibPin3, HIGH);
  
        // play melody
        for (int thisNote = 0; thisNote < 23; thisNote++) {
            // to calculate the note duration, take one second divided by the note type.
            //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
            int noteDuration = 1000 / noteDurations[thisNote];
            tone(buzzerPin1, melody[thisNote], noteDuration);
        
            // to distinguish the notes, set a minimum time between them.
            // the note's duration + 30% seems to work well:
            int pauseBetweenNotes = noteDuration * 1.30;
            delay(pauseBetweenNotes);
            // stop the tone playing:
            noTone(buzzerPin1);
          }
    }
    else {
      // stop vibrations
      digitalWrite(vibPin1, LOW);
      digitalWrite(vibPin2, LOW);
      digitalWrite(vibPin3, LOW);
    }
    delay(10);                             // arbitrary delay to limit data to serial port
  }

  // flash lights to show alarm is set
  else {
    delay(3000);  // short delay before lights start

    // FastLED
    // Go through each LED one by one
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Green;    // actually red for some reason
    
      FastLED.show();
      delay(50);
      // Now turn the LED off, then pause
      leds[i] = CRGB::Black;
      FastLED.show();
      delay(50);
    }
    delay(500);

    // Blink all LEDs at the same time
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;  // actually green for some reason
      FastLED.show();
    }
    delay(300);   // keep LEDs on
    for (int i = 0; i < NUM_LEDS; i++) {
      // Now turn the LED off, then pause
      leds[i] = CRGB::Black;
      FastLED.show();
    }

    delay(500);   // wait before next blink

    // Blink all LEDs at the same time
    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB::Red;  // actually flashes green for some reason
      FastLED.show();
    }
    delay(1000);    // keep LEDs on
    for (int i = 0; i < NUM_LEDS; i++) {
      // Now turn the LED off, then pause
      leds[i] = CRGB::Black;
      FastLED.show();
    }

    alarmStarted = true;
    
    delay(10000);   // 10 second delay before alarm can be triggered (to simulate sleep time)
  }
}
