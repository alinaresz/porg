//=======================================================================
// pkg.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_PKG_H
#define PORG_PKG_H

#include "config.h"
#include "porg/basepkg.h"
#include <iosfwd>
#include <set>


namespace Porg
{

class DB;

class Pkg : public BasePkg
{
	public:

	Pkg(std::string const& name_);
	
	void unlog() const;
	void remove(DB const&);
	void print_conf_opts(bool print_pkg_name) const;
	void print_info() const;
	void list(int, int) const;
	void list_files(int size_w);
	void append(std::set<std::string> const& files);

};	// class Pkg

}	// namespace Porg


#endif  // PORG_PKG_H

