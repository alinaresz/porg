//=======================================================================
// filestreeview.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "pkg.h"
#include "porg/file.h"
#include "filestreeview.h"

using namespace Grop;
using namespace Gtk;


FilesTreeView::FilesTreeView(Pkg const& pkg)
:
	TreeView(),
	m_pkg(pkg),
	m_columns(),
	m_model(ListStore::create(m_columns))
{
	set_rules_hint();
	set_vexpand();

	add_columns();
	fill_model();
	set_model(m_model);
}


void FilesTreeView::fill_model()
{
	g_warn_if_fail(m_model->children().empty());
	m_model->clear();

	for (uint i = 0; i < m_pkg.files().size(); ++i) {
		TreeModel::iterator it = m_model->append();
		File* file = m_pkg.files()[i];
		(*it)[m_columns.m_file] = file;
		(*it)[m_columns.m_name] = file->name();
		(*it)[m_columns.m_size] = file->size();
	}
}


void FilesTreeView::add_columns()
{
	int id;
	CellRenderer* cell;
	TreeViewColumn* col;

	id = append_column("Name", m_columns.m_name) - 1;
	cell = get_column_cell_renderer(id);
	col = get_column(id);
	col->set_cell_data_func(*cell, mem_fun(this, &FilesTreeView::name_cell_func));
	col->set_resizable();
	col->set_sort_column(id);

	id = append_column("Size", m_columns.m_size) - 1;
	cell = get_column_cell_renderer(id);
	col = get_column(id);
	cell->set_alignment(1, 0.5);
	col->set_cell_data_func(*cell, mem_fun(this, &FilesTreeView::size_cell_func));
	col->set_resizable();
	col->set_sort_column(id);
}


void FilesTreeView::name_cell_func(CellRenderer* cell, TreeModel::iterator const& it)
{
	CellRendererText* cell_text = static_cast<CellRendererText*>(cell);
	File* file = (*it)[m_columns.m_file];
	cell_text->property_foreground() = file->is_missing() ? "red" : "black";
}


void FilesTreeView::size_cell_func(CellRenderer* cell, TreeModel::iterator const& it)
{
	CellRendererText* cell_text = static_cast<CellRendererText*>(cell);
	File* file = (*it)[m_columns.m_file];
	cell_text->property_foreground() = file->is_missing() ? "red" : "black";
	cell_text->property_text() = Porg::fmt_size(file->size());
}

