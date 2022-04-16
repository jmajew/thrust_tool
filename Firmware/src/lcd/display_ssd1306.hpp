/*
 * display_ssd1306.hpp
 *
 *  Created on: Mar 16, 2021
 *      Author: jmajew
 */
 
#ifndef __LCD_DISPLAY_SSD1306_HPP__
#define __LCD_DISPLAY_SSD1306_HPP__

#include "system/sysdecl.hpp"
#include "display_fonts.hpp"
#include "display_i2c_if.hpp"
#include "display_spi_if.hpp"




enum EColor
{
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
};



////////////////////////////////////////////////////////////////////////////////////////////////////
// class Display_SSD1306
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class DispInterface>
class Display_SSD1306
{
public:
	struct Vertex
	{
		Vertex( uint8_t nx=0, uint8_t ny=0) : x(nx), y(ny)	{}

	    uint8_t x;
	    uint8_t y;
	};

public:
	Display_SSD1306( DispInterface* pif = nullptr);

//	void	SetInterface( DispInterface* pif)	{ mpIf = pif; }

	uint8_t	Init();

	void	Fill( EColor color);
	void	FillRect( uint8_t xpos, uint8_t ypos, uint8_t w, uint8_t h, EColor color);

	void 	UpdateScreen();

	void	WriteChar( char c, const FontDef& fdef, EColor color);
	void	WriteString( const char* str, const FontDef& fdef, EColor color);

	void 	SetCursor( uint8_t x, uint8_t y);
	void 	SetPixel( uint8_t x, uint8_t y, EColor color);
	void 	Line( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, EColor color);
	void 	Rectangle( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, EColor color);
	void 	Circle( uint8_t par_x, uint8_t par_y, uint8_t par_r, EColor color);

//	void 	DrawArc( uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, EColor color);
//	void 	Polyline( const Vertex *par_vertex, uint16_t par_size, EColor color);

	void	SetContrast( const uint8_t value);
	void	SetDisplayOn( const bool bon);
	bool	IsDisplayOn();

	//// Low-level procedures
	//void ssd1306_Reset(void);
	//void ssd1306_WriteCommand(uint8_t byte);
	//void ssd1306_WriteData(uint8_t* buffer, size_t buff_size);

//	SSD1306_Error_t	FillBuffer(uint8_t* buf, uint32_t len);

	void	DrawBitmap( uint8_t xpos, uint8_t ypos, uint8_t w, uint8_t h, const uint8_t *bitmap, EColor color);

private:
	DispInterface*	mpIf;

	uint8_t		mBuffer[SSD1306_BUFFER_SIZE];

    bool 		mbInverted;
    bool 		mbInitialized;
    bool 		mbDisplayOn;

    uint16_t	mCurrentX;
    uint16_t 	mCurrentY;

// TODO :: add mutex
};



#ifdef LCD_IF_I2C
using Display = Display_SSD1306<DispI2cInterface>;
using DisplayIf = DispI2cInterface;
#endif

#ifdef LCD_IF_SPI
using Display = Display_SSD1306<DispSpiInterface>;
using DisplayIf = DispSpiInterface;
#endif



#endif // __LCD_DISPLAY_SSD1306_HPP__
