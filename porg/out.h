//=======================================================================
// out.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_OUT_H
#define PORG_OUT_H

#include "config.h"
#include <iosfwd>


namespace Porg {

class Out
{
	public:

	static void inc_verbosity()	{ s_verbosity++; }
	static bool debug()			{ return s_verbosity >= DEBUG; }
	static bool verbose()		{ return s_verbosity >= VERBOSE; }

	static void vrb(std::string const&, int errno_ = 0);
	static void dbg(std::string const&, bool print_prog_name = true);
	static void dbg_title(std::string const& title = "");

	protected:

	enum { QUIET, VERBOSE, DEBUG };

	static const int DEFAULT_SCREEN_WIDTH = 70;

	static int s_verbosity;
};

}	// namespace Porg


#endif  // PORG_OUT_H

