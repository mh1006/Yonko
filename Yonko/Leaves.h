#include <Adafruit_NeoPixel.h>

// --- Leaves (Single Pin Daisy-Chain on A1 with Fade-out Animation) ---
const int leafBtnPins[2] = {7, 9}; 
const int leafStripPin = A1; 

const int LEDS_PER_LEAF = 1; 
const int TOTAL_LEDS = LEDS_PER_LEAF * 2; 

Adafruit_NeoPixel leavesStrip(TOTAL_LEDS, leafStripPin, NEO_GRB + NEO_KHZ800);

// --- Constants & Variables ---
const unsigned long FIFTEEN_MINUTES = 5000UL; // 5000ms (5 seconds) for testing

int leafQueue[2]; 
int queueHead = 0;
int queueTail = 0;
int activeLeaves = 0; 
bool isLeafDown[2] = {false, false};
bool lastBtnStates[2] = {HIGH, HIGH};

unsigned long lastLeafRiseTime = 0;
bool isTimerActive = false; 

unsigned long leafTimerStart[2] = {0, 0}; 

// Dynamically update colors and brightness for both halves of the strip
void updateStripColors() {
  unsigned long now = millis();
  
  for (int idx = 0; idx < 2; idx++) {
    int startIndex = idx * LEDS_PER_LEAF; 
    int endIndex = startIndex + LEDS_PER_LEAF;
    
    if (isLeafDown[idx]) {
      unsigned long elapsed = now - leafTimerStart[idx];
      
      if (elapsed >= FIFTEEN_MINUTES) {
        // Time's up, turn off the LEDs for this section
        for (int i = startIndex; i < endIndex; i++) {
          leavesStrip.setPixelColor(i, 0); 
        }
      } else {
        // Calculate time progress (0.0 to 1.0)
        float progress = (float)elapsed / FIFTEEN_MINUTES;
        
        // 🌟 Calculate brightness fade (1.0 at start -> 0.0 at the end)
        float fade = 1.0 - progress; 
        
        // Base color transition (Green -> Yellow)
        float base_r = progress * 255;          
        float base_g = 255 - (progress * 135);  
        
        // Apply the fade multiplier to gradually dim the LEDs
        int r = base_r * fade;
        int g = base_g * fade;
        int b = 0;                       
        
        // Push the calculated color and brightness to the LEDs
        for (int i = startIndex; i < endIndex; i++) {
          leavesStrip.setPixelColor(i, leavesStrip.Color(r, g, b));
        }
      }
    } else {
      // If the leaf is UP, make sure this section is dark
      for (int i = startIndex; i < endIndex; i++) {
        leavesStrip.setPixelColor(i, 0);
      }
    }
  }
  leavesStrip.show(); 
}

// Turn leaf LEDs ON at full Green brightness when first pressed
void setLeafStrip(int leafIndex, bool turnOn) {
  int startIndex = leafIndex * LEDS_PER_LEAF;
  int endIndex = startIndex + LEDS_PER_LEAF;
  for (int i = startIndex; i < endIndex; i++) {
    if (turnOn) {
      leavesStrip.setPixelColor(i, leavesStrip.Color(0, 255, 0)); // Full Green
    } else {
      leavesStrip.setPixelColor(i, 0); // Turn off
    }
  }
  leavesStrip.show();
}

void setupLeaves() {
  for (int i = 0; i < 2; i++) {
    pinMode(leafBtnPins[i], INPUT_PULLUP);
  }
  leavesStrip.begin();
  leavesStrip.show(); 
}

void updateLeaves() {
  // 1. Detect Button Press
  for (int i = 0; i < 2; i++) {
    bool currentState = digitalRead(leafBtnPins[i]);
    
    if (lastBtnStates[i] == HIGH && currentState == LOW) {
      delay(50); // Debounce
      
      if (!isLeafDown[i] && activeLeaves < 2) {
        isLeafDown[i] = true;
        leafTimerStart[i] = millis(); 
        
        // Turn on instantly to full brightness
        setLeafStrip(i, true); 
        
        leafQueue[queueTail] = i; 
        queueTail = (queueTail + 1) % 2;
        
        if (activeLeaves == 0) {
          lastLeafRiseTime = millis();
        }
        activeLeaves++;
      }
    }
    lastBtnStates[i] = currentState;
  }

  // 2. Timer Logic 
  if (activeLeaves > 0) {
    isTimerActive = true; 
    int oldestLeaf = leafQueue[queueHead];
    
    if (millis() - lastLeafRiseTime >= FIFTEEN_MINUTES) {
      queueHead = (queueHead + 1) % 2;
      setLeafStrip(oldestLeaf, false);
      isLeafDown[oldestLeaf] = false;
      
      activeLeaves--;
      lastLeafRiseTime = millis(); 
    }
  } else {
    isTimerActive = false; 
  }

  // 3. Keep updating the fading animation smoothly
  updateStripColors();
}