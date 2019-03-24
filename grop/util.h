//=======================================================================
// util.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_UTIL_H
#define GROP_UTIL_H 1

#include "config.h"
#include <iosfwd>
#include <gtkmm/window.h>

namespace Grop
{

	extern void main_iter();
	extern bool run_question_dialog(std::string const&, Gtk::Window*);
	extern void run_error_dialog(std::string const&, Gtk::Window*);

}	// namespace Grop

#endif	// GROP_UTIL_H
