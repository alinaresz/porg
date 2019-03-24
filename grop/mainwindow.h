//=======================================================================
// mainwindow.h
//-----------------------------------------------------------------------
// This file is part of the package grop
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_MAIN_WINDOW_H
#define GROP_MAIN_WINDOW_H

#include "config.h"
#include "maintreeview.h"
#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/uimanager.h>


namespace Grop {


class MainWindow : public Gtk::Window 
{
	public:

	MainWindow();
	~MainWindow();

	void scroll_to_pkg(Pkg*);

	private:

	void update_statusbar();
	void build_menu_bar();
	void on_about();
	void on_preferences();
	void on_find_file();
	void on_pkg_menu();
	void on_properties();
	void on_unlog();
	void on_remove();
	void on_porgball();
	void on_popup_menu(GdkEventButton*);
	void on_2button_press(GdkEventButton*);
	void on_key_press(GdkEventKey*);
	void on_pkg_selected(Pkg*);
	void unlog_pkg(Pkg*);
	void set_actions_sensitivity();

	MainTreeView					m_treeview;
	Gtk::Statusbar					m_statusbar;
	Glib::RefPtr<Gtk::UIManager>	m_uimanager;
	Glib::RefPtr<Gtk::ActionGroup>	m_action_group;
	Glib::RefPtr<Gtk::Action>		m_action_find;
	Glib::RefPtr<Gtk::Action>		m_action_properties;
	Glib::RefPtr<Gtk::Action>		m_action_porgball;
	Glib::RefPtr<Gtk::Action>		m_action_remove;
	Glib::RefPtr<Gtk::Action>		m_action_unlog;
	Gtk::Menu*						m_popup_menu;
	Pkg*							m_selected_pkg;

};	// class MainWindow

} 	// namespace Grop


#endif  // GROP_MAIN_WINDOW_H
