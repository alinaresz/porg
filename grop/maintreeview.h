//=======================================================================
// maintreeview.h
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_MAIN_TREE_VIEW_H
#define GROP_MAIN_TREE_VIEW_H

#include "config.h"
#include "pkg.h"
#include <iosfwd>
#include <gtkmm/liststore.h>
#include <gtkmm/treeview.h>


namespace Grop
{

class MainTreeView : public Gtk::TreeView
{
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		friend class MainTreeView;

		ModelColumns()
		{
			add(m_name);
			add(m_size);
			add(m_nfiles);
			add(m_date);
			add(m_summary);
			add(m_pkg);
		}

		Gtk::TreeModelColumn<Pkg*>			m_pkg;
		Gtk::TreeModelColumn<Glib::ustring>	m_name;
		Gtk::TreeModelColumn<float>			m_size;
		Gtk::TreeModelColumn<int>			m_date;
		Gtk::TreeModelColumn<gulong>		m_nfiles;
		Gtk::TreeModelColumn<Glib::ustring>	m_summary;

	};	// class MainTreeView::ModelColumns

	public:

	MainTreeView();
	
	enum {
		COL_NAME,
		COL_SIZE,
		COL_NFILES,
		COL_DATE,
		COL_SUMMARY,
		NCOLS
	};

	// signals for interclass comunication
	sigc::signal<void, GdkEventButton*> signal_popup_menu;
	sigc::signal<void, GdkEventButton*> signal_2button_press;
	sigc::signal<void, GdkEventKey*> signal_key_press;
	sigc::signal<void, Pkg*> signal_pkg_selected;

	void reset_opts();
	void remove_pkg(Pkg const* const);
	void scroll_to_pkg(Pkg*);

	private:

	typedef Gtk::TreeModel::iterator iterator;

	void add_columns();
	void set_columns_visibility();
	void fill_model();
	void size_cell_func(Gtk::CellRenderer*, iterator const&);
	void date_cell_func(Gtk::CellRenderer*, iterator const&);
	
	// overriden signal handlers
	virtual bool on_button_press_event(GdkEventButton*);
	virtual bool on_key_press_event(GdkEventKey*);

	void on_selection_changed();
	bool on_refresh_date(iterator const&);

	ModelColumns					m_columns;
	Glib::RefPtr<Gtk::ListStore>	m_model;

};	// class MainTreeView

} 	// namespace Grop

#endif  // GROP_MAIN_TREE_VIEW_H
