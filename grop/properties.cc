//=======================================================================
// properties.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "properties.h"
#include "filestreeview.h"
#include "infotextview.h"
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/label.h>
#include <gtkmm/stock.h>

using namespace Gtk;


Grop::Properties::Properties(Grop::Pkg const& pkg, Window& parent)
:
	Dialog("grop :: properties", parent),
	m_notebook()
{
	set_border_width(4);
	set_default_size(500, 500);

	Label* label = manage(new Label());
	Box* label_box = manage(new Box());
	label->set_markup("Package <b>" + pkg.name() + "</b>");
	label_box->pack_start(*label, PACK_SHRINK);

	Box* box = get_content_area();
	box->set_spacing(4);
	box->pack_start(*label_box, PACK_SHRINK);
	box->pack_start(m_notebook, PACK_EXPAND_WIDGET);

	ScrolledWindow* scrolled_window_files = manage(new ScrolledWindow());
	scrolled_window_files->add(*(manage(new Grop::FilesTreeView(pkg))));
	m_notebook.append_page(*scrolled_window_files, "Files");

	ScrolledWindow* scrolled_window_info = manage(new ScrolledWindow());
	scrolled_window_info->add(*(manage(new Grop::InfoTextView(pkg))));
	m_notebook.append_page(*scrolled_window_info, "Info");

	add_button(Stock::CLOSE, RESPONSE_CLOSE);

	show_all();
	run();
}


void Grop::Properties::instance(Pkg const& pkg, Window& parent)
{
	Grop::Properties properties(pkg, parent);
}

