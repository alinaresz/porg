//=======================================================================
// rexp.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "rexp.h"
#include <string>

using namespace Porg;
using std::string;


Rexp::Rexp(string const& exp /* = "" */, int flags /* = 0 */)
:
	m_regex(),
	m_pmatch(),
	m_str(),
	m_matched(false),
	m_compiled(exp.empty() ? false : compile(exp, flags))
{ }


Rexp::~Rexp()
{
	if (m_compiled)
		regfree(&m_regex);
}


bool Rexp::compile(string const& exp, int flags /* = 0 */)
{
	if (m_compiled)
		regfree(&m_regex);

	m_compiled = !regcomp(&m_regex, exp.c_str(), REG_EXTENDED | flags);
	assert(m_compiled);
	
	return m_compiled;
}


bool Rexp::exec(string const& str)
{
	m_str = str;
	m_matched = !regexec(&m_regex, str.c_str(), MAX_MATCHES, m_pmatch, 0);
	return m_matched;
}


string Rexp::match(int n /* = 0 */)
{
	assert(m_matched);
	assert(n < MAX_MATCHES);

	if (!m_matched || n >= MAX_MATCHES || m_pmatch[n].rm_so == -1)
		return "";

	return m_str.substr(m_pmatch[n].rm_so, m_pmatch[n].rm_eo - m_pmatch[n].rm_so);
}

