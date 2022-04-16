#ifndef __APPSETUP_H__
#define __APPSETUP_H__


#include "config.hpp"
#include "thsp_packet.hpp"



////////////////////////////////////////////////////////////////////////////////////////////////////
// class ToolSetup
////////////////////////////////////////////////////////////////////////////////////////////////////
class ToolSetup
{
public:
	ToolSetup() : mcountMotorCh(0), mcountStgCh(0), mcountAdcCh(0), mcountRpmCh(0), mcountEsctCh(0) {}

	size_t			Size() const		{ return mcountMotorCh + mcountStgCh + mcountAdcCh + mcountRpmCh + mcountEsctCh; }
	
	Config&			rConfig()			{ return mConfig; }
	const Config&	cConfig() const		{ return mConfig; }
	
	
	bool	initChannels( ThspMessage& msg);

	void	initCfgMotor( ThspMessage& msg);
	void	initCfgStg( ThspMessage& msg);
	void	initCfgAdc( ThspMessage& msg);
	void	initCfgRpm( ThspMessage& msg);
	void	initCfgConvert( ThspMessage& msg);

	void	packCfgMotor( ThspMessage& msg);
	void	packCfgStg( ThspMessage& msg);
	void	packCfgAdc( ThspMessage& msg);
	void	packCfgRpm( ThspMessage& msg);
	void	packCfgConvert( ThspMessage& msg);

private:
	size_t	mcountMotorCh;
	size_t	mcountStgCh;
	size_t	mcountAdcCh;
	size_t	mcountRpmCh;
	size_t	mcountEsctCh;

// tool config
	Config	mConfig;
};
	


#endif // __APPSETUP_H__
