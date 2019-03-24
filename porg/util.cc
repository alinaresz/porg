//=======================================================================
// util.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "util.h"
#include "porg/common.h"	// Error, strip_trailing()
#include <string>

using std::string;
using namespace Porg;


//
// Like libc's realpath(), but it only resolves symlinks in the partial
// directories of the path, thereby retaining symlinks as symlinks.
//
string Porg::clear_path(string const& inpath)
{
	if (inpath.empty())
		return inpath;

	string path(inpath);

	// absolutize path

	if (path[0] != '/') {
		char cwd[4096];
		if (getcwd(cwd, sizeof(cwd)))
			path.insert(0, string(cwd) + "/");
	}

	path = strip_trailing(path, '/');

	// separate dirname from basename

	string::size_type p = path.rfind('/');
	string base((p == string::npos) ? "" : path.substr(p + 1));
	string dir(path.substr(0, p));

	// get realpath of dirname

	char real_dir[4096];

	if (!::realpath(dir.c_str(), real_dir))
		return path;

	return string(real_dir) + "/" + base;
}


Dir::Dir(string const& path)
:
	m_dir(opendir(path.c_str())),
	m_dirent(0)
{
	if (!m_dir)
		throw Error("opendir(\"" + path + "\")", errno);
}


Dir::~Dir()
{
	if (m_dir)
		closedir(m_dir);
}


bool Dir::read(string &name)
{
	if (!(m_dirent = readdir(m_dir)))
		return false;
	
	name = m_dirent->d_name;

	// skip hidden files and special files '.' and '..'
	if (name.empty() || name[0] == '.')
		return read(name);

	return true;
}


void Dir::rewind()
{
	rewinddir(m_dir);
}

