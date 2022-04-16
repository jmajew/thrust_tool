#ifndef __SSD1306_COMMANDS_H__
#define __SSD1306_COMMANDS_H__

//#define SSD1306_DEFAULT_ADDRESS  		0x78


#define SSD1306_SET_CONTRAST      		0x81
#define SSD1306_OUTPUT_FOLLOWS_RAM	 	0xA4
#define SSD1306_OUTPUT_IGNORE_RAM     	0xA5
#define SSD1306_NORMAL_DISPLAY    		0xA6
#define SSD1306_INVERT_DISPLAY    		0xA7
#define SSD1306_SET_MULTIPLEX  		   	0xA8
#define SSD1306_DISPLAY_OFF       		0xAE
#define SSD1306_DISPLAY_ON        		0xAF

#ifdef SSD1306_USE_SH1106
	#define SSD1306_SET_LOW_COLUMN   	  	0x02	// sh1106
	#define SSD1306_SET_HIGH_COLUMN   	 	0x10
#else
	#define SSD1306_SET_LOW_COLUMN   	  	0x00	// ssd1306
	#define SSD1306_SET_HIGH_COLUMN   	 	0x10
#endif // SSD1306_USE_SH1106

#define SSD1306_COLUMN_ADDR       		0x21
#define SSD1306_MEMORY_MODE     	  	0x20
#define SSD1306_ADDR_MODE_HORIZON		0x00
#define SSD1306_ADDR_MODE_VERT			0x01
#define SSD1306_ADDR_MODE_PAGE			0x02

#define SSD1306_PAGE_ADDR         		0x22
#define SSD1306_SET_START_PAGE          0xB0	// page addressing B0 - B7
#define SSD1306_SET_START_LINE   	  	0x40
#define SSD1306_SEG_REMAP_0        		0xA0
#define SSD1306_SEG_REMAP_127      		0xA1

#define SSD1306_COMSCAN_INC       		0xC0
#define SSD1306_COMSCAN_DEC       		0xC8
#define SSD1306_SET_DISPLAY_OFFSET 		0xD3
#define SSD1306_SET_COM_PINS       		0xDA
#define SSD1306_SET_DISPLAY_CLOCK_DIV 	0xD5
#define SSD1306_SET_PRECHARGE     		0xD9
#define SSD1306_SET_VCOM_DETECT    		0xDB

#define SSD1306_SWITCHCAPVCC     		0x02
#define SSD1306_NOP              		0xE3

#define SSD1306_CHARGE_PUMP       		0x8D


#endif // __SSD1306_COMMANDS_H__
