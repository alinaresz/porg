//=======================================================================
// opt.cc
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "mainwindow.h" // NCOLS
#include <glibmm/miscutils.h>
#include <fstream>

using std::string;
using std::vector;
using namespace Grop;


bool Opt::s_initialized	= false;
bool Opt::s_hour 		= false;
int Opt::s_width 		= Opt::DEFAULT_WIDTH;
int Opt::s_height 		= Opt::DEFAULT_HEIGHT;
int Opt::s_xpos 		= Opt::DEFAULT_XPOS;
int Opt::s_ypos 		= Opt::DEFAULT_YPOS;
vector<bool> Opt::s_columns;
Glib::OptionContext Opt::s_context;


Opt::Opt()
:
	Porg::BaseOpt(),
	Glib::KeyFile(),
	m_groprc()
{
	string dir(Glib::get_home_dir() + "/.config");
	mkdir(dir.c_str(), 0700);
	dir += "/grop";
	mkdir(dir.c_str(), 0700);
	m_groprc = dir + "/groprc";

	read_config_file();
	set_command_line_options();
	s_initialized = true;
}


Opt::~Opt()
{
	set_boolean("gui", "hour", s_hour);
	set_integer("gui", "width", s_width); 
	set_integer("gui", "height", s_height);
	set_integer("gui", "xpos", s_xpos);
	set_integer("gui", "ypos", s_ypos);
	set_boolean_list("gui", "columns", s_columns);

	std::ofstream os(m_groprc.c_str());
	if (!(os << to_data()))
		g_warning("Cannot open file '%s' for writing", m_groprc.c_str());
}


void Opt::read_config_file()
{
	try
	{
		load_from_file(m_groprc);

		s_hour 		= get_boolean("gui", "hour");
		s_width 	= get_integer("gui", "width");
		s_height 	= get_integer("gui", "height");
		s_xpos		= get_integer("gui", "xpos");
		s_ypos		= get_integer("gui", "ypos");
		s_columns	= get_boolean_list("gui", "columns");
	}
	catch (...) 
	{
		// On error, remove config file (will be rebuild in ~Opt())
		unlink(m_groprc.c_str());
	}

	if (s_columns.size() != MainTreeView::NCOLS)
		s_columns = vector<bool>(MainTreeView::NCOLS, true);
}


void Opt::set_command_line_options()
{
	static Glib::OptionGroup opt_group("grop", "Grop Options:");
	static Glib::OptionEntry opt_logdir;

	opt_logdir.set_long_name("logdir");
	opt_logdir.set_short_name('L');
	opt_logdir.set_description("Porg database directory (default is '" + s_logdir + "')");

	opt_group.add_entry_filename(opt_logdir, s_logdir);

	s_context.set_main_group(opt_group);
}


void Opt::init()
{
	static Opt opt;
}

