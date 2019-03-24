//=======================================================================
// lock.h
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_LOCK_H
#define GROP_LOCK_H

#include "config.h"
#include <iosfwd>


namespace Grop {


class Lock
{
	public:

	static void init();

	protected:

	Lock();
	~Lock();

	static void stop();
	static void signal_handler(int);
	
	static std::string const s_lockfile;
};

}	// namespace Grop


#endif  // GROP_LOCK_H
