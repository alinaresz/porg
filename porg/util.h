//=======================================================================
// util.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_UTIL_H
#define PORG_UTIL_H

#include "config.h"
#include <dirent.h>
#include <iosfwd>


namespace Porg
{
	std::string clear_path(std::string const&);


class Dir
{
	public:

	Dir(std::string const&);
	~Dir();

	bool read(std::string&);
	void rewind();

	private:

	DIR* m_dir;
	struct dirent* m_dirent;
	
};	// class Dir

}	// namespace Porg

#endif  // PORG_UTIL_H

