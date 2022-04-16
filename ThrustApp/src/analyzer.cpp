#include <iostream>
#include <fstream>
#include <iomanip>

#include "analyzer.h"

#include "measure_data.h"


Analyzer::Analyzer() : mpData(nullptr)
{
}

void Analyzer::DoAnalyze( ResultSet& res, const TRange& range, const MeasurementData* pdata)
{
	if ( pdata->size() == 0 )
		return;

	if ( range.second > pdata->size() || range.first > range.second )
	{
		qDebug() << "Analyzer :: problem with ids : " << range.first << ", " << range.second;
		return;
	}

	res.Reset();

	mpData = pdata;
	mRange = range;

	//const auto& tab = result.cTabMarks();
	
	std::vector< TLevelDef > tab;
	FindMarks( tab);

	int dlev = 0;
	int plev = tab[0].first;

	int id_b = 0;
	int id = 0;

	for (int i = 1; i < tab.size(); ++i)
	{
		int clev = tab[i].first;

		qDebug() << " clev = " << clev <<  " plev = " << plev <<  " dlev = " << dlev;

		if  ( clev != 0 )
		{
			if (clev == plev || (dlev > 0 && plev > clev) || (dlev < 0 && plev < clev) )
			{
				// cut
				qDebug() << " ---- cut  from " << id_b << " - " << i;
				
				Result r;
				r.rTabMarks().resize( i-id_b);
				std::copy( tab.begin() + id_b, tab.begin() + i, r.rTabMarks().begin() );
				res.AddItem( r);

				id_b = id;
			}
			else
			{
				dlev = clev - plev > 0 ? 1 : (clev - plev < 0 ? -1 : 0);
			}

			plev = clev;
		}
		else
		{
			id = i;
		}
	}

	{
		qDebug() << " ---- cut  from " << id_b << " - " << tab.size();
		Result r;
		r.rTabMarks().resize( tab.size() - id_b);
		std::copy( tab.begin() + id_b, tab.end(), r.rTabMarks().begin() );
		res.AddItem( r);
	}

	qDebug() << "finish split";

	for (size_t i = 0; i < res.Size(); ++i)
	{
		AnalyzeItem( res.rItem(i) );
	}
}

void Analyzer::FindMarks( std::vector< TLevelDef >& tab)
{
	double lev = -1;
	int ilevbeg = mRange.first;

	for (int i = mRange.first; i < mRange.second; ++i)
	{
		double curlev = mpData->getData( CH_EXT_MOTOR_OUT_ID, i);
		if (curlev != lev)
		{
			if (lev >= 0)
			{
				TLevelDef def { lev, TRange{ilevbeg, i} };
				TRange r = std::make_pair(ilevbeg, i);
				tab.emplace_back( def);
			}

			lev = curlev;
			ilevbeg = i;
		}
	}

	if (lev >= 0)
	{
		TLevelDef def { lev, TRange{ilevbeg, mRange.second} };
		tab.emplace_back( def);
	}
}

StatisticValue Analyzer::ComputeChannel(int ch, const TLevelDef& def)
{
	ASSERT(mpData);
	const auto& channel = mpData->getChannel(ch);

	int pre_marg = mLeftMarg;
	int post_marg = mRightMarg;

	const TRange& range = def.second;

	StatisticValue sval;
	std::vector<double> tab;
	tab.reserve( range.second - range.first );
	for (int i = range.first + pre_marg; i < range.second - post_marg; ++i)
	{
		tab.push_back( channel[i] );
		sval.Accumulate( channel[i] );
	}

	double mean = sval.Mean();
	double dev = sval.Deviation();
	double marg = 1.*dev;

	std::vector<double> tab_fin;
	tab_fin.reserve( tab.size());

	if ( dev != 0 ) // possible spikes
	{
		// remove spikes
		for (auto e : tab)
		{
			if ( std::fabs( e - sval.Mean() ) < 2.*marg )
				tab_fin.push_back( e);
		}

	}
	else
	{
		tab_fin = tab;
	}


	//std::vector<double> tab_fin;
	//if ( def.first != 0 )
	//{
	//	// removing over/under shoots; calc deviation
	//	tab_fin.reserve( tab.size());
	//	double coeff = 0.5;
	//	for (auto e : tab)
	//	{
	//		if ( std::fabs( e - mean_1) < coeff * std::fabs(mean_1) )
	//			tab_fin.push_back( e);
	//	}
	//}
	//else
	//{
	//	tab_fin = tab;
	//}

	StatisticValue val;
	for (auto e : tab_fin)
		val.Accumulate( e);

	return val;
}


