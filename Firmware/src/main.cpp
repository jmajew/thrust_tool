/*
 ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <cmath>
#include <utility>

#include "system/sysdecl.hpp"
#include "util/waitmicro.hpp"

#include "io/io.hpp"
#include "master.hpp"

#include "cli.hpp"

#include "lcd/display_ssd1306.hpp"

#include "driver/motor.hpp"
#include "driver/adc.hpp"
#include "driver/hx711.hpp"
#include "driver/rpm_icu.hpp"

#include "th_measure.hpp"
#include "th_serial.hpp"
#include "th_esctelem.hpp"


#define WITH_LCD_DISPLAY


using blink_def = std::pair<systime_t,systime_t>;

blink_def blink_arr[] = { {250,250}, {125,300}, {80,80} };
uint8_t blink_mode = 0;

//===========================================================================
// Red LED blinker thread, times are in milliseconds.
//===========================================================================
class BlinkerThread: public chibi::BaseStaticThread<128>
{
public:
	BlinkerThread( void)
			: chibi::BaseStaticThread<128>()
	{
	}

protected:
	void main( void) override
	{
		setName( "Blinker");

//		blink_def time(250,150);
//		systime_t time = 250;

		while ( true)
		{
			palClearLine( LINE_LED_BLUE);
			sleep( TIME_MS2I( blink_arr[blink_mode].first));
			palSetLine( LINE_LED_BLUE);
			sleep( TIME_MS2I( blink_arr[blink_mode].second));

//			palToggleLine( LINE_LCD_SPI_CS );
//			palToggleLine( LINE_LCD_SPI_DC );
//			palToggleLine( LINE_LCD_RESET );
		}
	}

};


//===========================================================================
// Heart-beat thread, times are in milliseconds.
//===========================================================================
class HBeatThread: public chibi::BaseStaticThread<128>
{
public:
	HBeatThread( void)
			: chibi::BaseStaticThread<128>(), mpMaster(nullptr)
	{
	}

	void	SetMaster( Master* pmst)	{ mpMaster = pmst; }

protected:
	void main( void) override
	{
		setName( "HBeat");

		while ( true)
		{
			if ( mpMaster)
			{			
				mpMaster->pData()->mux.lock();
				bool bHBeat = mpMaster->pData()->bConnAlive;
				mpMaster->pData()->mux.unlock();

				
				if ( bHBeat )
				{
					palSetLine( LINE_LED_GREEN);
				}
				else
				{
					mpMaster->SetSerialMode( ESerialMode::Command );
					mpMaster->MotorStop();
					mpMaster->MotorDisarm();
					palClearLine( LINE_LED_GREEN);
				}
				
				mpMaster->pData()->mux.lock();
				mpMaster->pData()->bConnAlive = false;
				mpMaster->pData()->mux.unlock();
			}
			
			chibi::BaseThread::sleep( TIME_MS2I( 1300));
		}
	}
	
private:
	Master*			mpMaster;
};



IoMaster	io( (BaseAsynchronousChannel*)&SD_OUTPUT, (BaseAsynchronousChannel*)&SD_SERIAL );
Master		master;

#ifdef WITH_LCD_DISPLAY
DisplayIf	lcd_if( &LCD_IF);
Display		lcd( &lcd_if);	// TODO : add lcd_mutex
#endif

//RpmDev			dev_rpm( &master.pConfig()->groupRpm );


//===========================================================================
static BlinkerThread 	blinker_th;
static HBeatThread		hbeat_th;



systime_t now = 0;

//===========================================================================
// Application entry point.
//===========================================================================
int main( void)
{
	halInit();
	chibi::System::init();


	gpioInit();

	InitSystem();
	shellInit();
	gptInitTimer();

	master.Init();
	io.Start();

	palClearLine( LINE_LED_YELLOW);
	palClearLine( LINE_LED_GREEN);
	palClearLine( LINE_LED_BLUE);



#ifdef WITH_LCD_DISPLAY
	////////////////////////////////////////////////////////
	// LCD init

	lcd_if.Start();
	lcd.Init();

	chibi::BaseThread::sleep( TIME_MS2I( 200));

	lcd.Fill(Black);
	lcd.SetCursor(5, 10);
	lcd.WriteString("Hello !!!", font7x10, White);
	lcd.UpdateScreen();
	chibi::BaseThread::sleep( TIME_MS2I( 800));

	lcd.Fill(Black);
	for ( uint32_t delta = 0; delta < 5; ++delta)
		lcd.Rectangle( 1 + (5*delta), 1 + (5*delta), SSD1306_WIDTH-1 - (5*delta), SSD1306_HEIGHT-1 - (5*delta), White);
	lcd.UpdateScreen();
	chibi::BaseThread::sleep( TIME_MS2I( 500));

	lcd.Fill(White);
	lcd.UpdateScreen();
	chibi::BaseThread::sleep( TIME_MS2I( 500));
	lcd.Fill(Black);
	lcd.UpdateScreen();
	////////////////////////////////////////////////////////
#endif

	master.Start();

	blinker_th.start( NORMALPRIO + 5);
	
	hbeat_th.SetMaster( &master );
	hbeat_th.start( NORMALPRIO );

//	dev_rpm.Init( &RPM_ICUD, RPM_ICU_CH);
//	dev_rpm.Start();

	chibi::BaseThread::sleep( TIME_MS2I( 50));

	while ( true)
	{
		now = chVTGetSystemTime();
//		dbg_printf("systime = %d\n", now);

//		dbg_puts("HELLO\n" );

//		palToggleLine( LINE_LED_BLUE );
//		palToggleLine( LINE_DEBUG_1 );
//		palToggleLine( LINE_DEBUG_2 );
//		spiSend( &SPID3, 6, "abcdef");

//		if ( palReadLine(LINE_BUTTON) == PAL_LOW)
//		{
//				motor.SetOutput( 1200);
//			 shellStart( (BaseSequentialStream*)&SD_OUTPUT);
//		}

		palToggleLine( LINE_DEBUG_1 );

#ifdef WITH_LCD_DISPLAY
		master.pData()->PrintOnScreen(&lcd);
#endif

//		palToggleLine( LINE_DEBUG_1 );

		chibi::BaseThread::sleep( TIME_MS2I( 50));
	}

	return 0;
}

