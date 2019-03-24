//=======================================================================
// mainwindow.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "db.h"
#include "porg/file.h"
#include "mainwindow.h"
#include "properties.h"
#include "preferences.h"
#include "porgball.h"
#include "removepkg.h"
#include "util.h"	// run_*_dialog()
#include "find.h"
#include <gtkmm/box.h>
#include <gtkmm/stock.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/aboutdialog.h>
#include <gtkmm/scrolledwindow.h>


using std::string;
using namespace Gtk;
using namespace Grop;
using namespace sigc;


MainWindow::MainWindow()
:
	Window(),
	m_treeview(),
	m_statusbar(),
	m_uimanager(UIManager::create()),
	m_action_group(ActionGroup::create()),
	m_action_find(Action::create("FindFile", Stock::FIND)),
	m_action_properties(Action::create("Properties", Stock::PROPERTIES)),
	m_action_porgball(Action::create("Porgball", "Create porgball")),
	m_action_remove(Action::create("RemovePkg", Stock::REMOVE)),
	m_action_unlog(Action::create("UnlogPkg", "Unlog")),
	m_popup_menu(0),
	m_selected_pkg(0)
{
	g_return_if_fail(Opt::initialized() && DB::initialized());

	set_default_size(Opt::width(), Opt::height());
	move(Opt::xpos(), Opt::ypos());
	set_border_width(4);

	try { set_default_icon_from_file(DATADIR "/pixmaps/grop.png"); }
	catch (Glib::Error& x) { g_warning("%s", x.what().c_str()); }

	build_menu_bar();
	set_actions_sensitivity();

	// Handle events from the treeview
	m_treeview.signal_popup_menu.connect(mem_fun(this, &MainWindow::on_popup_menu));
	m_treeview.signal_2button_press.connect(mem_fun(this, &MainWindow::on_2button_press));
	m_treeview.signal_key_press.connect(mem_fun(this, &MainWindow::on_key_press));
	m_treeview.signal_pkg_selected.connect(mem_fun(this, &MainWindow::on_pkg_selected));

	m_statusbar.set_vexpand(false);
	update_statusbar();

	ScrolledWindow* scrolled_window = manage(new ScrolledWindow());
	scrolled_window->add(m_treeview);

	Box* box = manage(new Box(ORIENTATION_VERTICAL));
	box->pack_start(*(m_uimanager->get_widget("/MenuBar")), false, true);
	box->pack_start(*scrolled_window, true, true);
	box->pack_start(m_statusbar, false, true);
	
	add(*box);
	show_all();
}


MainWindow::~MainWindow()
{
	// save window geometry
	int w, h, x, y;
	get_size(w, h);
	get_position(x, y);
	Opt::set_whxy(w, h, x, y);
}


void MainWindow::update_statusbar()
{
	string msg = Porg::num2str(DB::pkg_cnt()) + " packages | " 
		+ Porg::fmt_size(DB::total_size());

	if (DB::total_size() < Porg::KILOBYTE)
		msg += " bytes";

	m_statusbar.push(msg);
}


void MainWindow::build_menu_bar()
{
	m_action_group->add(Action::create("MenuFile", "_File"));
	m_action_group->add(Action::create("Quit", Stock::QUIT), 
		mem_fun(this, &MainWindow::hide));

	m_action_group->add(Action::create("MenuEdit", "_Edit"));
	m_action_group->add(m_action_find, mem_fun(this, &MainWindow::on_find_file));
	m_action_group->add(Action::create("Preferences", Stock::PREFERENCES), 
		mem_fun(this, &MainWindow::on_preferences));

	m_action_group->add(Action::create("MenuPackage", "_Package"));
	m_action_group->add(m_action_properties, mem_fun(this, &MainWindow::on_properties));
	m_action_group->add(m_action_porgball, mem_fun(this, &MainWindow::on_porgball));
	m_action_group->add(m_action_remove, mem_fun(this, &MainWindow::on_remove));
	m_action_group->add(m_action_unlog, mem_fun(*this, &MainWindow::on_unlog));

	m_action_group->add(Action::create("MenuHelp", "_Help"));
	m_action_group->add(Action::create("About", Stock::ABOUT), 
		mem_fun(this, &MainWindow::on_about));

	m_uimanager->insert_action_group(m_action_group);
	add_accel_group(m_uimanager->get_accel_group());
	
	m_uimanager->add_ui_from_string(
		"<ui>"
		"	<menubar name='MenuBar'>"
		"		<menu action='MenuFile'>"
		"			<menuitem action='Quit'/>"
		"		</menu>"
		"		<menu action='MenuEdit'>"
		"			<menuitem action='FindFile'/>"
		"			<menuitem action='Preferences'/>"
		"		</menu>"
		"		<menu action='MenuPackage'>"
		"			<menuitem action='Properties'/>"
		"			<menuitem action='Porgball'/>"
		"			<separator/>"
		"			<menuitem action='RemovePkg'/>"
		"			<menuitem action='UnlogPkg'/>"
		"		</menu>"
		"		<menu action='MenuHelp'>"
		"			<menuitem action='About'/>"
		"		</menu>"
		"	</menubar>"
		"	<popup name='PopupMenu'>"
		"		<menuitem action='Properties'/>"
		"		<menuitem action='Porgball'/>"
		"		<separator/>"
		"		<menuitem action='RemovePkg'/>"
		"		<menuitem action='UnlogPkg'/>"
		"	</popup>"
		"</ui>");

	m_popup_menu = dynamic_cast<Menu*>(m_uimanager->get_widget("/PopupMenu"));
}


