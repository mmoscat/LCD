#include <stdlib.h>
#include <Arduino.h>
#include "Font.h"
#include "LCD.h"

LCD::LCD(int clock, int output, int type, int enable, int reset, int backlight)
{
     // Initialize the members of the LCD class
     m_clock = clock;
     m_output = output;
     m_type = type;
     m_enable = enable;
     m_reset = reset;
     m_backlight = backlight;

     m_screen = 0;
     m_autoflush = true;
     m_wrapstyle = WRAP_RETURN;
     m_font = Font();
}

bool LCD::init(bool buffered)
{
     // Initialize all the pins to low (including reset and enable, which are active LOW)
     // RESET signal needs to be sent within 100 ms of power being applied to the LCD controller
     pinMode(m_clock, OUTPUT);
     digitalWrite(m_clock, LOW);

     pinMode(m_output, OUTPUT);
     digitalWrite(m_output, LOW);

     pinMode(m_type, OUTPUT);
     digitalWrite(m_type, LOW);

     pinMode(m_enable, OUTPUT);
     digitalWrite(m_enable, LOW);

     pinMode(m_reset, OUTPUT);
     digitalWrite(m_reset, LOW);

     pinMode(m_backlight, OUTPUT);
     digitalWrite(m_backlight, LOW);

     // Reset function:
     // enable pin must be high when the reset pin goes high
     digitalWrite(m_enable, HIGH);
     delay(100);
     digitalWrite(m_reset, HIGH);

     // Initialize the options
     setBiasSystem(BS_1_48);
     setOperatingVoltage(16);
     setDisplayMode(DISPLAY_BLANK_OFF);

     // Set to buffered
     if (!setBuffered(buffered)) {
	  return false;
     }

     // Clear the screen
     clear();

     // Set display to normal
     setDisplayMode(DISPLAY_NORMAL);

     return true;
}

void LCD::clear()
{
     // If not buffered, write all 0s
     if (!m_screen) {
	  // Set the screen settings for output
	  set(false, false, false);

	  // Set the cursor to (0, 0)
	  writeByte(0x80, COMMAND_BYTE); // X
	  writeByte(0x40, COMMAND_BYTE); // Y

	  // Clear screen
	  for (int i = 0; i < 6; i++) {
	       for (int j = 0; j < 84; j++) {
		    writeByte(0, DATA_BYTE);
	       }
	  }

	  return;
     }

     // If buffered, clear the screen buffer
     for (int i = 0; i < 6; i++) {
	  for (int j = 0; j < 84; j++) {
	       m_screen[i][j] = 0;
	  }
     }

     // Flush to the screen
     if (m_autoflush) {
	  flush();
     }
}

void LCD::flush()
{
     // If not buffered, and flushing screen, do nothing
     if (!m_screen) {
	  return;
     }

     // If not buffered, and not flusning screen (flushing with 0s instead)
     // Or if buffered and flushing screen

     // Set the screen settings for output
     set(false, false, false);

     // Set the cursor to (0, 0)
     writeByte(0x80, COMMAND_BYTE); // X
     writeByte(0x40, COMMAND_BYTE); // Y

     // Write screen bytes
     for (int i = 0; i < 6; i++) {
	  for (int j = 0; j < 84; j++) {
	       writeByte(m_screen[i][j], DATA_BYTE);
	  }
     }
}

bool LCD::setBuffered(bool buffered)
{
     if (!buffered && m_screen) {
	  // Free the screen pointer if going from buffered to not buffered
	  for (int i = 0; i < 6; i++) {
	       free(m_screen[i]);
	       m_screen[i] = 0;
	  }

	  free(m_screen);
	  m_screen = 0;
     }
     else if (buffered && !m_screen) {
	  // Initialize the screen buffer if going from not buffered to buffered
	  m_screen = (char **) malloc(sizeof(char *) * 6);

	  if (!m_screen) {
	       return false;
	  }

	  for (int i = 0; i < 6; i++) {
	       m_screen[i] = (char *) malloc(sizeof(char) * 84);

	       if (!m_screen[i]) {
		    return false;
	       }

	       memset(m_screen[i], 0, 84);
	  }
     }

     return true;
}

bool LCD::isBuffered()
{
     // Return if the output is being buffered
     return m_screen != 0;
}

void LCD::setFont(Font font)
{
     // Set the font being used for output
     m_font = font;
}

Font LCD::getFont()
{
     // Get the font being used for output
     return m_font;
}

