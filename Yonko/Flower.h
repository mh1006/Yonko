#include <Servo.h>

// Import the timer status from Leaves.h
extern bool isTimerActive; 

const int trigPin = 5;
const int echoPin = 6;
const int flowerServoPin = A2; // Assign a pin for the flower servo

Servo flowerServo;

int flowerDistance = 0; 
int lastDistance = 0;
unsigned long lastDistanceTime = 0;
unsigned long flowerClosedTime = 0;

// Configurable Constants 
const int FLOWER_OPEN_ANGLE = 90;
const int FLOWER_CLOSE_ANGLE = 10;
const int DETECT_DISTANCE = 30; // Distance in cm to trigger "presence"
const float SPEED_THRESHOLD = 0.5; // Change in distance per ms (adjust as needed)

bool isFlowerClosed = false;

void setupFlower() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  flowerServo.attach(flowerServoPin);
  flowerServo.write(FLOWER_OPEN_ANGLE); // Initial state: Flower is open
}

void updateFlower() {
  unsigned long currentTime = millis();
  
  // Read distance every 50ms to avoid blocking and get stable speed readings
  if (currentTime - lastDistanceTime >= 50) {
    long duration;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH, 30000); 
    flowerDistance = duration * 0.034 / 2;
    
    // Catch invalid 0 readings from timeout
    if (flowerDistance == 0) flowerDistance = 999; 

    // Calculate speed of approach
    float approachSpeed = (float)(lastDistance - flowerDistance) / (currentTime - lastDistanceTime);
    
    // Condition 1: Fast approach detected
    bool fastApproach = (approachSpeed > SPEED_THRESHOLD);
    
    // Condition 2: Timer is active AND an object is detected nearby (regardless of speed)
    bool timerTrigger = isTimerActive && (flowerDistance < DETECT_DISTANCE);

    // If either condition is met, close the flower
    if (fastApproach || timerTrigger) {
      if (!isFlowerClosed) {
        flowerServo.write(FLOWER_CLOSE_ANGLE);
        isFlowerClosed = true;
      }
      flowerClosedTime = currentTime; // Reset the "closed" timer
    }
    
    lastDistance = flowerDistance;
    lastDistanceTime = currentTime;
  }

  // Auto-open the flower if it has been closed for 3 seconds without new triggers
  if (isFlowerClosed && (currentTime - flowerClosedTime > 3000)) {
    flowerServo.write(FLOWER_OPEN_ANGLE);
    isFlowerClosed = false;
  }
}