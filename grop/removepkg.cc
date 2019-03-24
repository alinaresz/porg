//=======================================================================
// removepkg.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "opt.h"
#include "db.h"
#include "util.h"
#include "porg/file.h"
#include "porg/common.h"
#include "removepkg.h"
#include "porg/common.h"
#include <glibmm/miscutils.h>	// path_get_dirname()
#include <glibmm/main.h>	// signal_timeout()
#include <gtkmm/stock.h>
#include <gtkmm/scrolledwindow.h>
#include <glibmm/stringutils.h>	// strerror()

using std::string;
using sigc::mem_fun;
using namespace Grop;
using namespace Gtk;


RemovePkg::RemovePkg(Pkg& pkg, Window& parent)
:
	Dialog("grop :: remove package", parent, true),
	m_error(false),
	m_label(),
	m_progressbar(),
	m_expander("Details"),
	m_button_close(Stock::CLOSE),
	m_text_buffer(TextBuffer::create()),
	m_text_view(m_text_buffer),
	m_tag_ok(m_text_buffer->create_tag()),
	m_tag_skipped(m_text_buffer->create_tag()),
	m_tag_error(m_text_buffer->create_tag()),
	m_pkg(pkg)
{
	set_border_width(4);
	set_default_size(450, 0);

	Glib::signal_timeout().connect_once(mem_fun(this, &RemovePkg::remove), 100);

	m_expander.property_expanded().signal_changed().connect(
		mem_fun(this, &RemovePkg::on_expander_changed));

	m_label.set_markup("<i>Removing package '" + pkg.name() + "'...</i>"),
	m_label.set_ellipsize(Pango::ELLIPSIZE_MIDDLE);

	m_text_view.set_editable(false);
	m_text_view.set_cursor_visible(false);
	m_text_view.set_right_margin(get_border_width());
	m_text_view.set_left_margin(get_border_width());
	m_text_view.override_background_color(Gdk::RGBA("black"), STATE_FLAG_NORMAL);

	m_tag_ok->property_foreground() = "white";
	m_tag_skipped->property_foreground() = "#ffff44";	// light yellow
	m_tag_error->property_foreground() = "#ff4444";		// light red

	ScrolledWindow* scrolled_window = manage(new ScrolledWindow());
	scrolled_window->add(m_text_view);

	m_expander.add(*scrolled_window);

	Box* label_box = manage(new Box());
	label_box->pack_start(m_label, PACK_SHRINK);

	Box* box = get_content_area();
	box->set_spacing(get_border_width());
	box->pack_start(*label_box, PACK_SHRINK);
	box->pack_start(m_progressbar, PACK_SHRINK);
	box->pack_start(m_expander, PACK_EXPAND_WIDGET);

	m_button_close.set_sensitive(false);
	add_action_widget(m_button_close, RESPONSE_CLOSE);

	show_all();
	run();
}


void RemovePkg::on_expander_changed()
{
	resize(get_width(), m_expander.get_expanded() ? 400 : 1);
}


bool RemovePkg::instance(Pkg& pkg, Window& parent)
{
	RemovePkg remove_pkg(pkg, parent);
	
	remove_pkg.hide();
	
	return !remove_pkg.m_error;
}


void RemovePkg::report(string const& msg, Glib::RefPtr<TextTag> const& tag)
{
	m_text_buffer->insert_with_tag(m_text_buffer->end(), msg + "\n", tag);
	TextIter end = m_text_buffer->end();
	m_text_view.scroll_to(end);
}


void RemovePkg::remove()
{
	float cnt = 1;
	int cnt_shared = 0, cnt_excluded = 0, cnt_removed = 0, cnt_error = 0;

	for (Pkg::const_iter f(m_pkg.files().begin()); f != m_pkg.files().end(); ++f) {
		
		string const& file = (*f)->name();

		m_progressbar.set_fraction(cnt++ / m_pkg.nfiles());
		main_iter();

		// skip excluded
		if (Porg::in_paths(file, Opt::remove_skip())) {
			report("'" + file + "': excluded", m_tag_skipped);
			cnt_excluded++;
		}

		// skip shared files
		else if (m_pkg.is_shared(*f, DB::pkgs())) {
			report("'" + file + "': shared", m_tag_skipped);
			cnt_shared++;
		}

		// remove file
		else if (!unlink(file.c_str())) {
			report("Removed '" + file + "'", m_tag_ok);
			remove_parent_dir(file);
			cnt_removed++;
		}

		// an error occurred
		else if (errno != ENOENT) {
			report("Failed to remove '" + file + "': " + 
				Glib::strerror(errno), m_tag_error);
			cnt_error++;
			m_error = true;
		}
	}

	std::ostringstream summary;
	summary << "\nSummary:\n"
		<< cnt_removed << " files removed\n"
		<< cnt_excluded << " files excluded\n"
		<< cnt_shared << " files shared\n"
		<< cnt_error << " errors";
	report(summary.str(), m_tag_ok);

	if (m_error) {
		m_label.set_markup("<span fgcolor=\"darkred\"><b>Completed with "
			"errors (see Details)</b></span>");
	}
	else {
		report("\nPackage '" + m_pkg.name() + "' removed from database", m_tag_ok);
		m_label.set_markup("<span fgcolor=\"darkgreen\"><b>Done</b></span>");
	}

	m_button_close.set_sensitive();
}


void RemovePkg::remove_parent_dir(string const& path)
{
	string parent = Glib::path_get_dirname(path);
	
	if (!rmdir(parent.c_str())) {
		report("Removed directory '" + parent + "'", m_tag_ok);
		remove_parent_dir(parent);
	}
}

