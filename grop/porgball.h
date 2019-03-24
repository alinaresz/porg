//=======================================================================
// porgball.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_PORGBALL_H
#define GROP_PORGBALL_H

#include "config.h"
#include "pkg.h"
#include <gtkmm/dialog.h>
#include <gtkmm/label.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/filechooserbutton.h>
#include <glibmm/spawn.h>

namespace Grop
{

class Porgball : public Gtk::Dialog
{
	public:

	static void instance(Pkg const&, Gtk::Window&);

	protected:

	Porgball(Pkg const&, Gtk::Window&);
	~Porgball();

	virtual bool on_delete_event(GdkEventAny*);

	typedef struct {
		std::string	folder;
		int			prog;
		int			level;
		bool		test;
		bool		porg_suffix;
	} Last;
		
	enum { PROG_GZIP, PROG_BZIP2, PROG_XZ };

	static Last s_last;

	Pkg const&				m_pkg;
	Gtk::Label				m_label_progress;
	Gtk::Label				m_label_tarball;
	Gtk::ComboBoxText		m_combo_prog;
	Gtk::ComboBoxText		m_combo_level;
	Gtk::FileChooserButton	m_filechooser_button;
	Gtk::CheckButton		m_button_test;
	Gtk::CheckButton		m_button_porg_suffix;
	Gtk::Button*			m_button_close;
	Gtk::Button*			m_button_cancel;
	Gtk::Button*			m_button_ok;
	Gtk::ProgressBar		m_progressbar;
	std::string				m_tmpfile;
	Glib::Pid				m_pid;
	bool					m_close;
	std::vector<Gtk::Widget*> m_children;

	void set_children_sensitive(bool = true);
	void on_cancel();
	void set_tarball_suffix();
	bool create_porgball();
	bool spawn(std::vector<std::string>&);
	void end_create(bool done = true);
};

} // namespace Grop


#endif  // GROP_PORGBALL_H