void Analyzer::AnalyzeItem(Result& result)
{
	for (const auto& def : result.rTabMarks() )
	{
		ResultRow rrow;
		rrow.rLevel() = def.first;
		rrow.rData()[RES_THRUST_ID]		= ComputeChannel( CH_EXT_THRUST_ID, def);
		rrow.rData()[RES_TORQUE_ID]		= ComputeChannel( CH_EXT_TORQUE_ID, def);
		rrow.rData()[RES_RPM_ID]		= ComputeChannel( CH_EXT_RPM_ID, def);
		rrow.rData()[RES_BATT_VOLT_ID]	= ComputeChannel( CH_EXT_BATT_VOLT_ID, def);
		rrow.rData()[RES_BATT_CURR_ID]	= ComputeChannel( CH_EXT_BATT_CURR_ID, def);

		//rrow.rData()[RES_POWER_IN_ID]	= ComputeChannel( CH_EXT_POWER_IN_ID, def);
		//rrow.rData()[RES_POWER_OUT_ID]	= ComputeChannel( CH_EXT_POWER_OUT_ID, def);
		//rrow.rData()[RES_EFFICIENCY_ID]	= ComputeChannel( CH_EXT_EFFICIENCY_ID, def);
		//rrow.rData()[RES_THRUST_EFF_ID]	= ComputeChannel( CH_EXT_THRUST_EFF_ID, def);

		result.rTabResult().push_back( rrow);
	}

	// calc zero
	ResultRow rzero;
	for (const auto& row : result.cTabResult() )
	{
		if (row.cLevel() == 0)
		{
			for ( int i=0; i<RES_COUNT; ++i)
				rzero.rData()[i].Accumulate( row.cData()[i]);
		}
	}

	// apply correction
	for (auto& row : result.rTabResult() )
	{
		//qDebug() << "LEVEL = " << row.cLevel();
		if (row.cLevel() != 0)
		{
			for ( int i=0; i<RES_CH_COUNT; ++i)
				if ( i != RES_BATT_VOLT_ID) // ew. RPM ??
				{
					size_t n0 = rzero.cData()[i].Count();
					double mean0 = rzero.cData()[i].Mean();
					double var0 = rzero.cData()[i].Variance();

					size_t n = row.cData()[i].Count();
					double mean = row.cData()[i].Mean();
					double var = row.cData()[i].Variance();

					//qDebug() << " mean0 = " << mean0 << " var0 = " << var0;
					//qDebug() << " mean = " << mean << " var = " << var;

					double nmean = mean - mean0;
					StatisticValue val( n, n* nmean, n* (var + var0 +  nmean*nmean ) );
					row.rData()[i] = val;

					//qDebug() << " new mean = " << row.cData()[i].Mean() << " new var = " << row.cData()[i].Variance();
					//qDebug() << " ";
				}

			
			double sx_pin = row.rData()[RES_BATT_VOLT_ID].Mean() * row.rData()[RES_BATT_CURR_ID].Mean();
			row.rData()[RES_POWER_IN_ID]	= StatisticValue( 1, sx_pin, sx_pin*sx_pin);

			//double sx_pout = row.rData()[RES_RPM_ID].Mean() * row.rData()[RES_TORQUE_ID].Mean() /60. / 10000.;
			//row.rData()[RES_POWER_OUT_ID]	= StatisticValue( 1, sx_pout, sx_pout*sx_pout);

			double sx_e =0.;
			double sx_te = 0;
			if ( sx_pin > 1.e-6 )
			{
				//sx_e = 100. * sx_pout / sx_pin;
				sx_te = row.rData()[RES_THRUST_ID].Mean() / sx_pin;
			}

			//row.rData()[RES_EFFICIENCY_ID]	= StatisticValue( 1, sx_e, sx_e*sx_e);
			row.rData()[RES_THRUST_EFF_ID]	= StatisticValue( 1, sx_te, sx_te*sx_te);
		}
	}
}
