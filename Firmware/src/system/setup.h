/*
 * setup.h
 *
 *  Created on: Mar 12, 2019
 *      Author: jmajew
 */
#ifndef __SYSTEM_SETUP_H__
#define __SYSTEM_SETUP_H__

#include "io/usbcfg.h"
//extern SerialUSBDriver SDU1;


////////////////////////////////////////////////////////////
// micro wait timer

#define TIMD_WAIT			GPTD4


////////////////////////////////////////////////////////////
// strain gauge

#define LINE_HX_SCK 		PAL_LINE(GPIOC,4)
#define LINE_HX_RATE 		PAL_LINE(GPIOC,5)
#define LINE_HX_DATA1		PAL_LINE(GPIOB,0)
#define LINE_HX_DATA2		PAL_LINE(GPIOB,1)
#define LINE_HX_DATA3		PAL_LINE(GPIOB,2)


////////////////////////////////////////////////////////////
// adc

#define LINE_ADC_INPUT1		PAL_LINE(GPIOA,0)	// RefV
#define LINE_ADC_INPUT2		PAL_LINE(GPIOA,1)	// BattV
#define LINE_ADC_INPUT3		PAL_LINE(GPIOA,4)	// BattCurrV
#define LINE_ADC_INPUT4		PAL_LINE(GPIOA,5)	// BattCurrZeroV

#define ADC_CHANNEL_1		ADC_CHANNEL_IN0
#define ADC_CHANNEL_2		ADC_CHANNEL_IN1
#define ADC_CHANNEL_3		ADC_CHANNEL_IN4
#define ADC_CHANNEL_4		ADC_CHANNEL_IN5


////////////////////////////////////////////////////////////
// motor

#define MOTOR_PWMD			PWMD1
#define MOTOR_CH			2					// counting from 0
#define LINE_MOTOR_OUT		PAL_LINE(GPIOA,10)	// TIM1_CH3


////////////////////////////////////////////////////////////
// esc telemetry

#define SD_ESC_TELEM		SD6
#define LINE_ESC_TELEM		PAL_LINE(GPIOC,7)	// UART6_RX


////////////////////////////////////////////////////////////
// rpm icu

#define RPM_ICUD			ICUD3
#define RPM_ICU_CH			ICU_CHANNEL_2
#define LINE_RPM			PAL_LINE(GPIOA,7)	// TIM3_CH1


////////////////////////////////////////////////////////////
// lcd comm

#define LCD_IF_I2C

#define LCD_IF				I2CD3
#define LINE_I2C_SDA		PAL_LINE(GPIOB,4)	// I2C1_SDA
#define LINE_I2C_SCL		PAL_LINE(GPIOA,8)	// I2C1_SCL

////----------------------------------------------------------
// #define LCD_IF_SPI
// #define LCD_CS_BY_SPIIF
//
// #define LCD_IF				SPID3
// #define LINE_LCD_SPI_SCK	PAL_LINE(GPIOC,10)
// #define LINE_LCD_SPI_MISO	PAL_LINE(GPIOC,11)
// #define LINE_LCD_SPI_MOSI	PAL_LINE(GPIOC,12)
// #define LINE_LCD_SPI_CS		PAL_LINE(GPIOA,15)
//
// #define LINE_LCD_SPI_DC		PAL_LINE(GPIOD,2)
// #define LINE_LCD_RESET		PAL_LINE(GPIOB,4)


////////////////////////////////////////////////////////////
// misc

#define LINE_DEBUG_1		PAL_LINE(GPIOB,8)
#define LINE_DEBUG_2		PAL_LINE(GPIOB,9)

#define LINE_BUTTON 		PAL_LINE(GPIOC,8)

#define LINE_LED_GREEN 		PAL_LINE(GPIOC,10)
#define LINE_LED_YELLOW 	PAL_LINE(GPIOC,11)
#define LINE_LED_BLUE 		PAL_LINE(GPIOC,12)

#define LINE_BATT_CURR_FAULT	PAL_LINE(GPIOB,5)



#define LINE_OTG_VBUS		PAL_LINE(GPIOA,9)

////////////////////////////////////////////////////////////
// serial comm

// UART UART1
#define UART_TX		PAL_LINE(GPIOB,6)
#define UART_RX		PAL_LINE(GPIOB,7)


//#define SUSB				SDU1
#define SDA					SD1

#define SD_OUTPUT	SDA
#define SD_SERIAL	SDU1




//#define INPUT_ADCD			ADCD1
//#define INPUT_CH_VSUP		3
//#define INPUT_CH_VREF		4
//#define INPUT_CH_VCUR		6
//#define INPUT_CH_VCURZ		7
//#define INPUT_CH_VBAT		5
////#define INPUT_CH_JOYX		0
////#define INPUT_CH_JOYY		1

//#define LINE_HX_THRUST		LINE_HX_DATA1
//#define LINE_HX_TORQUE_L	LINE_HX_DATA2
//#define LINE_HX_TORQUE_R	LINE_HX_DATA3


//#define SD_OUTPUT			SDA
//#define SD_SERIAL			SUSBD



//// obsolete
//
//#define SPI_LCD				SPID3
//#define LINE_SPI_LCD_CS		PAL_LINE(GPIOA,15)


#ifdef __cplusplus
extern "C"
{
#endif

void gpioInit(void);

#ifdef __cplusplus
}
#endif



#endif // __SYSTEM_SETUP_H__
