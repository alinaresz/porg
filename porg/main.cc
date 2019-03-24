//=======================================================================
// main.cc - The main source file.
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "logger.h"
#include "db.h"
#include "main.h"

using namespace Porg;

// Initialization of global vars
int Porg::g_exit_status = EXIT_SUCCESS;


int main(int argc, char* argv[])
{
	std::ios::sync_with_stdio(false);

	try 
	{
		Opt::init(argc, argv);

		if (Opt::mode() == MODE_LOG) {
			Logger::run();
			return g_exit_status;
		}

		DB db;

		if (Opt::mode() == MODE_QUERY || Opt::all_pkgs())
			db.get_pkgs_all();
		else
			db.get_pkgs(Opt::args());

		if (db.empty())
			return g_exit_status;

		db.sort_pkgs(Opt::sort_type(), Opt::reverse_sort());

		switch (Opt::mode()) {
			case MODE_CONF_OPTS:	db.print_conf_opts();	break;
			case MODE_INFO:			db.print_info();		break;
			case MODE_LIST_PKGS:	db.list_pkgs();			break;
			case MODE_LIST_FILES:	db.list_files();		break;
			case MODE_REMOVE:		db.remove();			break;
			case MODE_QUERY:		db.query();				break;
			default: 				assert(0);				break;
		}
	}

	catch (std::exception const& x) 
	{
		std::cerr << "porg: " << x.what() << '\n';
		g_exit_status = EXIT_FAILURE;
	}

	return g_exit_status;
}

