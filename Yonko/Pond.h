const int fsrPin = A0; 
int pondPressure = 0; 

void setupPond() {
  
}

void updatePond() {
  pondPressure = analogRead(fsrPin);
}