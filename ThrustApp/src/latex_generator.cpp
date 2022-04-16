#include <iostream>
#include <fstream>
#include <regex>

#include <QDebug>
#include <QMessageBox>

#include "latex_generator.h"
#include "document.h"

const char	STR_COMMENT[] = "//";

// removes chars in 'pat' from the front and the back of 'str'
inline void Strip( std::string& str, const std::string& pat)
{
	if ( str.empty() ) 
		return;

	size_t startIndex = str.find_first_not_of( pat.c_str());
	size_t endIndex = str.find_last_not_of( pat.c_str());
	std::string tempstr = str;

	str.erase();
	if ( startIndex == std::string::npos )
		return;

	str = tempstr.substr( startIndex, ( endIndex - startIndex + 1) );
}


// removes blank chars from the front and the back of 'str'
inline void StripBlanks( std::string& str)
{
	Strip( str, " \t\n\r");
}

// read line to std::string 'buf'
// 'comm' comments out to the end of the line (default is '//')
inline void ReadLine( std::istream& f, std::string& buf, const std::string& comm = STR_COMMENT)
{
	getline( f, buf);
	buf = buf.substr(0,buf.find( comm));
}

// read first non-empty line to std::string 'buf'
// 'comm' comments out to the end of the line (default is '//')
inline void ReadLineNonEmpty( std::istream& f, std::string& buf, const std::string& comm = STR_COMMENT)
{
	std::string stmp;
	do
	{
		getline( f, buf);
		buf = buf.substr(0,buf.find( comm) );
		stmp = buf;
		StripBlanks( stmp);
	}
	while ( stmp.size() == 0  &&  ! f.eof() );
}


////////////////////////////////////////////////////////////////////////////////////////////////////
// class LatexGenerator
////////////////////////////////////////////////////////////////////////////////////////////////////

const char LGEN_LOOP_BEG[]		= "LOOP_BEG";
const char LGEN_LOOP_ID[]		= "LOOP_ID";
const char LGEN_DATA_COMMA[]	= "DATA_COMMA";
const char LGEN_DATA_TABLE[]	= "DATA_TABLE";
const char LGEN_LOOP_END[]		= "LOOP_END";
const char LGEN_TOOL_INFO[]		= "TOOL_INFO";
const char LGEN_SETUP_INFO[]	= "SETUP_INFO";



template <class Generator>
void LatexGenerator<Generator>::Execute( const std::string& out_name)
{
	mOutName = out_name;

	std::ofstream fout( mOutName );
	if ( ! fout)
	{
		qDebug() <<"can not open the file: " << QString( mOutName.c_str() );
		QMessageBox::warning( nullptr,	("Warning"), ("LatexGenerator: Can't open the output file:") + QString(mOutName.c_str()) );
		return;
	}

	std::ifstream fin( mTmplName );
	if ( ! fin)
	{
		qDebug() <<"can not open the file: " << QString( mTmplName.c_str() );
		QMessageBox::warning( nullptr,	("Warning"), ("LatexGenerator: Can't open the template file:") + QString(mTmplName.c_str()) );
		return;
	}

	std::string buf;
	std::regex	rex;
	std::smatch rex_match;

	rex = "<<<([_A-Z]*)>>>";

	do
	{
		ReadLine( fin, buf, "%%%" );

		if ( regex_search( buf, rex_match, rex) )
		{
			std::string name = rex_match[1];
			qDebug() << "found: " << QString(name.c_str());

			if ( name == LGEN_TOOL_INFO )
			{
				mGen.WriteToolInfo_LATEX( fout);
				continue;
			}
			else if ( name == LGEN_SETUP_INFO )
			{
				mGen.WriteSetupInfo_LATEX( fout);
				continue;
			}
			else if ( name == LGEN_LOOP_BEG )
			{
				std::vector<std::string> tab_buf;
				do
				{
					ReadLine( fin, buf, "%%%" );
					if ( regex_search( buf, rex_match, rex) )
					{
						std::string name = rex_match[1];
						qDebug() << "found: " << QString(name.c_str());
	
						if ( name == LGEN_LOOP_END )
						{
							buf = "";
							break;
						}
					}

					tab_buf.push_back( buf);
				}
				while( ! fin.eof() );

				// process tab_buf
				if ( tab_buf.size() > 0 )
					RunLoop( fout, tab_buf);
			}
		}

		fout << buf << "\n";
	}
	while( ! fin.eof() );

}

template <class Generator>
void LatexGenerator<Generator>::RunLoop( std::ofstream& fout, const std::vector<std::string>& tab)
{
	std::regex	rex;
	std::regex	rex_cmd;
	std::smatch rex_match;

	rex_cmd = "<<<([_A-Z]*)>>>";
	rex = std::string("<<<") + std::string(LGEN_LOOP_ID) + std::string(">>>");

	for ( int i=0; i<mGen.cResult().Size(); ++i)
	{
		for ( auto itr=tab.begin(); itr!=tab.end(); ++itr)
		{
			std::string buf = *itr;

			if ( regex_search( buf, rex_match, rex_cmd) )
			{
				std::string name = rex_match[1];

				if ( name == LGEN_DATA_COMMA )
				{
					//fout << "write data comma\n";
					fout << "out,      th,    th_err, tq,     tq_err,  rpm,   rpm_err,  v,    v_err,   a,     a_err,   pin,        theff\n";
					mGen.cResult().cItem(i).WriteDataTable( fout, ",", "" );
				}
				else if ( name == LGEN_DATA_TABLE )
				{
					//fout << "write data table\n";
					mGen.cResult().cItem(i).WriteDataTable( fout, " & ", " \\\\" );
				}
				else if ( name == LGEN_LOOP_ID )
				{
					std::string nbuf = std::regex_replace( buf, rex, std::to_string(i+1) );
					fout <<  nbuf << "\n";
				}

			}
			else
			{
				fout <<  buf << "\n";
			}

		}
	}
}

template class LatexGenerator<Document>;

