//=======================================================================
// lock.cc
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "lock.h"
#include "porg/common.h"
#include <csignal>
#include <fstream>
#include <glibmm/miscutils.h>

using namespace Grop;

std::string const Lock::s_lockfile = Glib::get_tmp_dir() + "/.grop.lock";


Lock::Lock()
{
	if (!access(s_lockfile.c_str(), F_OK))
		throw Porg::Error("Grop is already running");
	
	// set signal_handler

	struct sigaction sa;
	memset(&sa, 0, sizeof(struct sigaction));
	
	sa.sa_handler = signal_handler;	
	sigaction(SIGHUP,  &sa, NULL);
	sigaction(SIGINT,  &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);	
	
	// prevent child processes from becoming zombies
	sa.sa_handler = SIG_IGN;
	sigaction(SIGCHLD, &sa, NULL);

	// create lock file

	std::ofstream f(s_lockfile.c_str());
}


Lock::~Lock()
{
	stop();
}


void Lock::init()
{
	static Lock lock;
}


void Lock::stop()
{
	remove(s_lockfile.c_str());
}



void Lock::signal_handler(int)
{
	stop();
	exit(EXIT_SUCCESS);
}

