//=======================================================================
// newpkg.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_NEW_PKG_H
#define PORG_NEW_PKG_H

#include "config.h"
#include "porg/basepkg.h"
#include <iosfwd>
#include <set>


namespace Porg
{

class NewPkg : public BasePkg
{
	public:

	NewPkg(std::string const& name_, std::set<std::string> const& files);
	
	protected:

	void print_info_dbg() const;
	void get_icon_path();
	void get_configure_options();
	void read_spec();
	void read_pc();
	void read_desktop();
	void read_config();

};	// class NewPkg

}	// namespace Porg


#endif  // PORG_NEW_PKG_H

