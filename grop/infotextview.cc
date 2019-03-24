//=======================================================================
// infotextview.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "infotextview.h"

using Glib::ustring;
using namespace Grop;
using namespace Gtk;


InfoTextView::InfoTextView(Pkg const& pkg)
:
	TextView(),
	m_text_buffer(get_buffer()),
	m_tag_wrap(m_text_buffer->create_tag()),
	m_tag_title(m_text_buffer->create_tag())
{
	set_editable(false);
	set_cursor_visible(false);
	set_left_margin(10);
	set_right_margin(10);

	m_tag_wrap->property_wrap_mode() = WRAP_WORD;
	m_tag_title->property_weight() = Pango::WEIGHT_BOLD;
	m_tag_title->property_family() = "monospace";

	try 
	{ 
		Glib::RefPtr<Gdk::Pixbuf> icon
			= Gdk::Pixbuf::create_from_file(pkg.icon_path(), 72, 72);
		if (icon) {
			m_text_buffer->insert(m_text_buffer->end(), "\n");
			m_text_buffer->insert_pixbuf(m_text_buffer->end(), icon);
			m_text_buffer->insert(m_text_buffer->end(), "\n");
		}
	}
	catch (...) { }

	insert("\nName:    ", pkg.base_name());
	insert("\nVersion: ", pkg.version());
	insert("\nSummary: ", pkg.summary());
	insert("\nAuthor:  ", pkg.author());
	insert("\nLicense: ", pkg.license());
	insert("\nURL:     ", pkg.url());
	insert("\n\nDescription:\n", pkg.description());
	insert("\n\nConfigure options:\n", pkg.conf_opts(), true);
}


void InfoTextView::insert(ustring const& title, ustring const& text, 
                          bool allow_empty /* = false */)
{
	if (!text.empty() || allow_empty) {
		m_text_buffer->insert_with_tag(m_text_buffer->end(), title, m_tag_title);
		m_text_buffer->insert_with_tag(m_text_buffer->end(), text, m_tag_wrap);
	}
}


