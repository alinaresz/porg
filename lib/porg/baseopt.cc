//=======================================================================
// baseopt.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "baseopt.h"
#include "rexp.h"
#include "common.h"
#include <wordexp.h>
#include <fstream>

using std::string;
using namespace Porg;

static string sh_expand(string const&);

string BaseOpt::s_logdir		= LOGDIR;
string BaseOpt::s_include		= "/";
string BaseOpt::s_exclude		= EXCLUDE;
string BaseOpt::s_remove_skip	= "";


BaseOpt::BaseOpt()
{
	std::ifstream porgrc(PORGRC);
	assert(porgrc.is_open());
	if (!porgrc)
		return;

	Rexp re("^([a-z_]+)=(.*)$", REG_ICASE);

	for (string buf, opt, val; getline(porgrc, buf); ) {

		if (re.exec(buf)) {
			
			opt = Porg::to_lower(re.match(1));
			val = re.match(2);
			
			if (opt == "logdir")
				s_logdir = sh_expand(val);
			else if (opt == "include")
				s_include = val;
			else if (opt == "exclude")
   				s_exclude = val;
			else if (opt == "remove_skip")
   				s_remove_skip = val;
		}
	}
}


bool BaseOpt::logdir_writable()
{
	return !access(s_logdir.c_str(), W_OK);
}


static string sh_expand(string const& str)
{
	wordexp_t p;

	if (0 != wordexp(str.c_str(), &p, WRDE_NOCMD))
		return str;
 
 	string ret = p.we_wordv[0];
	for (uint i = 1; i < p.we_wordc; ++i)
		ret += string(" ") + p.we_wordv[i];

	wordfree(&p);
  	return ret;
}

