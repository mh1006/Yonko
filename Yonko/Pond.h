#include <Adafruit_NeoPixel.h>

// --- Pin & LED Definitions ---
const int fsrPin = A0;      
const int neoPixelPin = A3; 
const int NUM_LEDS = 7;    

Adafruit_NeoPixel pondLeds(NUM_LEDS, neoPixelPin, NEO_GRB + NEO_KHZ800);

// --- State Variables ---
int pondPressure = 0; 
bool isCupPlaced = false; 
int storedWeight = 0; // Records the stable weight of the cup
unsigned long cupPlacedTime = 0;
bool pendingWeightCheck = false; // Flag to trigger weight comparison after stabilizing

unsigned long lastFsrCheckTime = 0;

float waterLevel = NUM_LEDS; 
unsigned long lastEvaporationTime = 0;
const unsigned long EVAPORATION_RATE = 5000; 
bool needsLedUpdate = true; 

void setupPond() {
  pondLeds.begin();
  pondLeds.setBrightness(150); 
}

void updatePond() {
  unsigned long currentTime = millis();
  
  // -------------------------
  // 1. Natural water evaporation over time
  // -------------------------
  if (currentTime - lastEvaporationTime >= EVAPORATION_RATE) {
    if (waterLevel > 0) {
      waterLevel -= 0.5; 
      needsLedUpdate = true; 
    }
    lastEvaporationTime = currentTime;
  }

  // -------------------------
  // 2. Detect drinking (Weight comparison logic)
  // -------------------------
  if (currentTime - lastFsrCheckTime >= 100) {
    pondPressure = analogRead(fsrPin);
    
    // Condition A: Cup is PLACED on the coaster (Pressure > 200)
    if (pondPressure > 50) {
      if (!isCupPlaced) {
        // The exact moment the cup is placed back down
        isCupPlaced = true;
        cupPlacedTime = currentTime; // Start the stabilization timer
        pendingWeightCheck = true;   // Flag that we need to check the weight soon
      } 
      // Wait for 1 second (1000ms) for the water/cup to stabilize
      else if (pendingWeightCheck && (currentTime - cupPlacedTime > 1000)) {
        int newWeight = pondPressure;
        
        // Check if the new weight is significantly lighter than the stored weight (Threshold: 30)
        // Also ensure storedWeight > 0 to prevent triggering on the very first startup
        if (storedWeight > 0 && newWeight < (storedWeight - 30)) {
          waterLevel += 4.0; // Refill 4 units of water
          if (waterLevel > NUM_LEDS) {
            waterLevel = NUM_LEDS; 
          }
          needsLedUpdate = true;
        }
        
        // Update the stored weight to the new weight for the next round
        storedWeight = newWeight;
        pendingWeightCheck = false; // Check complete, wait for the next lift
      }
    }
    // Condition B: Cup is LIFTED from the coaster (Pressure < 200)
    else if (pondPressure < 30) {
      isCupPlaced = false;
      pendingWeightCheck = false; // Cancel any pending checks if lifted too quickly
    }
    
    lastFsrCheckTime = currentTime;
  }

  // -------------------------
  // 3. Update WS2812 blue light display 
  // -------------------------
  if (needsLedUpdate) {
    pondLeds.clear();
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < (int)waterLevel) {
        pondLeds.setPixelColor(i, pondLeds.Color(0, 0, 255));
      } else if (i == (int)waterLevel && (waterLevel - (int)waterLevel) > 0) {
        pondLeds.setPixelColor(i, pondLeds.Color(0, 0, 50));
      }
    }
    pondLeds.show();
    needsLedUpdate = false; 
  }
}