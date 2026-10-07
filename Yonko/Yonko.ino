#include "Flower.h"
#include "Leaves.h"
#include "Pond.h"
#include "Mushroom.h"

unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  setupMushroom();
  setupFlower();
  setupLeaves();
  setupPond();
  Serial.begin(9600);
}

void loop() {
  updateLeaves();  
  updateFlower();  
  updatePond();    
  updateMushroom(); 
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis; 
    
    Serial.print("Flower(cm): ");
    Serial.print(flowerDistance);
    Serial.print(" \t|\t ");
    
    Serial.print("Pond(0-1023): ");
    Serial.print(pondPressure);
    Serial.print(" \t|\t ");

    Serial.print("Motor(Angle): ");
    Serial.println(leavesAngle);

    Serial.print("Speaker(Vol): ");
    Serial.print(musicVolume);
    Serial.print(" \t|\t ");
    
    Serial.print("State: ");
    Serial.println(isPlaying ? "Play" : "Pause");
  }
}