void MainWindow::on_popup_menu(GdkEventButton* event)
{
	if (m_selected_pkg) {
		g_return_if_fail(event != NULL && m_popup_menu != NULL);
		m_popup_menu->popup(event->button, event->time);
	}
}


void MainWindow::on_2button_press(GdkEventButton*)
{
	if (m_selected_pkg)
		on_properties();
}


void MainWindow::on_key_press(GdkEventKey* event)
{
	if (!m_selected_pkg)
		return;

	g_return_if_fail(event != NULL);

	switch (event->keyval) {
		
		case GDK_KEY_Delete: 
			on_unlog();
			break;
		
		case GDK_KEY_Return:
			on_properties();
			break;
		
		case GDK_KEY_Menu:   
			g_return_if_fail(m_popup_menu != NULL);
			m_popup_menu->popup(0, event->time);
	}
}


void MainWindow::on_pkg_selected(Pkg* pkg)
{
	m_selected_pkg = pkg;
	set_actions_sensitivity();
}


void MainWindow::set_actions_sensitivity()
{
	m_action_find		->set_sensitive(DB::pkg_cnt() > 0);
	m_action_properties	->set_sensitive(m_selected_pkg);
	m_action_porgball	->set_sensitive(m_selected_pkg);
	m_action_remove		->set_sensitive(m_selected_pkg && Opt::logdir_writable());
	m_action_unlog		->set_sensitive(m_selected_pkg && Opt::logdir_writable());
}


void MainWindow::on_about()
{
	AboutDialog dialog;

	dialog.set_transient_for(*this);
	dialog.set_name("grop");
	dialog.set_logo_default();
	dialog.set_version("Version " PACKAGE_VERSION " (" RELEASEDATE ")");
	dialog.set_comments("Graphic interface of porg,\na source code package organizer");
	dialog.set_authors(std::vector<Glib::ustring>(1, "David Ricart"));
	dialog.set_website(PACKAGE_BUGREPORT);
	dialog.set_copyright("Copyright (C) 2015 David Ricart");
	dialog.set_license("This program is free software; you can redistribute it and/or "
		"modify it under the terms of the GNU General Public License as published by the "
		"Free Software Foundation; either version 2 of the License, or (at your option) "
		"any later version.\n\nThis program is distributed in the hope that it will be "
		"useful, but WITHOUT ANY WARRANTY; without even the implied warranty of "
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public "
		"License for more details.\n\nYou should have received a copy of the GNU General "
		"Public License along with this program; if not, write to the Free Software "
		"Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA");
	dialog.set_wrap_license(true);

	dialog.run();
}


void MainWindow::on_preferences()
{
	if (Preferences::instance(*this) == RESPONSE_OK)
		m_treeview.reset_opts();
}


void MainWindow::on_find_file()
{
	Find::instance(*this);
}


void MainWindow::on_properties()
{
	if (m_selected_pkg)
		Properties::instance(*m_selected_pkg, *this);
}


void MainWindow::on_porgball()
{
	if (m_selected_pkg)
		Porgball::instance(*m_selected_pkg, *this);
}


void MainWindow::scroll_to_pkg(Pkg* pkg)
{
	g_return_if_fail(pkg != NULL);
	m_treeview.scroll_to_pkg(pkg);
}


void MainWindow::on_unlog()
{
	if (!(Opt::logdir_writable() && m_selected_pkg))
		return;

	if (run_question_dialog("Remove package '" + m_selected_pkg->name() 
		+ "' from database ?", this))
		unlog_pkg(m_selected_pkg);
}


void MainWindow::on_remove()
{
	g_return_if_fail(Opt::logdir_writable() && m_selected_pkg != NULL);

	if (!run_question_dialog("Remove package '" + m_selected_pkg->name() + "' ?", this))
		return;

	if (RemovePkg::instance(*m_selected_pkg, *this))
		unlog_pkg(m_selected_pkg);
}


void MainWindow::unlog_pkg(Pkg* pkg)
{
	g_return_if_fail(pkg != NULL);

	try
	{
		DB::remove_pkg(pkg);
		m_treeview.remove_pkg(pkg);
		update_statusbar();
	}
	catch (std::exception const& x)
	{
		run_error_dialog(x.what(), this);
	}
}

