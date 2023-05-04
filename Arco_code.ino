#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include "NewPing.h"
#include "FastLED.h"

// Define the pins used for the hardware
#define TRIGGER_PIN 12
#define ECHO_PIN 13
#define LED_PIN 10
#define NUM_LEDS 150

// Define the distance threshold for proximity detection (in cm)
#define PROXIMITY_THRESHOLD 20

// Set up the mini mp3 df player
SoftwareSerial mySoftwareSerial(5, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

// Set up the LED strip
CRGB leds[NUM_LEDS];

// Set up the ultrasonic sensor
NewPing sonar(TRIGGER_PIN, ECHO_PIN);
long distance;

// Set up a timer for 10 seconds
unsigned long timerStart;

// Define the stage
int stage = 1;

void setup() {

  mySoftwareSerial.begin(9600);
  Serial.begin(115200);

  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    while(true);
  }

  myDFPlayer.volume(20);  //Set volume value. From 0 to 30

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);

  // Initialize the timer
  timerStart = 0;
}

void loop() {
  if(stage == 1) {
    // Measure the distance using the ultrasonic sensor
    distance = sonar.ping_cm();
    // Print distance in cm to Serial Monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Check if the distance is below the proximity threshold
    if (distance < PROXIMITY_THRESHOLD) {
      // Enter stage 2
      stage = 2;
      Serial.println("Entering Stage 2");
    }
  }
  else if(stage == 2) {
    // Check if the timer has not started yet
    if (timerStart == 0) {
      // Play the desired song using the mini mp3 df player
      myDFPlayer.play(1);
      Serial.println("Playing music");
      // Fade in rainbow colors on the LED strip
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(i * 255 / NUM_LEDS, 255, 255);
        FastLED.show();
        delay(10);
      }
      // Start the timer
      timerStart = millis();
    }

    // Check if the timer has elapsed (10 seconds)
    if (millis() - timerStart > 10000) {
      // Stop the LEDs and the music
      for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::Black;
      }
      FastLED.show();
      myDFPlayer.stop();
      Serial.println("Stopping music");
      // Reset the timer and enter stage 1
      timerStart = 0;
      stage = 1;
      Serial.println("Entering Stage 1");
    }
  }
}
