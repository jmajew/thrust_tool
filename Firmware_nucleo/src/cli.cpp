
#include "hal.h"
#include "shell.h"

#include "cli.hpp"

#include "io/io.hpp"
#include "util/util.hpp"

#include "config_def.hpp"
#include "config.hpp"
#include "master.hpp"

#include "flash/flash.hpp"

// configured via linker script when building binaries.
extern uint8_t __config_start;
extern uint8_t __config_end;

extern uint8_t __flash_start;
extern uint8_t __flash_end;

extern uint8_t __ram_start;
extern uint8_t __ram_end;

extern Master master;

BaseSequentialStream* CH_SDU = (BaseSequentialStream*) &SD_OUTPUT;


static Config cli_config = {};


//===========================================================================
// Command line related.
//===========================================================================

#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(2048)
//#define SHELL_WA_SIZE   THD_WORKING_AREA_SIZE(4096)

static thread_t *shelltp = NULL;


// Shell history buffer
char history_buffer[SHELL_MAX_HIST_BUFF];

// Shell completion buffer
char *completion_buffer[SHELL_MAX_COMPLETIONS];


static uint8_t buf[] =
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
				"0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef";


// Can be measured using dd if=/dev/xxxx of=/dev/null bs=512 count=10000.
static void cmd_write( BaseSequentialStream *chp, int argc, char *argv[])
{

	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: write\r\n");
		return;
	}

	while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT )
	{
#if 1
		/* Writing in channel mode.*/
		chnWrite( chp, buf, sizeof buf - 1);
#else
		/* Writing in buffer mode.*/
		(void) obqGetEmptyBufferTimeout( &(SD_OUTPUT.obqueue), TIME_INFINITE);
		memcpy(SD_OUTPUT.obqueue.ptr, buf, SERIAL_USB_BUFFERS_SIZE);
		obqPostFullBuffer( &(SD_OUTPUT.obqueue), SERIAL_USB_BUFFERS_SIZE);
#endif
	}
	chprintf(chp, "\r\n\nstopped\r\n");
}


/* Can be measured using dd if=/dev/xxxx of=/dev/null bs=512 count=10000.*/
static void cmd_flashinfo( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	if (argc > 0)
	{
		chprintf(chp, "Usage: flashinfo\r\n");
		return;
	}

	chprintf(chp, "git rev. = %s\r\n", sgitRev );

	chprintf(chp, "__config_start = %x\r\n", &__config_start );
	chprintf(chp, "__config_end   = %x\r\n", &__config_end );
	chprintf(chp, "sizeof(config) = %x\r\n", sizeof( Config ) );
}


static void cmd_setvar( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	if ( argc != 2 )
	{
		chprintf(chp, "Usage: setvar [VAR_NAME] [VALUE]\r\n" );
		return;
	}

	char *cmnd = argv[0];
	char *parm = argv[1];

    const VarDef *curval = NULL;

    uint32_t i, k;
    for ( i = 0; i < sizeCliVar; i++)
    {
        curval = &tabCliVar[i];
        if ( strncasecmp( cmnd, tabCliVar[i].name, strlen(tabCliVar[i].name)) == 0
        	 && strlen( cmnd) == strlen(tabCliVar[i].name))
        {
            bool bChange = false;
            Var value = {.int32 = 0 };

    		switch ( curval->varType & VALUE_MODE_MASK)
    		{
			case MODE_DIRECT:
				{
					value.int32 = atoi( parm);

					if ( value.int32 >= curval->config.valuedef.min && value.int32 <= curval->config.valuedef.max)
					{
						bChange = true;
					}
				}
				break;

			case MODE_LOOKUP:
				{
					const LookupTableEntry *tabEntry = &lookupTables[ curval->config.lookup.tableIndex ];
					bool bFound = false;
					for ( k = 0; k < tabEntry->valueCount && !bFound; ++k)
					{
						bFound = strcasecmp( tabEntry->values[k], parm) == 0;

						if ( bFound)
						{
							value.int32 = k;
							bChange = true;
						}
					}
				}
				break;
            }

            if ( bChange)
            {
                cliSetVar( &cli_config, curval, value);

                chprintf(chp, "%s set to ", curval->name);
                cliPrintValue(chp, &cli_config, curval);
                chprintf(chp, "\r\n");
            }
            else
            {
                chprintf(chp, "Invalid value\r\n");
                cliPrintValueDef(chp, curval);
            }

            return;

        }
    }
}

static void cmd_getvar( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

//	chprintf(chp, "argc = %d\r\n", argc );
//	for ( int i=0; i<argc; ++i)
//		chprintf(chp, "argv[%d] = %s\r\n", i, argv[i] );

	uint32_t i;
	static char s[] = "";

	char *ptr = s;
	if ( argc >= 1 )
		ptr = argv[0];

    const VarDef *val;
    int matchedCommands = 0;

    for ( i = 0; i < sizeCliVar; i++)
    {
        if ( strstr(tabCliVar[i].name, ptr ) )
        {
            val = &tabCliVar[i];
            chprintf(chp, "%s = ", tabCliVar[i].name);
            cliPrintValue(chp, &cli_config, val);
            chprintf(chp, "\r\n");;

            matchedCommands++;
        }
    }


    if ( matchedCommands)
        return;

    chprintf(chp, "Invalid name");
}


