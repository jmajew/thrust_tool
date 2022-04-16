#ifndef __LATEX_GENERATOR__
#define __LATEX_GENERATOR__


#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
// class LatexGenerator
////////////////////////////////////////////////////////////////////////////////////////////////////
template <class Generator>
class LatexGenerator
{
public:
	LatexGenerator( const Generator& gen) : mGen(gen)	{}

	void	Execute( const std::string& out_name);

	void	SetTmplName( const std::string& name)		{ mTmplName = name; }

private:

	void	RunLoop( std::ofstream& fout, const std::vector<std::string>& tab);

private:
	const Generator&	mGen;
	std::string			mTmplName;
	std::string			mOutName;
};

#endif // __LATEX_GENERATOR__
