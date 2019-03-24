//=======================================================================
// file.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "file.h"

using std::string;
using namespace Porg;


//
// Ctor. for newly logged files
//
File::File(string const& name_)
:
	m_name(name_),
	m_size(0),
	m_inode(0),
	m_ln_name()
{
	struct stat s;

	if (lstat(m_name.c_str(), &s) < 0)
		return;

	else if (S_ISLNK(s.st_mode)) {
		char ln[4096];
		int cnt = readlink(m_name.c_str(), ln, sizeof(ln) - 1);
		if (cnt > 0) {
			ln[cnt] = 0;
			m_ln_name = ln;
		}
	}

	m_inode = s.st_ino;
	m_size = s.st_size;
}


//
// Ctor. for files read from database
//
File::File(string const& name_, ulong size_, string const& ln_name_ /* = "" */)
:
	m_name(name_),
	m_size(size_),
	m_inode(0),
	m_ln_name(ln_name_)
{ }


bool File::is_missing() const
{
	struct stat s;
	return lstat(m_name.c_str(), &s);
}