static void cmd_readflash( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	EFlashError error = Flash::ReadData( &cli_config, sizeof( Config ) );

	// 	TODO check this
	memcpy( master.pConfig(), &cli_config, sizeof(Config) );

	if ( error )
	    chprintf(chp, "Error reading flash: %d\r\n", error);
	else
		chprintf(chp, "Config read from flash\r\n");
}


static void cmd_writeflash( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	// 	TODO check this
	memcpy( master.pConfig(), &cli_config, sizeof( Config ) );

	EFlashError error = Flash::WriteData( master.pConfig(), sizeof( Config ) );

	if ( error )
	    chprintf(chp, "Error writing flash: %d\r\n", error);
	else
		chprintf(chp, "Config written to flash\r\n");

	chThdSleepMilliseconds(1000);

	// TODO reset or restart systme

	//restart
	//chprintf(chp, "Restarting\r\n");
	//NVIC_SystemReset();
}


static void cmd_eraseflash( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	EFlashError error = Flash::EraseDataSector();

	if ( error )
	    chprintf(chp, "Error erasing flash: %d\r\n", error);
	else
		chprintf(chp, "Flash erased succesfully\r\n");
}

//	0x08000000 - 512k
//	0x20000000 - 96k
static void cmd_dumpflash( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	uint16_t n;
	uint8_t* ptr = (uint8_t*)&__config_start;

	if ( argc == 0 )
	{
		chprintf(chp, "Usage: dumpflash [SIZE] [[HEX_ADDRESS]]\r\n" );
		chprintf(chp, "valid flash: 0x%p-0x%p\r\n", &__flash_start, &__flash_end );
		chprintf(chp, "valid ram:   0x%p-0x%p\r\n\r\n", &__ram_start, &__ram_end );
		n = 128;
	}
	else
	{
		n = atoi( argv[0] );
	}

	if ( argc == 2 )
	{
		ptr = (uint8_t*) strtol( argv[1], NULL, 0 );
	}

	if (( (ptr   >= &__flash_start && ptr   < &__flash_end) && (ptr+n >= &__flash_start && ptr+n < &__flash_end) ) ||
	    ( (ptr   >= &__ram_start   && ptr   < &__ram_end)   && (ptr+n >= &__ram_start   && ptr+n < &__ram_end) )  )
	{
		chprintf(chp, "size = %d\r\n", n );

		uint16_t k = 0;
		do
		{
			chprintf(chp, "%08x | ", ptr+k );
			for ( uint16_t i=0; i<16; ++i, k++)
			{
				chprintf(chp, "%02x ", *(ptr+k) );
			}
			chprintf(chp, "\r\n" );
		}
		while ( k<n);

	}

}


static void cmd_reset( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	//restart

	chprintf(chp, "Restarting\r\n");
	NVIC_SystemReset();
}



static void cmd_new_command( BaseSequentialStream *chp, int argc, char *argv[])
{
	(void) argv;
	(void) argc;

	chprintf(chp, "HELLO !!! \r\n\r\n" );
}

// static void cmd_cdetect( BaseSequentialStream *chp, int argc, char *argv[])
// {
	// (void) argv;
	// (void) argc;

	// if ( palReadLine( LINE_CD) == PAL_HIGH )
		// chprintf(chp, "LINE_CD = HIGH !!! \r\n\r\n" );
	// else
		// chprintf(chp, "LINE_CD = LOW !!! \r\n\r\n" );
// }






static const ShellCommand commands[] =
	{
		{ "write", 		cmd_write },
		{ "flashinfo", 	cmd_flashinfo },
		{ "setvar", 	cmd_setvar },
		{ "getvar", 	cmd_getvar },
		{ "load", 		cmd_readflash },
		{ "save", 		cmd_writeflash },
		{ "eraseflash", cmd_eraseflash },
		{ "dump",  		cmd_dumpflash },
		{ "reset",		cmd_reset },
		{ "hello",		cmd_new_command },

		{ NULL, 		NULL }
	};

	
//static const ShellConfig shell_cfg1 =
static ShellConfig shell_cfg1 =
	{
		(BaseSequentialStream *) CH_SDU,
		commands,
		history_buffer,
		sizeof( history_buffer),
		completion_buffer
	};



//===========================================================================




	
//===========================================================================
//
void shellStart( BaseSequentialStream *psd)
{

    if ( !shelltp)
	{
    	dbg_puts( "Starting the shell");
    	memcpy( &cli_config, master.pConfig(), sizeof(Config) );

//    	chMtxLock( &mtxSerial);
    	shell_cfg1.sc_channel = psd;
    	shelltp = chThdCreateFromHeap( NULL, SHELL_WA_SIZE, "shell", NORMALPRIO + 1, shellThread, (void *)&shell_cfg1);
    	msg_t msg = chThdWait(shelltp);               // Waiting for exit

    	if (msg != MSG_OK)
    		dbg_printf( "Shell thread exit code: %d", msg);
    	else
    		dbg_puts( "Shell thread exit with OK");

		chThdSleepMilliseconds(1000);

//    	chMtxUnlock( &mtxSerial);
 	}

    if ( chThdTerminatedX( shelltp) )
	{
		shelltp = NULL; // Triggers spawning of a new shell.
	}
}

