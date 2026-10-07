const int trigPin = 5;
const int echoPin = 6;
int flowerDistance = 0; 

void setupFlower() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void updateFlower() {
  long duration;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH, 30000); 
  flowerDistance = duration * 0.034 / 2;
}