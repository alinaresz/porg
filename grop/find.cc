//=======================================================================
// find.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "find.h"
#include "db.h"
#include "mainwindow.h"
#include <gtkmm/grid.h>
#include <gtkmm/button.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/stock.h>
#include <gtkmm/filechooserdialog.h>

using namespace Grop;
using namespace Gtk;

Find* Find::s_find = 0;


Find::Find(MainWindow& parent)
:
	Dialog("grop :: find file", parent, true),
	m_entry(),
	m_treeview(parent)
{
	set_border_width(8);
	set_default_size(300, 200);
	
	Button* button_browse = manage(new Button("_Browse", true));
	button_browse->signal_clicked().connect(sigc::mem_fun(*this, &Find::browse));

	Grid* grid = manage(new Grid());
	grid->set_column_spacing(get_border_width());
	grid->attach(m_entry, 0, 0, 1, 1);
	grid->attach(*button_browse, 1, 0, 1, 1);

	ScrolledWindow* scrolled_window = manage(new ScrolledWindow());
	scrolled_window->add(m_treeview);

	Box* box = get_content_area();
	box->set_spacing(get_border_width());
	box->pack_start(*grid, PACK_SHRINK);
	box->pack_start(*scrolled_window, PACK_EXPAND_WIDGET);

	add_button(Stock::CLOSE, RESPONSE_CLOSE);
	add_button(Stock::FIND, RESPONSE_APPLY);

	set_default_response(RESPONSE_APPLY);
	m_entry.set_activates_default();
	m_entry.set_hexpand();

	get_action_area()->set_layout(BUTTONBOX_EDGE);

	show_all();
}


void Find::instance(MainWindow& parent)
{
	if (!s_find)
		s_find = new Find(parent);

	s_find->m_entry.set_text("");
	s_find->reset_treeview();
	s_find->run();
}


void Find::on_response(int id)
{
	if (id == RESPONSE_APPLY)
		find();
	else
		hide();
}


TreeModel::iterator Find::reset_treeview()
{
	m_treeview.m_model->clear();
	return m_treeview.m_model->append();
}


void Find::find()
{
	TreeModel::iterator i = reset_treeview();
	Glib::ustring path(m_entry.get_text());

	if (path.empty())
		return;
	
	(*i)[m_treeview.m_columns.m_name] = "(file not found)";
	(*i)[m_treeview.m_columns.m_pkg] = 0;

	if (path[0] != '/')
		return;
	
	int cnt = 0;

	for (DB::const_iter p = DB::pkgs().begin(); p != DB::pkgs().end(); ++p) {
		if ((*p)->find_file(path)) {
			if (cnt++)
				i = m_treeview.m_model->append();
			(*i)[m_treeview.m_columns.m_name] = (*p)->name();
			(*i)[m_treeview.m_columns.m_pkg] = *p;
		}
	}
}


void Find::browse()
{
	FileChooserDialog dialog(*this, "Select file");

	dialog.add_button(Stock::CANCEL, RESPONSE_CANCEL);
	dialog.add_button(Stock::OK, RESPONSE_OK);
	dialog.set_show_hidden();
	dialog.set_filename(m_entry.get_text());
	
	if (dialog.run() == RESPONSE_OK) {
		m_entry.set_text(dialog.get_filename());
		m_entry.set_position(-1);
	}
}


//--------------------//
// Find::PkgsTreeView //
//--------------------//


Find::PkgsTreeView::PkgsTreeView(MainWindow& parent)
:
	m_columns(),
	m_model(ListStore::create(m_columns)),
	m_selected_pkg(0),
	m_mainwindow(parent)
{
	set_model(m_model);
	set_headers_visible(false);
	append_column("", m_columns.m_name);

	get_selection()->signal_changed().connect(
		sigc::mem_fun(this, &Find::PkgsTreeView::on_selection_changed));
}


void Find::PkgsTreeView::on_selection_changed()
{
	TreeModel::iterator i = get_selection()->get_selected();
	m_selected_pkg = i ? (*i)[m_columns.m_pkg] : static_cast<Pkg*>(0);
}


//
// Double clicking on a row makes the main treeview in the main window
// scroll to the selected package.
//
bool Find::PkgsTreeView::on_button_press_event(GdkEventButton* event)
{
	bool handled = TreeView::on_button_press_event(event);

	if (m_selected_pkg && event->window == get_bin_window()->gobj()
	&& event->button == 1 && event->type == GDK_2BUTTON_PRESS)
		m_mainwindow.scroll_to_pkg(m_selected_pkg);
	
	return handled;
}


//
// Pressing <Return> makes the main treeview in the main window
// scroll to the selected package.
//
bool Find::PkgsTreeView::on_key_press_event(GdkEventKey* event)
{
	bool handled = TreeView::on_key_press_event(event);
	
	if (m_selected_pkg && event->keyval == GDK_KEY_Return)
		m_mainwindow.scroll_to_pkg(m_selected_pkg);
	
	return handled;
}

