//=======================================================================
// file.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef LIBPORG_FILE_H
#define LIBPORG_FILE_H

#include "config.h"
#include <string>


namespace Porg {

class File
{
	public:

	File(std::string const& name_);
	File(std::string const& name_, ulong size_, std::string const& ln_name_ = "");

	ulong size() const					{ return m_size; }
	std::string const& name() const		{ return m_name; }
	std::string const& ln_name() const	{ return m_ln_name; }
	ino_t inode() const					{ return m_inode; }
	bool is_symlink() const				{ return !m_ln_name.empty(); }
	bool is_missing() const;

	private:

	std::string const m_name;
	ulong m_size;

	// inode of file. Used to detect hardlinks.
	ino_t m_inode;
	
	// if the file is a symlink, name of the file it refers to,
	// or an empty string otherwise
	std::string m_ln_name;	

};	// class File

}	// namespace Porg


#endif  // LIBPORG_FILE_H
