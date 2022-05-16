/*
 * config_def.hpp
 *
 *  Created on: 23.03.2019
 *      Author: jmajew
 */

#ifndef __CONFIG_DEF_HPP__
#define __CONFIG_DEF_HPP__

#include "system/sysdecl.hpp"


#define VALUE_TYPE_OFFSET 0
#define VALUE_MODE_OFFSET 6

#define VALUE_TYPE_MASK (0x3F)	// bin 00111111
#define VALUE_MODE_MASK (0xC0)	// bin 11000000


enum EVarType
{
	// bits reserved : 0-5
	VARTP_NONE 		= 0,
	VARTP_INT8		= (1 << VALUE_TYPE_OFFSET),
	VARTP_UINT8		= (2 << VALUE_TYPE_OFFSET),
	VARTP_INT16		= (3 << VALUE_TYPE_OFFSET),
	VARTP_UINT16	= (4 << VALUE_TYPE_OFFSET),
	VARTP_INT32		= (5 << VALUE_TYPE_OFFSET),
	VARTP_UINT32	= (6 << VALUE_TYPE_OFFSET),
	VARTP_FLOAT		= (7 << VALUE_TYPE_OFFSET),

	// bits reserved : 6-7
    MODE_DIRECT 	= (0 << VALUE_MODE_OFFSET),
    MODE_LOOKUP 	= (1 << VALUE_MODE_OFFSET)
};



enum ELookupTableIndex
{
    TABLE_OFF_ON = 0,
    TABLE_MOTOR_PROTOCOL,
    TABLE_TENSOR_RATE,
    TABLE_TENSOR_GAIN,
    LOOKUP_TABLE_COUNT
};

union Var
{
    int8_t 		int8;
    uint8_t 	uint8;
    int16_t 	int16;
    uint16_t 	uint16;
    int32_t 	int32;
    uint32_t 	uint32;
    float		real;
};

struct VarValueDef
{
    const int32_t min;
    const int32_t max;
    const int32_t def;
};

struct LookupTableEntry
{
    const char* const*	values;
    const uint8_t 		valueCount;
};

struct LookupTableConfig
{
    const ELookupTableIndex	tableIndex;
    const uint16_t			defElementId;
};

union ValueConfig
{
	LookupTableConfig 	lookup;
	VarValueDef 		valuedef;
};



struct VarDef
{
	const char*	name;
	uint8_t		varType;	// def. cliVarType_e
	ValueConfig	config;
	uint32_t	offset;
}
__attribute__((packed));


extern const uint16_t* const pts_cal1;
extern const uint16_t* const pts_cal2;

extern const LookupTableEntry lookupTables[];

extern const VarDef	tabCliVar[];
extern const uint16_t sizeCliVar;

void cliVarBuildCheck(void);
void cliSetVar( void *pcfg, const VarDef *var, const Var value);
void cliSetVarDefault( void *pcfg, const VarDef *var);
void cliPrintValue( BaseSequentialStream *chp, const void *pcfg, const VarDef *var);
void cliPrintValueDef( BaseSequentialStream *chp, const VarDef *var);






#endif // __CONFIG_DEF_HPP__
