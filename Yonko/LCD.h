#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Use extern to access variables from Leaves.h without interfering with the original code
extern int activeLeaves;
extern unsigned long lastLeafRiseTime;

// ⚠️ Ensure this timer matches FIFTEEN_MINUTES in Leaves.h
// Change to 10000UL (10 seconds) for testing, revert to 900000UL (15 minutes) for exhibition
const unsigned long DISPLAY_TIMER = 900000UL; 

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2); 
unsigned long lastLcdUpdate = 0; 

void setupLCD() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("System Starting.");
  delay(500); 
  lcd.clear();
}

void updateLCD() {
  // Update the screen every 1 second (1000ms) to prevent flickering
  if (millis() - lastLcdUpdate >= 1000) {
    lastLcdUpdate = millis();

    if (activeLeaves > 0) {
      // Calculate remaining time for the next leaf to rise
      unsigned long elapsed = millis() - lastLeafRiseTime;
      unsigned long remaining = (DISPLAY_TIMER > elapsed) ? (DISPLAY_TIMER - elapsed) : 0;
      
      int remSeconds = remaining / 1000;
      int m = remSeconds / 60;
      int s = remSeconds % 60;

      // First Row: Display the number of leaves currently down
      lcd.setCursor(0, 0);
      lcd.print("Leaves Down: ");
      lcd.print(activeLeaves);
      lcd.print("   "); // Clear trailing characters

      // Second Row: Display the countdown timer (MM:SS)
      lcd.setCursor(0, 1);
      lcd.print("Next In: ");
      if (m < 10) lcd.print("0"); // Add leading zero for single digits
      lcd.print(m);
      lcd.print(":");
      if (s < 10) lcd.print("0"); // Add leading zero for single digits
      lcd.print(s);
      lcd.print("   "); // Clear trailing characters
    } else {
      // Standby screen when no leaves are active
      lcd.setCursor(0, 0);
      lcd.print("All Leaves Up   ");
      lcd.setCursor(0, 1);
      lcd.print("Timer Idle      ");
    }
  }
}