//=======================================================================
// out.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "out.h"
#include <string>

using std::string;
using std::cerr;
using namespace Porg;

int Out::s_verbosity = QUIET;


void Out::vrb(string const& msg, int errno_ /* = 0 */)
{
	if (verbose()) {
		if (errno_)
			cerr << "porg: " << msg << ": " << strerror(errno_) << "\n";
		else
			cerr << msg << "\n";
	}
}


void Out::dbg(string const& msg, bool print_prog_name /* = true */)
{
	if (debug())
		cerr << (print_prog_name ? "porg :: " : "") << msg << '\n';
}


void Out::dbg_title(string const& title /* = "" */)
{
	if (!debug())
		return;
	
	string head("porg :: ----");
	cerr << head;
	int cnt = head.size();

	if (title.size()) {
		string str(string("[ ") + title + " ]");
		cerr << str;
		cnt += str.size();
	}
	
	int width = DEFAULT_SCREEN_WIDTH;
	if (width > cnt)
		cerr << string(width - cnt, '-');
	
	cerr << '\n';
}

