#ifndef LCD_H_
#define LCD_H_

#include "Font.h"

class LCD
{
public:
     // Enum to represent the wrap style of the string being written
     enum wrap_style {
	  NO_WRAP = 0,
	  WRAP_RETURN = 1,
	  WRAP_NEWLINE = 2
     };

     //Enum to represent the type of byte being sent to the LCD screen
     enum byte_type {
	  COMMAND_BYTE = LOW,
	  DATA_BYTE = HIGH
     };

     // Enum to represent the LCD bias voltage level ratios, BS_1_100 = 1/100
     enum bias_system {
	  BS_1_100 = 0,
	  BS_1_80 = 1,
	  BS_1_65 = 2,
	  BS_1_48 = 3,
	  BS_1_34 = 4,
	  BS_1_40 = 4,
	  BS_1_24 = 5,
	  BS_1_16 = 6,
	  BS_1_18 = 6,
	  BS_1_8 = 7,
	  BS_1_9 = 7,
	  BS_1_10 = 7
     };

     // Enum to represent the temperature control coefficient
     enum temperature_control
     {
	  TC0 = 0,
	  TC1 = 1,
	  TC2 = 2,
	  TC3 = 3
     };

     // Enum to represent the display modes of the LCD
     enum display_mode
     {
	  DISPLAY_BLANK_OFF = 0,  // Blank screen, all pixels off
	  DISPLAY_BLANK_ON = 1,   // Blank screen, all pixels on
	  DISPLAY_NORMAL = 4,     // Normal mode, display pixels from ram
	  DISPLAY_INVERTED = 5    // Inverted mode, display pixels from ram inverted
     };

     // Enum to represent the orientation of the screen
     enum orientation
     {
	  LANDSCAPE = 0,       // Normal landscape
	  PORTRAIT = 1,        // Normal portrait 90 degrees to the left of landscape
	  REV_LANDSCAPE = 2,   // Landscape reversed, 180 degrees
	  REV_PORTRAIT = 3     // Portrait reversed, 90 degrees to the right of landscape
     };

     // Create an istance of the LCD class
     LCD(int clock = 2, int output = 3, int type = 4, int enable = 5, int reset = 6, int backlight = 7);

     // Initialize the LCD screen, and set the output to buffered or not
     bool init(bool buffered = true);

     // Clear the screen buffer if the output is being buffered
     // Clear the screen if output is not buffered
     void clear();

     // Buffered function
     // Flush the contents of the screen buffer if the output is buffered
     void flush();

     // Set whether the output should be buffered or not
     bool setBuffered(bool buffered = true);

     // Returns whether the output is being buffered or not
     bool isBuffered();

     // Writing/drawing related functions
     // Set the font to be used when writing
     void setFont(Font font);

     // Get the font being used when writing
     Font getFont();

     // Write a srtring to the LCD screen
     // If not buffered, will write the string directly using writeStringDirect(string, locx, locy / 8, inverted);
     // The font's actual size will be 2^(size - 1)
     void writeString(const char *string, int locx, int locy, int size = 1, bool inverted = false);

     // Write a string directly to the LCD screen, not buffered
     // This method can only write on the 5 LCD screen rows individually (0 <= locy <= 5) with size 1
     void writeStringDirect(const char *string, int locx, int locy, bool inverted = false);

     // Draw the specified bitmap to the LCD screen
     // If not buffered, will draw the bitmap directly using drawBitmapDirect(bitmap, locx, locy / 8, width, height, inverted);
     // The bitmaps actual scale will be 2^(scale - 1)
     void drawBitmap(char *bitmap, int locx, int locy, int width, int height, int scale = 1, bool inverted = false);

     // Draw the specified bitmap directly to the LCD screen
     // This method can only draw on the 5 LCD screen rows  (0 <= locy <= 5) with scale 1
     void drawBitmapDirect(char *bitmap, int locx, int locy, int width, int height, bool inverted = false);

     // Write a single byte to the LCD screen
     void writeByte(char byte, byte_type type);

     // Writing options functions
     // Set whether the output should be flushed automatically
     void setAutoFlush(bool flush = true);

     // Returns whether the output is being flushed automatically
     bool isAutoFlush();

     // Set the word wrap style for output
     void setWrapStyle(wrap_style wrap);

     // Get the current word wrap style
     wrap_style getWrapStyle();

     // LCD Options
     // Set the LCD power down state on or off
     void setPowerDown(bool powerdown = true);

     // Set the backlight on or off
     void setBacklight(bool backlight = true);

     // Set the bias system ratio
     void setBiasSystem(bias_system bs);

     // Set the temperature control coefficient
     void setTemperatureControl(temperature_control tc);

     // Set the operating voltage of the LCD screen, using the lower 6 bits of the parameter
     void setOperatingVoltage(char opvol);

     // Set the display mode of the LCD screen
     void setDisplayMode(display_mode mode);

private:
     // The pins for this instance, initialized with constructor
     int m_clock;
     int m_output;
     int m_type;
     int m_enable;
     int m_reset;
     int m_backlight;

     // Writing options
     bool m_autoflush; // Initially true
     wrap_style m_wrapstyle; // Initially WRAP_RETURN

     // Screen buffer
     char **m_screen; // Initially not initialized, setBuffered(true), or init(true) will initialize it

     // Font
     Font m_font; // Initially uses DEFAULT_FONT from Font.h

     // Set the settings of the LCD screen
     // powerdown = power down state
     // vertical = vertical (true) or horizontal (false) data entry
     // extended = function set of the LCD screen
     void set(bool powerdown, bool vertical, bool extended);

     // Write a column to the screen buffer with a per-pixel location
     void writeBuffered(char column, int locx, int locy, int cxoff, int cyoff, int size);
};

#endif /* LCD_H_ */
