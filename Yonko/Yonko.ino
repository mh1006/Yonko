#include "Flower.h"
#include "Leaves.h"
#include "Pond.h"
#include "Mushroom.h"


unsigned long previousMillis = 0;
const long interval = 500;

void setup() {
  Serial.begin(9600);

  setupMushroom();
  setupFlower();
  setupLeaves();
  setupPond();

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
    Serial.print(" \t|\t ");\

    Serial.print(" [");
    if (isCupPlaced) {
      Serial.print("Placed, Diff: ");
      int diff = pondPressure - storedWeight;
      
      if (diff > 0) {
        Serial.print("+"); 
      }
      Serial.print(diff); 
    } else {
      Serial.print("Lifted"); 
    }
    Serial.print("] \t|\t ");

    Serial.print("Active Leaves: ");
    Serial.println(activeLeaves);

    Serial.print("State: ");
    Serial.println(isPlaying ? "Play" : "Pause");
    Serial.print(" \t|\t ");
    Serial.print("Speaker(Vol): ");
    Serial.print(musicVolume);  
    Serial.print(" \t");  
    
  }
}