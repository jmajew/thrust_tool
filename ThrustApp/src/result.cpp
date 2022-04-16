#include <fstream>
#include <iomanip>

#include "result.h"
#include "measure_data.h"


void ResultRow::UpdateDepValues()
{
	ASSERT(0);
	mtab[RES_POWER_IN_ID] = mtab[RES_BATT_VOLT_ID].Mean() * mtab[RES_BATT_CURR_ID].Mean();
	//mtab[RES_POWER_OUT_ID] = mtab[RES_TORQUE_ID].Mean() * mtab[RES_RPM_ID].Mean() / 60.0 / 10000; // convert to N*m/s

	//mtab[RES_EFFICIENCY_ID] = 0.;
	mtab[RES_THRUST_EFF_ID] = 0.;

	//if ( mtab[RES_POWER_IN_ID] > 0.1)
	//{
	//}

}

void Result::WriteData( QTextStream& out, const MeasurementData* pdata) const
{
	for (const auto& def : cTabMarks() )
	{
		out << "\nMotor output = " << (int)def.first << "\n";

		out << "\n";
		out.setFieldWidth(14);
		out << "level [%]";
		out << "thrust [G]";
		out << "torque [G cm]";
		out << "rpm [1/min]";
		out << "volt [V]";
		out << "curr [A]";
		out.setFieldWidth(0);
		out << "\n";

		const TRange& range = def.second;
		for (int i = range.first; i < range.second; ++i)
		{
			out.setFieldWidth(14);

			out << pdata->getData( CH_EXT_MOTOR_OUT_ID, i);
		
			out << pdata->getData( CH_EXT_THRUST_ID, i);
			out << pdata->getData( CH_EXT_TORQUE_ID, i);
			out << pdata->getData( CH_EXT_RPM_ID, i);
			out << pdata->getData( CH_EXT_BATT_VOLT_ID, i);
			out << pdata->getData( CH_EXT_BATT_CURR_ID, i);

			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_MOTOR_OUT_ID, i) << "";	
			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_THRUST_ID, i) << "";
			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_TORQUE_ID, i) << "";
			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_RPM_ID, i) << "";
			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_BATT_VOLT_ID, i) << "";
			//out << qSetFieldWidth(10) << pdata->getData( CH_EXT_BATT_CURR_ID, i) << "";

			out.setFieldWidth(0);
			out << "\n";
		}
		out << "\n";;
	}
}

void Result::WriteProcessedData( QTextStream& out) const
{

	std::array<int,RES_COUNT> tabMPrec = {2, 2, 0, 3, 3, 3,  3};
	std::array<int,RES_COUNT> tabDPrec = {3, 3, 1, 3, 3, 3,  3};
	const int nsp_1 = 16;
	const int nsp_2 = 10;

	out << "\n";
	out.setFieldWidth(9);
	out << "level [%]";
	out.setFieldWidth(nsp_1);
	out << "thrust [G]";
	out << "torque [G cm]";
	out << "rpm [1/min]";
	out << "volt [V]";
	out << "curr [A]";
	out.setFieldWidth(nsp_2);
	out << "Pin [W]";
	//out << "Pout[W]";
	//out << "Eff [%]";
	out << "Eff [G/W]";
	out.setFieldWidth(0);
	out << "\n";

	for ( const auto& row : mtabRes )
	{
		if ( row.cLevel() == 0 )
			continue;

		//out.setRealNumberPrecision( 8);
		out << qSetFieldWidth(9) << row.cLevel();

		for ( int i=0; i<row.cData().size(); ++i )
		{
			const auto& val = row.cData()[i];

			QString smean = QString::number(val.Mean(), 'f', tabMPrec[i] );
			QString sdev = QString::number(val.Deviation(), 'f', tabDPrec[i] );

			int nsp = nsp_2;
			QString sval = smean;
			if ( i < RES_CH_COUNT )
			{
				nsp = nsp_1;
				sval += " [" + sdev +"]";
			}

			nsp -= sval.length();

			out << qSetFieldWidth(nsp) << " ";
			out << qSetFieldWidth(0) << sval;
		}

		out << "\n";
	}
}
//qSetRealNumberPrecision


void Result::WriteDataTable( std::ofstream& fout, const std::string& sep, const std::string& end) const
{
	std::array<int,RES_COUNT> tabMPrec = {2, 2, 0, 3, 3, 3,  3};
	std::array<int,RES_COUNT> tabDPrec = {3, 3, 1, 4, 4, 3,  3};
	const int nw = 11;

	for ( const auto& row : mtabRes )
	{
		if ( row.cLevel() == 0 )
			continue;

		//out.setRealNumberPrecision( 8);
		fout << std::setw(9) << std::setprecision(0) << row.cLevel();

		for ( int i=0; i<row.cData().size(); ++i )
		{
			const auto& val = row.cData()[i];

			fout << sep << " " << std::setw(nw) << std::setprecision(tabMPrec[i]) << std::fixed << val.Mean();
			if ( i < RES_CH_COUNT )
				fout << sep << " " << std::setw(nw) << std::setprecision(tabDPrec[i]) << std::fixed << val.Deviation();
		}

		fout << end <<  "\n";
	}

}


QDataStream& operator<<(QDataStream& out, const Result& res)
{	
	out << res.cTabMarks().size();
	for ( size_t i=0; i<res.cTabMarks().size(); ++i)
	{
		out << res.cTabMarks()[i].first;
		out << res.cTabMarks()[i].second.first;
		out << res.cTabMarks()[i].second.second;
	}

	out << res.cTabResult().size();
	for ( size_t i=0; i<res.cTabResult().size(); ++i)
	{
		out << res.cTabResult()[i].cLevel();
		for ( size_t k=0; k<RES_COUNT; ++k)
			out << res.cTabResult()[i].cData()[k];
	}

	return out;
}

QDataStream& operator>>(QDataStream& in, Result& res)
{
	size_t n;

	in >> n;
	res.rTabMarks().resize(n);
	for ( size_t i=0; i<res.cTabMarks().size(); ++i)
	{
		in >> res.rTabMarks()[i].first;
		in >> res.rTabMarks()[i].second.first;
		in >> res.rTabMarks()[i].second.second;
	}

	in >> n;
	res.rTabResult().resize(n);
	for ( size_t i=0; i<res.cTabResult().size(); ++i)
	{
		in >> res.rTabResult()[i].rLevel();
		for ( size_t k=0; k<RES_COUNT; ++k)
			in >> res.rTabResult()[i].rData()[k];
	}


	return in;
}


void ResultSet::WriteData(QTextStream& out, const MeasurementData* pdata) const
{
	int id = 0;
	for (const auto& res : mtabRes)
	{
		out <<"\n";
		out << "## Measurement - " << ++id << "\n";
		out << "comment: " << res.cText() << "\n\n";
		res.WriteProcessedData( out);
	}

	id = 0;
	for (const auto& res : mtabRes)
	{
		out <<"\n";
		out << "## Measurement - " << ++id << "\n\n";
		res.WriteData( out, pdata);
	}
}


QDataStream& operator<<(QDataStream& out, const ResultSet& res)
{
	return out;
}

QDataStream& operator>>(QDataStream& in, ResultSet& res)
{
	return in;
}


