//=======================================================================
// removepkg.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_REMOVE_PKG_H
#define GROP_REMOVE_PKG_H

#include "config.h"
#include "pkg.h"
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/expander.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>


namespace Grop {

class RemovePkg : public Gtk::Dialog
{
	public:

	static bool instance(Pkg&, Gtk::Window&);

	private:

	RemovePkg(Pkg&, Gtk::Window&);

	void on_expander_changed();
	void remove();
	void remove_parent_dir(std::string const&);
	void report(std::string const&, Glib::RefPtr<Gtk::TextTag> const&);

	bool 							m_error;
	Gtk::Label						m_label;
	Gtk::ProgressBar				m_progressbar;
	Gtk::Expander					m_expander;
	Gtk::Button						m_button_close;
	Glib::RefPtr<Gtk::TextBuffer>	m_text_buffer;
	Gtk::TextView					m_text_view;
	Glib::RefPtr<Gtk::TextTag> 		m_tag_ok;
	Glib::RefPtr<Gtk::TextTag> 		m_tag_skipped;
	Glib::RefPtr<Gtk::TextTag> 		m_tag_error;
	Pkg&							m_pkg;
};

} // namespace Grop


#endif  // GROP_REMOVE_PKG_H

