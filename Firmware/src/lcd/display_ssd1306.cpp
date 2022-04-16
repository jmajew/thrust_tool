/*
 * display_ssd1306.cpp
 *
 *  Created on: Mar 16, 2021
 *      Author: jmajew
 */
 
#include "display_ssd1306.hpp"
#include "ssd1306_commands.h"


 ////////////////////////////////////////////////////////////////////////////////////////////////////
// class Display_SSD1306
////////////////////////////////////////////////////////////////////////////////////////////////////

template <class DispInterface>
Display_SSD1306<DispInterface>::Display_SSD1306( DispInterface* pif)
	: mpIf(pif),
	  mbInverted(false), mbInitialized(false), mbDisplayOn(false),
	  mCurrentX(0), mCurrentY(0)
{
}

template <class DispInterface>
uint8_t Display_SSD1306<DispInterface>::Init()
{
    // Wait for the screen to boot
	chThdSleepMilliseconds( 100);


    int status = 0;

    // Display_SSD1306 off
    status += mpIf->WriteCommand( SSD1306_DISPLAY_OFF );
    mbDisplayOn = false;

    // Set Memory Addressing Mode
    status += mpIf->WriteCommand( SSD1306_MEMORY_MODE );
    status += mpIf->WriteCommand( SSD1306_ADDR_MODE_PAGE);

    // Set COM Output Scan Direction
    status += mpIf->WriteCommand( SSD1306_COMSCAN_DEC);

    // Set Page Start Address for Page Addressing Mode,0-7
    status += mpIf->WriteCommand( SSD1306_SET_START_PAGE);
    status += mpIf->WriteCommand( SSD1306_SET_LOW_COLUMN);   // Set low column address
    status += mpIf->WriteCommand( SSD1306_SET_HIGH_COLUMN);   // Set high column address

    status += mpIf->WriteCommand( SSD1306_SET_START_LINE);   // Set start line address

    // set contrast control register
    status += mpIf->WriteCommand( SSD1306_SET_CONTRAST);
    status += mpIf->WriteCommand( 0xFF);

    // Set segment re-map 0 to 127
    status += mpIf->WriteCommand( SSD1306_SEG_REMAP_127);

    // Set normal display
    status += mpIf->WriteCommand( SSD1306_NORMAL_DISPLAY);

    // Set multiplex ratio(1 to 64)
    status += mpIf->WriteCommand( SSD1306_SET_MULTIPLEX);
    status += mpIf->WriteCommand( SSD1306_HEIGHT - 1);

    // Set display offset
    status += mpIf->WriteCommand( SSD1306_SET_DISPLAY_OFFSET);
    status += mpIf->WriteCommand( 0x00);   // No offset

    // Set display clock divide ratio/oscillator frequency
    status += mpIf->WriteCommand( SSD1306_SET_DISPLAY_CLOCK_DIV);
    status += mpIf->WriteCommand( 0x80);   // Set divide ratio

    status += mpIf->WriteCommand( SSD1306_SET_PRECHARGE);   // Set pre-charge period
    status += mpIf->WriteCommand( 0x22);

    // Set com pins hardware configuration
    status += mpIf->WriteCommand( SSD1306_SET_COM_PINS);

#ifdef SSD1306_COM_LR_REMAP
    status += mpIf->WriteCommand( 0x32);   // Enable COM left/right remap
#else
    status += mpIf->WriteCommand( 0x12);   // Do not use COM left/right remap
#endif // SSD1306_COM_LR_REMAP

    status += mpIf->WriteCommand( SSD1306_SET_VCOM_DETECT);	// Set vcomh
    status += mpIf->WriteCommand( 0x20);   					// 0x20,0.77xVcc

    status += mpIf->WriteCommand( SSD1306_CHARGE_PUMP);   	// Set DC-DC enable
    status += mpIf->WriteCommand( 0x14);

    // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    status += mpIf->WriteCommand( SSD1306_OUTPUT_FOLLOWS_RAM);

    // Display_SSD1306 on
    status += mpIf->WriteCommand( SSD1306_DISPLAY_ON);
    mbDisplayOn = true;

    if ( status != 0)
        return 1;

    // Clear screen
    Fill(Black);

    // Set default values for screen object
    mCurrentX = 0;
    mCurrentY = 0;

    mbInitialized = true;

    // Flush buffer to screen
    UpdateScreen();

    return 0;
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::SetContrast( const uint8_t value)
{
    mpIf->WriteCommand( SSD1306_SET_CONTRAST);
    mpIf->WriteCommand( value);
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::SetDisplayOn( const bool bon)
{
	mbDisplayOn = bon;
    if ( bon )
    	mpIf->WriteCommand(SSD1306_DISPLAY_ON);
    else
    	mpIf->WriteCommand(SSD1306_DISPLAY_OFF);
}

template <class DispInterface>
bool Display_SSD1306<DispInterface>::IsDisplayOn(void)
{
	return mbDisplayOn;
}


template <class DispInterface>
void Display_SSD1306<DispInterface>::Fill( EColor color)
{
    for( uint32_t i = 0; i < sizeof(mBuffer); i++)
        mBuffer[i] = (color == Black) ? 0x00 : 0xFF;

//	memset( mBuffer, (color==White ? 0xFF : 0x00 ), sizeof(mBuffer) );
//    FillRect( 0, 0, SSD1306_WIDTH-1, SSD1306_HEIGHT-1, color );
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::FillRect( uint8_t xpos, uint8_t ypos, uint8_t w, uint8_t h, EColor color)
{
	for( int j = 0; j < w; ++j )
		for(int i = 0; i < h; ++i)
			SetPixel(xpos + j, ypos + i, color);

}


template <class DispInterface>
void Display_SSD1306<DispInterface>::UpdateScreen()
{
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++)
    {
    	// Set the current RAM page address.
    	mpIf->WriteCommand(SSD1306_SET_START_PAGE + i);
    	mpIf->WriteCommand(SSD1306_SET_LOW_COLUMN);
    	mpIf->WriteCommand(SSD1306_SET_HIGH_COLUMN);

    	mpIf->WriteData( &mBuffer[SSD1306_WIDTH*i], SSD1306_WIDTH);
    }
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::SetCursor( uint8_t x, uint8_t y)
{
    this->mCurrentX = x;
    this->mCurrentY = y;
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::SetPixel( uint8_t x, uint8_t y, EColor color)
{
    if ( x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT )
        return;

    if( this->mbInverted )
        color = (EColor)!color;

    // Draw in the right color
    if ( color == White )
    {
        mBuffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        mBuffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, EColor color)
{
	int32_t deltaX = abs(x2 - x1);
	int32_t deltaY = abs(y2 - y1);
	int32_t signX = ((x1 < x2) ? 1 : -1);
	int32_t signY = ((y1 < y2) ? 1 : -1);
	int32_t error = deltaX - deltaY;
	int32_t error2;

	SetPixel( x2, y2, color);
	while ( (x1 != x2) || (y1 != y2) )
	{
		SetPixel( x1, y1, color);
		error2 = error * 2;
		if ( error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}

		if ( error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}
	return;
}

template <class DispInterface>
void Display_SSD1306<DispInterface>::Rectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, EColor color)
{
	this->Line(x1,y1,x2,y1,color);
	this->Line(x2,y1,x2,y2,color);
	this->Line(x2,y2,x1,y2,color);
	this->Line(x1,y2,x1,y1,color);

	return;
}



template <class DispInterface>
void Display_SSD1306<DispInterface>::Circle( uint8_t par_x, uint8_t par_y, uint8_t par_r, EColor par_color)
{
  int32_t x = -par_r;
  int32_t y = 0;
  int32_t err = 2 - 2 * par_r;
  int32_t e2;

  if (par_x >= SSD1306_WIDTH || par_y >= SSD1306_HEIGHT)
  {
    return;
  }

    do
    {
		SetPixel( par_x - x, par_y + y, par_color);
		SetPixel( par_x + x, par_y + y, par_color);
		SetPixel( par_x + x, par_y - y, par_color);
		SetPixel( par_x - x, par_y - y, par_color);
        e2 = err;

        if ( e2 <= y)
        {
            y++;
            err = err + (y * 2 + 1);
            if(-x == y && e2 <= x)
            {
              e2 = 0;
            }
        }

        if ( e2 > x)
        {
          x++;
          err = err + (x * 2 + 1);
        }
    }
    while (x <= 0);

    return;
}


template <class DispInterface>
void Display_SSD1306<DispInterface>::DrawBitmap( uint8_t xpos, uint8_t ypos, uint8_t w, uint8_t h, const uint8_t *bitmap, EColor color)
{
	int nw = (h - 1) / 8 + 1;

	for(int j = 0; j < w; ++j)
	{
		for ( int k=0; k<nw; ++k)
		{
			uint8_t byte = *(bitmap + j + k*w);

			for ( int i = 0; i < 8; ++i)
			{
				if ( k*8 + i >= h )
					break;

				if( (byte >> i) & 0x01)
					SetPixel(xpos + j, ypos + i + 8*k, color);
				else
					SetPixel(xpos + j, ypos + i + 8*k, (EColor)!color);
			}
		}
	}

}


template <class DispInterface>
void Display_SSD1306<DispInterface>::WriteChar( char c, const FontDef& fdef, EColor color)
{
	int nw = (fdef.fontHeight - 1) / 8 + 1;
	const uint8_t* chardata = fdef.data + (c - 32)*fdef.fontWidth*nw;
	DrawBitmap( this->mCurrentX, this->mCurrentY, fdef.fontWidth, fdef.fontHeight, chardata, color);
}


template <class DispInterface>
void Display_SSD1306<DispInterface>::WriteString( const char* str, const FontDef& fdef, EColor color)
{
	while (*str)
	{
		if ( this->mCurrentX + fdef.fontWidth > SSD1306_WIDTH )
			return;

		WriteChar( *str, fdef, color);

		// Next char
		this->mCurrentX += fdef.fontWidth + 1;
		str++;
	}
}


#ifdef LCD_IF_I2C
template class Display_SSD1306<DispI2cInterface>;
#endif

#ifdef LCD_IF_SPI
template class Display_SSD1306<DispSpiInterface>;
#endif



