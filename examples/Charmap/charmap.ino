#include <Arduino.h>
#include <LCD.h>

// The charmap that will be written
char charmap[255];

// The LCD instance
LCD lcd;

void setup()
{
     int count = lcd.getFont().getCharacterCount();

     // Set up the char map
     for (int i = 0; i < count; i++) {
	  charmap[i] = i + 1;
     }

     charmap[count] = 0;

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

int scroll = 0;

void loop()
{
     // Clear the screen and write the map, which will scroll up with each loop
     // The text will wrap because the default wrap style is WRAP_RETURN
     lcd.clear();
     lcd.writeString(charmap, 0, - (scroll * 4));

     // Flush the output, because autoflush is off
     lcd.flush();

     // Increment the scroll
     scroll = scroll + 1;

     // If we have scrolled enough to see all the characters in the font, reset the scroll
     if (scroll > ((lcd.getFont().getCharacterCount() / 14) - 2) * 2) {
	  scroll = 0;

	  delay(1200);
     }
     else {
	  delay(200);
     }
}
