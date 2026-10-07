// Keyes KY-040 Pin Definitions
const int clkPin = 2;  
const int dtPin = 3;   
const int swPin = 4;   

int musicVolume = 15;  
bool isPlaying = true; 

// Variables to store previous states
int lastClkState;
bool lastBtnState = HIGH; 

void setupMushroom() {
  // Serial.begin is already initialized in main, no need to call it here
  
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(swPin, INPUT_PULLUP);
  
  lastClkState = digitalRead(clkPin);
}

void updateMushroom() {
  // -------------------------
  // 1. Detect Button Press (Play / Pause)
  // -------------------------
  bool currentBtnState = digitalRead(swPin);
  
  if (lastBtnState == HIGH && currentBtnState == LOW) {
    isPlaying = !isPlaying;     
    delay(50); // Debounce
  }
  lastBtnState = currentBtnState;

  // -------------------------
  // 2. Detect Rotation (Adjust Volume)
  // -------------------------
  int currentClkState = digitalRead(clkPin);
  
  if (currentClkState != lastClkState && currentClkState == HIGH) {
    if (digitalRead(dtPin) != currentClkState) {
      musicVolume++; // Counter-clockwise: Volume down
    } else {
      musicVolume--; // Clockwise: Volume up
    }   
    musicVolume = constrain(musicVolume, 0, 30);
  }
  lastClkState = currentClkState;
}