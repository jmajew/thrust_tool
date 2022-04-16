#include "appsetup.h"


bool ToolSetup::initChannels( ThspMessage& msg)
{
	SBuf	sbin( msg.data);

	mcountMotorCh = sbin.ReadU8();
	mcountStgCh = sbin.ReadU8();
	mcountAdcCh = sbin.ReadU8();
	mcountRpmCh = sbin.ReadU8();
	mcountEsctCh = sbin.ReadU8();

	if (mcountMotorCh != MOTOR_CH_COUNT ||
		mcountStgCh != STGAUGE_CH_COUNT ||
		mcountAdcCh != ADC_CH_COUNT ||
		mcountRpmCh != RPM_CH_COUNT ||
		mcountEsctCh != ESCTELEM_CH_COUNT  )
	{
		return false;
	}

	return true;
}


void ToolSetup::initCfgMotor( ThspMessage& msg)
{
	SBuf	sbin( msg.data);
	mConfig.groupMotor.Unpack( sbin);
}

void ToolSetup::initCfgStg( ThspMessage& msg)
{
	SBuf	sbin( msg.data);
	mConfig.groupStrain.Unpack( sbin);
}

void ToolSetup::initCfgAdc( ThspMessage& msg)
{
	SBuf	sbin( msg.data);
	mConfig.groupADC.Unpack( sbin);
}

void ToolSetup::initCfgRpm( ThspMessage& msg)
{
	SBuf	sbin( msg.data);
	mConfig.groupRpm.Unpack( sbin);
}

void ToolSetup::initCfgConvert( ThspMessage& msg)
{
	SBuf	sbin( msg.data);
	mConfig.groupConvert.Unpack( sbin);
}



void ToolSetup::packCfgMotor( ThspMessage& msg)
{
	SBuf	sbout( msg.data);
	mConfig.groupMotor.Pack( sbout);

	msg.command = THSP_SET_CONFIG_MOTOR;
	msg.packetType = THSP_PACKET_COMMAND;
	msg.dataSize = (uint16_t)sbout.Length();
}

void ToolSetup::packCfgStg( ThspMessage& msg)
{
	SBuf	sbout( msg.data);
	mConfig.groupStrain.Pack( sbout);

	msg.command = THSP_SET_CONFIG_STGAUGE;
	msg.packetType = THSP_PACKET_COMMAND;
	msg.dataSize = (uint16_t)sbout.Length();
}

void ToolSetup::packCfgAdc( ThspMessage& msg)
{
	SBuf	sbout( msg.data);
	mConfig.groupADC.Pack( sbout);

	msg.command = THSP_SET_CONFIG_ADC;
	msg.packetType = THSP_PACKET_COMMAND;
	msg.dataSize = (uint16_t)sbout.Length();
}

void ToolSetup::packCfgRpm( ThspMessage& msg)
{
	SBuf	sbout( msg.data);
	mConfig.groupRpm.Pack( sbout);

	msg.command = THSP_SET_CONFIG_RPM;
	msg.packetType = THSP_PACKET_COMMAND;
	msg.dataSize = (uint16_t)sbout.Length();
}

void ToolSetup::packCfgConvert( ThspMessage& msg)
{
	SBuf	sbout( msg.data);
	mConfig.groupConvert.Pack( sbout);

	msg.command = THSP_SET_CONFIG_CONVERT;
	msg.packetType = THSP_PACKET_COMMAND;
	msg.dataSize = (uint16_t)sbout.Length();
}
