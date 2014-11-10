#ifndef FONT_H_
#define FONT_H_

#include <avr/pgmspace.h>

#ifndef NO_DEFAULT_FONT

extern PROGMEM char DEFAULT_FONT[][6];

#endif /* NO_DEFAULT_FONT */

class Font
{
public:
     // Font wrapper to be able to use fonts as 2D arrays
     class FontWrapper
     {
     public:
	  
	  // Sets the font this is wrapping, and the first dimension
	  // index.
	  FontWrapper(Font *font, int index);
	  
	  // Second dimension of Font[][]
	  char operator[](int index);
	  
	  // Calls Font.getCharColumn(m_first, index);
	  char getCharColumn(int index);
	  
     private:	  
	  // The font being wrapped
	  Font *m_font;
	  
	  // The first dimension index
	  int m_first;
     };
     
#ifndef NO_DEFAULT_FONT
     
     // Default font constructor, for the DEFAULT_FONT
     Font();
     
#endif /* NO_DEFAULT_FONT */
     
     // This constructor initializes the font to be used with
     // the parameters provided. The arguments are a pointer to
     // the 2D font array, the number of characters in the font,
     // the width of the font characters, and the offset of
     // where the font starts.
     Font(char *font, int characters, int width, int offset = 0);
     
     // First dimension
     FontWrapper operator[](int index);
     
     // Returns the width of a single character
     int getWidth();
     
     // Returns the offset of the character set
     int getOffset();
     
     // Returns the number of characters in the font
     int getCharacterCount();
     
     // Returns a single column for a character in the font
     char getCharColumn(int which, int index);
     
private:
     // The width of a character
     int m_width;
     
     // The font offset
     int m_offset;
     
     // The number of characters
     int m_chars;
     
     // The font pointer
     char *m_font;
};

#endif /* FONT_H_ */
