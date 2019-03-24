//=======================================================================
// filestreeview.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_FILES_TREE_VIEW_H
#define GROP_FILES_TREE_VIEW_H

#include "config.h"
#include "pkg.h"
#include <iosfwd>
#include <gtkmm/treeview.h>
#include <gtkmm/liststore.h>


namespace Grop
{

class FilesTreeView : public Gtk::TreeView
{
	class ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
		friend class FilesTreeView;

		ModelColumns()
		{
			add(m_name);
			add(m_size);
			add(m_file);
		}

		Gtk::TreeModelColumn<File*>			m_file;
		Gtk::TreeModelColumn<Glib::ustring>	m_name;
		Gtk::TreeModelColumn<ulong>			m_size;

	};	// class FilesTreeView::ModelColumns

	public:

	FilesTreeView(Pkg const&);

	private:

	void fill_model();
	void add_columns();

	Pkg const&						m_pkg;
	ModelColumns					m_columns;
	Glib::RefPtr<Gtk::ListStore>	m_model;

	void size_cell_func(Gtk::CellRenderer*, Gtk::TreeModel::iterator const&);
	void name_cell_func(Gtk::CellRenderer*, Gtk::TreeModel::iterator const&);

};		// class FilesTreeView

}		// namespace Grop

#endif	// GROP_FILES_TREE_VIEW_H
