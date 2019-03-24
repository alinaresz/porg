//=======================================================================
// properties.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef GROP_PROPERTIES_H
#define GROP_PROPERTIES_H

#include "config.h"
#include "pkg.h"
#include <gtkmm/dialog.h>
#include <gtkmm/notebook.h>


namespace Grop
{

class Properties : public Gtk::Dialog 
{
	public:

	static void instance(Pkg const&, Gtk::Window&);

	protected:

	Properties(Pkg const&, Gtk::Window&);

	Gtk::Notebook	m_notebook;
};

} // namespace Grop


#endif  // GROP_PROPERTIES_H
