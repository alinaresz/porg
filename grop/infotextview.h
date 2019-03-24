//=======================================================================
// infotextview.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_INFOTEXTVIEW_H
#define GROP_INFOTEXTVIEW_H

#include "config.h"
#include "pkg.h"
#include <gtkmm/textview.h>


namespace Grop
{

class InfoTextView : public Gtk::TextView
{
	public:

	InfoTextView(Pkg const&);

	private:

	void insert(Glib::ustring const& title, Glib::ustring const& text, 
                bool allow_empty = false);

	Glib::RefPtr<Gtk::TextBuffer>	m_text_buffer;
	Glib::RefPtr<Gtk::TextTag>		m_tag_wrap;
	Glib::RefPtr<Gtk::TextTag>		m_tag_title;
};

} // namespace Grop


#endif  // GROP_INFOTEXTVIEW_H
