// --- Leaves (Dual Button + Dual LED Simulation) ---
// 2 buttons to simulate 2 leaves (Recommended pins: D7, D9)
const int leafBtnPins[2] = {7, 9}; 
// 2 LEDs to simulate leaf status (Recommended pins: D8, D10)
const int leafLedPins[2] = {8, 10}; 

// --- Constants & Variables ---
// 5 seconds (5000 ms) for testing, change to 15 minutes (900000UL) for final exhibition
const unsigned long FIFTEEN_MINUTES = 5000UL; 

// Queue system to track the order of button presses
int leafQueue[2]; 
int queueHead = 0;
int queueTail = 0;
int activeLeaves = 0; 
bool isLeafDown[2] = {false, false};
bool lastBtnStates[2] = {HIGH, HIGH};

unsigned long lastLeafRiseTime = 0;

// Global variable read by Flower.h to share timer status
bool isTimerActive = false; 

void setupLeaves() {
  for (int i = 0; i < 2; i++) {
    pinMode(leafBtnPins[i], INPUT_PULLUP);
    pinMode(leafLedPins[i], OUTPUT);
    digitalWrite(leafLedPins[i], LOW); // Initial state: LED off (Leaf is up)
  }
}

void updateLeaves() {
  // -------------------------
  // 1. Detect Button Press
  // -------------------------
  for (int i = 0; i < 2; i++) {
    bool currentState = digitalRead(leafBtnPins[i]);
    
    // If button is pressed (LOW) and wasn't pressed previously
    if (lastBtnStates[i] == HIGH && currentState == LOW) {
      delay(50); // Simple debounce
      
      // If leaf is not already down (LED off), and active leaves are less than 2
      if (!isLeafDown[i] && activeLeaves < 2) {
        isLeafDown[i] = true;
        digitalWrite(leafLedPins[i], HIGH); // LED on: Leaf drops
        
        leafQueue[queueTail] = i; // Add this leaf to the queue
        queueTail = (queueTail + 1) % 2;
        
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
  // 2. Timer Logic (Sequentially turn off LEDs / raise leaves)
  // -------------------------
  if (activeLeaves > 0) {
    isTimerActive = true; // Timer is active, leaves haven't fully risen
    
    // Check if the set waiting time has been reached (currently 5 seconds)
    if (millis() - lastLeafRiseTime >= FIFTEEN_MINUTES) {
      // Get the oldest leaf from the queue (the one pressed first)
      int leafToRise = leafQueue[queueHead];
      queueHead = (queueHead + 1) % 2;
      
      // Raise the leaf (LED off)
      digitalWrite(leafLedPins[leafToRise], LOW);
      isLeafDown[leafToRise] = false;
      
      activeLeaves--;
      lastLeafRiseTime = millis(); // Reset timer for the next leaf
    }
  } else {
    isTimerActive = false; // All leaves are up, timer complete
  }
}