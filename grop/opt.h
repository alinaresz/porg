//=======================================================================
// opt.h
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_OPT_H
#define GROP_OPT_H

#include "config.h"
#include "porg/baseopt.h"
#include <glibmm/keyfile.h>
#include <glibmm/optioncontext.h>
#include <vector>


namespace Grop {


class Opt : public Porg::BaseOpt, public Glib::KeyFile
{
	public:

	static bool& hour()						{ return s_hour; }
	static int width()						{ return s_width; }
	static int height()						{ return s_height; }
	static int xpos()						{ return s_xpos; }
	static int ypos()						{ return s_ypos; }
	static std::vector<bool>& columns()		{ return s_columns; }
	static bool initialized()				{ return s_initialized; }
	static Glib::OptionContext& context()	{ return s_context; }

	static void set_whxy(int w, int h, int x, int y)
		{ s_width = w; s_height = h; s_xpos = x; s_ypos = y; }

	static void init();

	protected:

	Opt();
	~Opt();

	void read_config_file();
	void set_command_line_options();
	
	std::string m_groprc;

	static bool s_hour;
	static int s_width;
	static int s_height;
	static int s_xpos;
	static int s_ypos;
	static std::vector<bool> s_columns;
	static bool s_initialized;
	
	static Glib::OptionContext s_context;

	static int const DEFAULT_WIDTH	= 500;
	static int const DEFAULT_HEIGHT	= 500;
	static int const DEFAULT_XPOS	= 0;
	static int const DEFAULT_YPOS	= 0;

};

}	// namespace Grop


#endif  // GROP_OPT_H
