//=======================================================================
// find.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_FIND_H
#define GROP_FIND_H

#include "config.h"
#include "pkg.h"
#include <gtkmm/dialog.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>


namespace Grop
{

class MainWindow;

class Find : public Gtk::Dialog
{
	class PkgsTreeView : public Gtk::TreeView
	{
		friend class Find;

		class ModelColumns : public Gtk::TreeModel::ColumnRecord
		{
			friend class Find;

			ModelColumns() 
			{ 
				add(m_name);
				add(m_pkg);
			}
			
			Gtk::TreeModelColumn<Glib::ustring>	m_name;
			Gtk::TreeModelColumn<Pkg*>			m_pkg;

		};	// class Find::ModelColumns

		PkgsTreeView(MainWindow&);

		void on_selection_changed();
		virtual bool on_button_press_event(GdkEventButton*);
		virtual bool on_key_press_event(GdkEventKey*);

		ModelColumns                    m_columns;
		Glib::RefPtr<Gtk::ListStore>	m_model;
		Pkg*							m_selected_pkg;
		MainWindow&						m_mainwindow;
	};

	public:

	static void instance(MainWindow&);

	private:

	Find(MainWindow&);

	Gtk::Entry		m_entry;
	PkgsTreeView	m_treeview;

	static Find* s_find;

	Gtk::TreeModel::iterator reset_treeview();
	void browse();
	void find();
	virtual void on_response(int id);
};

} // namespace Grop


#endif  // GROP_FIND_H