void LCD::writeString(const char *string, int locx, int locy, int size, bool inverted)
{
     // If not buffered, write direct
     if (!m_screen) {
	  writeStringDirect(string, locx, locy / 8, inverted);
	  return;
     }

     int realSize = 1;
     int cxoff = 0;
     int cyoff = 0;

     // Set the real size
     for (int i = 0; i < size - 1; i++) {
	  realSize *= 2;
     }

     // Set the addition amount after every character
     int addition = realSize * m_font.getWidth();

     while (*string != 0) {
	  // Loop through the columns of the character bitmap
	  for (int col = 0; col < m_font.getWidth(); col++) {
	       // Get the current column, and invert it if needed
	       char column = m_font[(unsigned char) *string][col] ^ (inverted ? 0xFF : 0);

	       // Calculate the column x-offset
	       int xoff = col * realSize;

	       // Write the column to the buffer
	       writeBuffered(column, locx, locy, cxoff + xoff, cyoff, realSize);
	  }

	  // increment the character x-offset
	  cxoff = cxoff + addition;

	  // If there is a wrap style, apply the necessary corrections to the character x and y offsets
	  if (m_wrapstyle != NO_WRAP && (cxoff + addition + locx) >= 84) {
	       cyoff = cyoff + realSize;
	       cxoff = 0;

	       // If we are wrapping without new line, go back to beginning of the row
	       if (m_wrapstyle == WRAP_RETURN) {
		    locx = 0;
	       }
	  }

	  // Go to the next character
	  string++;
     }

     // Flush the screen buffer
     if (m_autoflush) {
	  flush();
     }
}

void LCD::writeStringDirect(const char *string, int locx, int locy, bool inverted)
{
     // Set the screen settings for output
     set(false, false, false);

     // Make sure the values are within limits
     locx = (locx & 0x7F) % 84;
     locy = (locy & 0x07) % 6;

     // Set the cursor to the specified location
     writeByte(0x80 + locx, COMMAND_BYTE); // X
     writeByte(0x40 + locy, COMMAND_BYTE); // Y

     // Write the string to the LCD screen
     while (*string != 0) {
	  // Check if we need to wrap the text
	  if (m_wrapstyle != NO_WRAP && locx + m_font.getWidth() >= 84) {
	       locy++;

	       // If we are wrapping without new line, go back to beginning of the row
	       if (m_wrapstyle == WRAP_RETURN) {
		    locx = 0;
	       }

	       // Return if we overflow
	       if (locy >= 6) {
		    break;
	       }

	       // Set the cursor to the specified location
	       writeByte(0x80 + locx, COMMAND_BYTE); // X
	       writeByte(0x40 + locy, COMMAND_BYTE); // Y
	  }

	  // Write the bytes
	  for (int i = 0; i < m_font.getWidth(); i++, locx++) {
	       writeByte(m_font[(unsigned char) *string][i] ^ (inverted ? 0xFF : 0), DATA_BYTE);
	  }

	  string++;
     }
}

void LCD::drawBitmap(char *bitmap, int locx, int locy, int width, int height, int scale, bool inverted)
{
     // If not buffered, draw direct
     if (!m_screen) {
	  drawBitmapDirect(bitmap, locx, locy / 8, width, height, inverted);
	  return;
     }

     int realScale = 1;

     // Set the correct height of the image in terms of LCD rows, not pixel rows
     height = ((height / 8) + ((height % 8) > 0 ? 1 : 0));

     // Set the actual scale of the image
     for (int i = 0; i < scale - 1; i++) {
	  realScale *= 2;
     }

     // Loop through the bitmap rows
     for (int y = 0; y < height && y < 6; y++) {
	  int curY = (y * width);

	  // Loop through the bitmap columns
	  for (int x = 0; x < width && x < (84 - locx); x++) {
	       // Write the column to the buffer
	       writeBuffered(bitmap[curY + x] ^ (inverted ? 0xFF : 0), locx, locy, x * realScale, y * realScale, realScale);
	  }
     }

     // Flush the screen buffer
     if (m_autoflush) {
	  flush();
     }
}

void LCD::drawBitmapDirect(char *bitmap, int locx, int locy, int width, int height, bool inverted)
{
     height = ((height / 8) + ((height % 8) > 0 ? 1 : 0));

     // Set the screen settings for output
     set(false, false, false);

     // Make sure the values are within limits
     locx = (locx & 0x7F) % 84;
     locy = (locy & 0x07) % 6;

     // Set the cursor to the specified location
     writeByte(0x80 + locx, COMMAND_BYTE); // X
     writeByte(0x40 + locy, COMMAND_BYTE); // Y

     for (int y = 0; y < height && y < (6 - locy); y++) {
	  // Set next location
	  writeByte(0x80 + locx, COMMAND_BYTE); // X
	  writeByte(0x40 + (y + locy), COMMAND_BYTE); // Y

	  int curY = y * width;

	  // Draw 8 rows of pixels at a time
	  for (int x = 0; x < width && x < (84 - locx); x++) {
	       writeByte(bitmap[curY + x] ^ (inverted ? 0xFF : 0), DATA_BYTE);
	  }
     }
}

