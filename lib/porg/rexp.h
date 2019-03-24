//=======================================================================
// rexp.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_REXP_H
#define PORG_REXP_H

#include "config.h"
#include <iosfwd>
#include <regex.h>

namespace Porg
{

class Rexp
{
	public:

	Rexp(std::string const& exp = "", int flags = 0);
	~Rexp();

	bool compile(std::string const& exp, int flags = 0);
	bool exec(std::string const&);
	std::string match(int);
		
	private:

	static int const MAX_MATCHES = 8;

	regex_t		m_regex;
	regmatch_t	m_pmatch[MAX_MATCHES];
	std::string	m_str;
	bool		m_matched;
	bool 		m_compiled;
};

}


#endif
