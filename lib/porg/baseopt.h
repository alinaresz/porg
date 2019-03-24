//=======================================================================
// baseopt.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef LIBPORG_BASEOPT_H
#define LIBPORG_BASEOPT_H

#include "config.h"
#include <string>

namespace Porg {

//
// Common options for porg and grop, set in configuration file porgrc.
//
class BaseOpt
{
	public:
	
	static std::string const& logdir()		{ return s_logdir; }
	static std::string const& include()		{ return s_include; }
	static std::string const& exclude()		{ return s_exclude; }
	static std::string const& remove_skip()	{ return s_remove_skip; }
	
	static bool logdir_writable();

	protected:

	BaseOpt();
	virtual ~BaseOpt() { };

	static std::string s_logdir;
	static std::string s_include;
	static std::string s_exclude;
	static std::string s_remove_skip;

};	// class BaseOpt

}	// namespace Porg

#endif  // LIBPORG_BASEOPT_H

