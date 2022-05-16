/*
 * util.cpp
 *
 *  Created on: 17.10.2018
 *      Author: jmajewski
 */

#include "util/util.hpp"
#include "util/waitmicro.hpp"


uint8_t shiftIn( ioline_t lineSCK, ioline_t lineDATA, uint8_t bitOrder)
{
	uint8_t value = 0;
	uint8_t i;

	for ( i = 0; i<8; ++i)
	{
		palSetLine( lineSCK );
		gptWaitMicro( 1); // 1 us delay

		if (bitOrder == LSBFIRST)
			value |= palReadLine( lineDATA) << i;
		else
			value |= palReadLine( lineDATA) << (7 - i);

		palClearLine( lineSCK );
		gptWaitMicro( 1); // 1 us delay
	}

	return value;
}


void shiftInArray( ioline_t lineSCK, const ioline_t* tabLineDATA, uint8_t* tabOut, uint8_t length, uint8_t bitOrder)
{
	uint8_t i, k;

	for ( k=0; k<length; ++k)
		tabOut[k] = 0;

	for ( i = 0; i<8; ++i)
	{
		palSetLine( lineSCK );
		gptWaitMicro( 1);

		for ( k=0; k<length; ++k)
		{
			if (bitOrder == LSBFIRST)
			{
				tabOut[k] |= palReadLine( tabLineDATA[k]) << i;
			}
			else
			{
				tabOut[k] |= palReadLine( tabLineDATA[k]) << (7 - i);
			}
		}
		palClearLine( lineSCK );
		gptWaitMicro( 1);
	}

}

