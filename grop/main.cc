//=======================================================================
// main.cc
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "db.h"
#include "mainwindow.h"
#include "util.h"
#include "lock.h"
#include <gtkmm/main.h>
#include <fstream>

static void show_error(std::string msg, bool gtk_started);


int main(int argc, char* argv[])
{
	bool gtk_started = false;

	try
	{
		Grop::Opt::init();
		Gtk::Main kit(argc, argv, Grop::Opt::context());
		gtk_started = true;
		Grop::Lock::init();
		Grop::DB::init();
		Grop::MainWindow window;
		kit.run(window);
	}

	catch (std::exception const& x)
	{
		show_error(x.what(), gtk_started);
	}
	
	catch (Glib::Exception const& x)
	{
		show_error(x.what(), gtk_started);
	}
}


void show_error(std::string msg, bool gtk_started)
{
	if (gtk_started)
		Grop::run_error_dialog(msg, 0);
	else
		std::cerr << "grop: " << msg << '\n';
}

