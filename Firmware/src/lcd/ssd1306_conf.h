/**
 * Private configuration file for the SSD1306 library.
 * This example is configured for STM32F0, I2C and including all fonts.
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

// Mirror the screen if needed
// #define SSD1306_MIRROR_VERT
// #define SSD1306_MIRROR_HORIZ

// Set inverse color if needed
// # define SSD1306_INVERSE_COLOR


//#define SSD1306_USE_SH1106


#define SSD1306_I2C_ADDR		0x3C

#define SSD1306_HEIGHT          64
#define SSD1306_WIDTH           128

#define SSD1306_BUFFER_SIZE   SSD1306_WIDTH * SSD1306_HEIGHT / 8

// Include only needed fonts
#define SSD1306_USE_FONT_3x5
#define SSD1306_USE_FONT_5x5
#define SSD1306_USE_FONT_5x7
#define SSD1306_USE_FONT_6x8
//#define SSD1306_USE_FONT_8x16

#define SSD1306_USE_FONT_7x10
//#define SSD1306_USE_FONT_11x18
//#define SSD1306_USE_FONT_16x26


#endif // __SSD1306_CONF_H__
