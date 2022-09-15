/*
 * data.cpp
 *
 *  Created on: Apr 26, 2021
 *      Author: jmajew
 */


#include "data.hpp"

#define  BUFF_SIZE 	48
char  sbuff[BUFF_SIZE];

void Data::UpdateZero()
{
	dataZero.mux.lock();

	grpStGauge.mux.lock();
	for ( int i=0; i<STGAUGE_CH_COUNT; ++i)
	{
		dataZero.tabStGauge[i].Accumulate( grpStGauge.tab[i] );
	}
	grpStGauge.mux.unlock();

	grpAdc.mux.lock();
	for ( int i=0; i<ADC_CH_COUNT; ++i)
	{
		dataZero.tabADC[i].Accumulate( grpAdc.tab[i] );
	}
	grpAdc.mux.unlock();

	dataZero.mux.unlock();
}


void Data::PrintOnScreen( Display* plcd)
{
	int offset = 16;
	systime_t now = chVTGetSystemTime();

	plcd->Fill(Black);

	chsnprintf( sbuff, BUFF_SIZE, "time = %d", now );
	plcd->SetCursor(2, 0);
	plcd->WriteString( sbuff, font5x5, White);

	bool bquit = false;
	mux.lock();
	if ( ! this->bActive )
		bquit = true;
	mux.unlock();

	if ( bquit )
	{
		plcd->SetCursor(32, 15);
		plcd->WriteString( "no data", font6x8, White);
		plcd->UpdateScreen();

		return;
	}


	this->grpMotor.mux.lock();
	chsnprintf( sbuff, BUFF_SIZE, "o:%5d", this->grpMotor.output );
	this->grpMotor.mux.unlock();
//		plcd->SetCursor(0, offset+37 );
	plcd->SetCursor(0, 7 );
	plcd->WriteString( sbuff, font5x7, White);



	// StG
	this->grpStGauge.mux.lock();
	for ( int i=0; i<STGAUGE_CH_COUNT; ++i)
	{
		chsnprintf( sbuff, BUFF_SIZE, "%9d", this->grpStGauge.tab[i] );
		plcd->SetCursor(64, offset+i*8);
		plcd->WriteString( sbuff, font5x7, White);
	}
	this->grpStGauge.mux.unlock();

	// ADC
	this->grpAdc.mux.lock();
	for ( int i=0; i<ADC_CH_COUNT; ++i)
	{
		chsnprintf( sbuff, BUFF_SIZE, "%d: %4d %d", i, this->grpAdc.tab[i].Mean(), this->grpAdc.tab[i].Deviation() );
		plcd->SetCursor(0, offset+i*8 );
		plcd->WriteString( sbuff, font5x7, White);
	}

	uint16_t itemp = adc_to_temp( this->grpAdc.tab[ID_CH_MPUTEMP].Mean() );
	this->grpAdc.mux.unlock();

	chsnprintf( sbuff, BUFF_SIZE, "%d.%0dc", itemp/100, itemp%100 );
	plcd->SetCursor(64, 7 );
	plcd->WriteString( sbuff, font5x7, White);

	// RPM
	this->grpRpm.mux.lock();
	chsnprintf( sbuff, BUFF_SIZE, "%r: %4d", this->grpRpm.freq );
	plcd->SetCursor(0, offset+5*8 );
	plcd->WriteString( sbuff, font5x7, White);
	this->grpRpm.mux.unlock();

//	uint16_t* cal1 = (uint16_t*)0x1fff7a2c;
//	uint16_t* cal2 = (uint16_t*)0x1fff7a2e;
//	chsnprintf( sbuff, BUFF_SIZE, "cal: %d %d", *cal1, *cal2 );
//	plcd->SetCursor(2, 56);
//	plcd->WriteString( sbuff, font5x7, White);


	// ESC Telem
	this->grpEscTelem.mux.lock();
//		if ( this->grpEscTelem.temp != 0xff )
	{
//		chsnprintf( sbuff, BUFF_SIZE, "t:%3dC", this->grpEscTelem.temp );
//		plcd->SetCursor(0, offset+32 );
//		plcd->WriteString( sbuff, font5x7, White);

		chsnprintf( sbuff, BUFF_SIZE, "t:%5d c", this->grpEscTelem.temp );
		plcd->SetCursor(64, offset+16 );
		plcd->WriteString( sbuff, font5x7, White);

		chsnprintf( sbuff, BUFF_SIZE, "v:%3d.%02dV", this->grpEscTelem.volt/100, this->grpEscTelem.volt%100 );
		plcd->SetCursor(64, offset+24 );
		plcd->WriteString( sbuff, font5x7, White);

		chsnprintf( sbuff, BUFF_SIZE, "c:%3d.%02dA", this->grpEscTelem.curr/100, this->grpEscTelem.curr%100 );
		plcd->SetCursor(64, offset+40 );
		plcd->WriteString( sbuff, font5x7, White);

		uint32_t rpm = (((uint32_t)this->grpEscTelem.rpm)*100)/7;	// assuming 14 poles motor
		if (this->grpMotor.output <= 1050 )
			rpm = 0;

		chsnprintf( sbuff, BUFF_SIZE, "r:%6d", rpm );
		plcd->SetCursor(64, offset+32 );
		plcd->WriteString( sbuff, font5x7, White);
	}
	this->grpEscTelem.mux.unlock();

	plcd->UpdateScreen();
}



