//=======================================================================
// db.h
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_DB_H
#define GROP_DB_H

#include "config.h"
#include "pkg.h"
#include "porg/basepkg.h"
#include <vector>
#include <iosfwd>

namespace Grop {


class DB
{
	public:

	typedef std::vector<Pkg*>::iterator			iter;
	typedef std::vector<Pkg*>::const_iterator	const_iter;
	
	static void init();

	static float total_size()			{ return s_total_size; }
	static std::vector<Pkg*>& pkgs()	{ return s_pkgs; }
	static bool initialized()			{ return s_initialized; }
	static int pkg_cnt()				{ return s_pkgs.size(); }

	static void remove_pkg(Pkg*);

	protected:

	DB();
	~DB();

	static std::vector<Pkg*> s_pkgs;
	static float s_total_size;
	static bool s_initialized;

};

}	// namespace Grop


#endif  // GROP_DB_H
