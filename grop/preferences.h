//=======================================================================
// preferences.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_PREFERENCES_H
#define GROP_PREFERENCES_H

#include "config.h"
#include "maintreeview.h"
#include <gtkmm/dialog.h>
#include <gtkmm/checkbutton.h>

namespace Grop {


class Preferences : public Gtk::Dialog
{
	public:

	static int instance(Gtk::Window&);

	private:

	Preferences(Gtk::Window&);

	void load_opts();
	void save_opts();
	
	Gtk::CheckButton	m_buttons[MainTreeView::NCOLS];
	Gtk::CheckButton	m_button_hour;

	static Preferences*	s_prefs;
};

} // namespace Grop


#endif  // GROP_PREFERENCES_H

