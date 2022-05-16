/*
 * display_fonts.hpp
 *
 *  Created on: 3 Mar 2021
 *      Author: jmajew
 */

#ifndef __LCD_DISPLAY_FONTS_HPP__
#define __LCD_DISPLAY_FONTS_HPP__


#include "system/sysdecl.hpp"
#include "lcd/ssd1306_conf.h"


struct FontDef
{
	const uint8_t 	fontWidth;    	// Font width in pixels
	const uint8_t 	fontHeight;   	// Font height in pixels
	const uint8_t	margX;
	const uint8_t*	data; 			// Pointer to data font data array
};

#ifdef SSD1306_USE_FONT_3x5
extern const FontDef font3x5;
#endif

#ifdef SSD1306_USE_FONT_5x5
extern const FontDef font5x5;
#endif

#ifdef SSD1306_USE_FONT_5x7
extern const FontDef font5x7;
#endif

#ifdef SSD1306_USE_FONT_6x8
extern const FontDef font6x8;
#endif

#ifdef SSD1306_USE_FONT_7x10
extern const FontDef font7x10;
#endif

#ifdef SSD1306_USE_FONT_8x16
extern const FontDef font8x16;
#endif

#ifdef SSD1306_USE_FONT_11x18
extern const FontDef font11x18;
#endif

#ifdef SSD1306_USE_FONT_16x26
extern const FontDef font16x26;
#endif



#endif // __LCD_DISPLAY_FONTS_HPP__
