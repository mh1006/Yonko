#include <Servo.h>

// --- Pin Definitions for Leaves ---
// 4 buttons for the 4 leaves
const int leafBtnPins[4] = {7, 8, 12, 13}; 
// 4 servos for the 4 leaves (Ensure you connect these to PWM capable or digital pins)
const int leafServoPins[4] = {9, 10, 11, A1}; 

Servo leafServos[4];

// --- Constants & Variables ---
const int LEAF_UP_ANGLE = 90;
const int LEAF_DOWN_ANGLE = 170;

// 15 minutes in milliseconds (15 * 60 * 1000)
// Tip: Change this to 5000 (5 seconds) while testing!
const unsigned long FIFTEEN_MINUTES = 900000UL; 

// Queue system to track the order of button presses
int leafQueue[4]; 
int queueHead = 0;
int queueTail = 0;
int activeLeaves = 0; 
bool isLeafDown[4] = {false, false, false, false};
bool lastBtnStates[4] = {HIGH, HIGH, HIGH, HIGH};

unsigned long lastLeafRiseTime = 0;

// Global variable to share timer status with Flower.h
bool isTimerActive = false; 

void setupLeaves() {
  for (int i = 0; i < 4; i++) {
    pinMode(leafBtnPins[i], INPUT_PULLUP);
    leafServos[i].attach(leafServoPins[i]);
    leafServos[i].write(LEAF_UP_ANGLE); // Initial position: Leaves are up
  }
}

void updateLeaves() {
  // -------------------------
  // 1. Check for Button Presses
  // -------------------------
  for (int i = 0; i < 4; i++) {
    bool currentState = digitalRead(leafBtnPins[i]);
    
    // If button is pressed (LOW) and wasn't pressed before
    if (lastBtnStates[i] == HIGH && currentState == LOW) {
      delay(50); // Simple debounce
      
      // If the leaf is not already down, drop it and add to queue
      if (!isLeafDown[i] && activeLeaves < 4) {
        isLeafDown[i] = true;
        leafServos[i].write(LEAF_DOWN_ANGLE); // Leaf drops
        
        leafQueue[queueTail] = i; // Save which leaf was pressed
        queueTail = (queueTail + 1) % 4;
        
        // If this is the first leaf to drop, start the timer
        if (activeLeaves == 0) {
          lastLeafRiseTime = millis();
        }
        activeLeaves++;
      }
    }
    lastBtnStates[i] = currentState;
  }

  // -------------------------
  // 2. Timer Logic (Raise Leaves sequentially)
  // -------------------------
  if (activeLeaves > 0) {
    isTimerActive = true; // Timer is ongoing, leaves haven't fully risen
    
    // Check if 15 minutes have passed since the last leaf was raised/dropped
    if (millis() - lastLeafRiseTime >= FIFTEEN_MINUTES) {
      // Get the oldest leaf from the queue
      int leafToRise = leafQueue[queueHead];
      queueHead = (queueHead + 1) % 4;
      
      // Raise the leaf
      leafServos[leafToRise].write(LEAF_UP_ANGLE);
      isLeafDown[leafToRise] = false;
      
      activeLeaves--;
      lastLeafRiseTime = millis(); // Reset timer for the next leaf
    }
  } else {
    isTimerActive = false; // All leaves are up, timer is complete
  }
}