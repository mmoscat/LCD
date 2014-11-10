#include <Arduino.h>
#include <LCD.h>

// The LCD instance
LCD lcd;

void setup()
{
     // Attempt to initialize the LCD as buffered (default)
     if (lcd.init()) {
	  // When successful, turn on the backlight, and set to not auto-flush
	  lcd.setBacklight();
	  lcd.setAutoFlush(false);

	  // Write text into screen buffer
	  lcd.writeString("LCD", 24, 8, 2);
	  lcd.writeString("INITIALIZED", 8, 24);

	  // Flush screen buffer
	  lcd.flush();

	  delay(800);

	  // Clear screen buffer, but don't yet flush
	  lcd.clear();
     }
}

void loop()
{
     // This text is size 3
     lcd.writeString("HEY", 5, 10, 3);
     lcd.flush();

     delay(1500);

     // This text is size 2, and is inverted
     lcd.writeString("this is", -1, 18, 2, true);
     lcd.flush();

     delay(1500);

     // This text is normal sized
     lcd.writeString("an LCD screen", 2, 22);
     lcd.flush();

     delay(2000);

     lcd.clear();
}