void LCD::writeByte(char byte, byte_type type)
{
     // Set the chip to look for clock cycles
     digitalWrite(m_enable, LOW);
     // Set the byte type
     digitalWrite(m_type, (char) type);

     // Write the byte to the LCD screen, one bit at a time, starting with high bit
     for (int i = 0; i < 8; i++, byte <<= 1) {
	  digitalWrite(m_clock, LOW);
	  digitalWrite(m_output, byte < 0 ? HIGH : LOW);
	  digitalWrite(m_clock, HIGH);
     }

     // Set the chip to ignore clock cycles
     digitalWrite(m_enable, HIGH);
}

void LCD::setAutoFlush(bool flush)
{
     // Set the autoflush tag
     m_autoflush = flush;
}

bool LCD::isAutoFlush()
{
     // Return the autoflush flag
     return m_autoflush;
}

void LCD::setWrapStyle(wrap_style wrap)
{
     // Set the wrap style
     m_wrapstyle = wrap;
}

LCD::wrap_style LCD::getWrapStyle()
{
     // Return the wrap style
     return m_wrapstyle;
}

void LCD::setPowerDown(bool power_down)
{
     // Set the LCD screen power down state
     set(power_down, false, false);
}

void LCD::setBacklight(bool bs)
{
     // Set the LCD screen backlight state
     digitalWrite(m_backlight, bs ? HIGH : LOW);
}

void LCD::setBiasSystem(bias_system bs)
{
     // Set the LCD screen bias voltage
     char byte = 0x10 + bs;

     // Set extended function set
     set(false, false, true);

     writeByte(byte, COMMAND_BYTE);
}

void LCD::setTemperatureControl(temperature_control tc)
{
     // Set the LCD screen temperature control coefficient
     char byte = 0x04 + tc;

     // Set extended function set
     set(false, false, true);

     writeByte(byte, COMMAND_BYTE);
}

void LCD::setOperatingVoltage(char vop)
{
     // Set the LCD screen operating voltage, only the last 6 bits of the char
     char byte = 0x80 + (vop & 0x7F);

     // Set extended function set
     set(false, false, true);

     writeByte(byte, COMMAND_BYTE);
}

void LCD::setDisplayMode(display_mode mode)
{
     // Set the LCD screen display mode
     char byte = 0x08 + mode;

     // Set normal function set
     set(false, false, false);

     writeByte(byte, COMMAND_BYTE);
}

// Private functions below

void LCD::set(bool power_down, bool vertical, bool extended)
{
     // Set the LCD screen settings
     char data = 0x20 + (power_down ? 4 : 0) + (vertical ? 2 : 0) + (extended ? 1 : 0);

     writeByte(data, COMMAND_BYTE);
}

void LCD::writeBuffered(char column, int locx, int locy, int cxoff, int cyoff, int size)
{
     int divisor = 8 / size;

     // Loop through the bits in the bitmap column
     for (int row = 7; row >= 0; row--) {
	  // Calculate the y-offset, and the shift amount for the current bit
	  int yoff = (row / divisor) + (locy / 8);
	  int shift = ((row % divisor) * size) + (locy % 8);
	  char dot = 1 << shift;

	  // If the shift amount is negative due to locy being negative, set dot to 0
	  if (shift < 0) {
	       dot = 0;
	  }

	  // Loop through a realSize * realSize square for a character pixel
	  for (int y = 0; y < size; y++) {
	       // If the shift amount was negative, do correction for offset character pixels
	       if (dot == 0 && shift < 8) {
		    if (y == 0) {
			 dot = 0x80 >> (abs(shift) - 1);
			 yoff = yoff - 1;
		    }
		    else {
			 dot = 1;
			 yoff = yoff + 1;
		    }
	       }

	       // Loop through the current row of the characyer pixel
	       for (int x = 0; x < size; x++) {
		    // If there was over-shift, apply the correction
		    if (dot == 0 && shift >= 8) {
			 dot = 1 << (shift - 8);
			 yoff = yoff + 1;
		    }

		    // Check if we are out of bounds after correction
		    int rx = cxoff + x + locx;
		    int ry = cyoff + yoff;

		    if (rx < 0 || rx >= 84 || ry < 0 || ry >= 6) {
			 break;
		    }

		    // Draw screen pixel (rx, ry) of the character pixel
		    if (column < 0) {
			 m_screen[ry][rx] |= dot;
		    }
		    else {
			 m_screen[ry][rx] &= ~dot;
		    }
	       }

	       // Shift the dot for the next row of screen pixels
	       dot = dot << 1;
	  }

	  // Shift the column for the next bitmap column
	  column = column << 1;
     }
}
