//=======================================================================
// db.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "db.h"
#include "util.h"
#include <gtkmm/messagedialog.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/image.h>
#include <glibmm/fileutils.h>	// Dir

using std::string;
using namespace Grop;
using namespace Gtk;

float 				DB::s_total_size = 0;
std::vector<Pkg*> 	DB::s_pkgs;
bool				DB::s_initialized = false;


DB::DB()
{
	g_return_if_fail(Opt::initialized());

	MessageDialog dialog("Reading database  (" + Opt::logdir() + ")", false, 
		MESSAGE_INFO, BUTTONS_NONE, true);
	dialog.set_title("grop :: info");
	
	ProgressBar* progressbar(manage(new ProgressBar()));
	dialog.get_message_area()->pack_start(*progressbar);
	dialog.set_image(*(manage(new Image(DATADIR "/pixmaps/grop.png"))));
	dialog.show_all();

	Glib::Dir dir(Opt::logdir());
	int pkg_cnt = std::vector<string>(dir.begin(), dir.end()).size();	
	s_pkgs.reserve(pkg_cnt);
	float cnt = 0;

	for (Glib::DirIterator d = dir.begin(); d != dir.end(); ++d) {

		if (!Glib::file_test(Opt::logdir() + "/" + *d, Glib::FILE_TEST_IS_REGULAR))
			continue;

		dialog.set_secondary_text(*d);
		main_iter();
		
		try 
		{	
			Pkg* pkg = new Pkg(*d);
			pkg->read_log();
			s_pkgs.push_back(pkg);
			s_total_size += pkg->size();
		}
		catch (std::exception const& x) 
		{
			g_warning("%s", x.what()); 
		}
		
		progressbar->set_fraction(cnt++ / pkg_cnt);
		main_iter();
	}

	s_initialized = true;
}


DB::~DB()
{
	for (const_iter p(s_pkgs.begin()); p != s_pkgs.end(); delete *p++) ;
}


void DB::init()
{
	static DB db;
}


void DB::remove_pkg(Pkg* pkg)
{
	g_return_if_fail(pkg != NULL);

	pkg->unlog();

	for (iter p(s_pkgs.begin()); p != s_pkgs.end(); ++p) {
		if (*p == pkg) {
			s_total_size -= pkg->size();
			s_pkgs.erase(p);
			break;
		}
	}
			
	delete pkg;
	pkg = NULL;
}

