//=======================================================================
// pkg.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "pkg.h"
#include "out.h"
#include "opt.h"
#include "db.h"
#include "main.h"			// g_exit_status
#include "porg/common.h"	// in_paths(), strip_trailing()
#include "porg/file.h"
#include <string>
#include <iomanip>

using std::string;
using std::cout;
using std::endl;
using std::set;
using std::setw;
using namespace Porg;

static void remove_parent_dir(string const& path);


Pkg::Pkg(string const& name_)
:
	BasePkg(name_)
{
	read_log();
}


void Pkg::print_info() const
{
	cout
		<< string(m_name.size() + 2, '-') << endl
		<< " " << m_name << " " << endl
		<< string(m_name.size() + 2, '-') << endl
		<< "Name:    " << m_base_name << endl
		<< "Version: " << m_version << endl
		<< "Summary: " << m_summary << endl
		<< "Author:  " << m_author << endl
		<< "License: " << m_license << endl
		<< "URL:     " << m_url << endl << endl
		<< description_str() << endl << endl;
}


void Pkg::append(set<string> const& files_)
{
	bool appended(false);

	for (set<string>::const_iterator f(files_.begin()); f != files_.end(); ++f) {
		if (!find_file(*f)) {
			log_file(*f);
			appended = true;
		}
	}

	if (appended)
		write_log();
}


void Pkg::unlog() const
{
	try 
	{
		BasePkg::unlog(); 
		Out::vrb("Package '" + m_name + "' removed from database");
	}
	catch (Error const& x) 
	{
		Out::vrb(x.what());
	}
}


void Pkg::list(int size_w, int nfiles_w) const
{
	if (Opt::print_sizes())
		cout << setw(size_w) << fmt_size(m_size) << "  ";

	if (Opt::print_nfiles())
		cout << setw(nfiles_w) << m_nfiles << "  ";

	if (Opt::print_date())
		cout << fmt_date(m_date, Opt::print_hour()) << "  ";

	if (!Opt::print_no_pkg_name())
		cout << m_name;
	
	cout << endl;
}


void Pkg::list_files(int size_w)
{
	assert(size_w > 0);

	sort_files(Opt::sort_type(), Opt::reverse_sort());

	if (!Opt::print_no_pkg_name())
		cout << m_name << ":" << endl;

	for (const_iter f(m_files.begin()); f != m_files.end(); ++f) {

		if (Opt::print_sizes())
			cout << setw(size_w) << fmt_size((*f)->size()) << "  ";

		cout << (*f)->name();

		if (Opt::print_symlinks() && (*f)->is_symlink())
			cout << " -> " << (*f)->ln_name();

		cout << endl;
	}
}


void Pkg::remove(DB const& db)
{
	for (iter f(m_files.begin()); f != m_files.end(); ++f) {

		// skip excluded
		if (in_paths((*f)->name(), Opt::remove_skip()))
			Out::vrb((*f)->name() + ": excluded");

		// skip shared files
		else if (is_shared(*f, db))
			Out::vrb((*f)->name() + ": shared");

		// remove file
		else if (!unlink((*f)->name().c_str())) {
			Out::vrb("Removed '" + (*f)->name());
			remove_parent_dir((*f)->name());
		}

		// an error occurred
		else if (errno != ENOENT) {
			Out::vrb("Failed to remove '" + (*f)->name() + "'", errno);
			g_exit_status = EXIT_FAILURE;
		}
	}

	if (g_exit_status == EXIT_SUCCESS)
		unlog();
}


static void remove_parent_dir(string const& path)
{
	string dir(strip_trailing(path, '/'));
	string::size_type i;

	if ((i = dir.rfind('/')) != string::npos) {
		dir.erase(i);
		if (rmdir(dir.c_str()) == 0) {
			Out::vrb("Removed directory '" + dir + "'");
			remove_parent_dir(dir);
		}
	}
}

