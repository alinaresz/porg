//=======================================================================
// db.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "porg/file.h"
#include "db.h"
#include "util.h"
#include "main.h"
#include "opt.h"
#include "out.h"
#include "pkg.h"
#include <algorithm>
#include <iomanip>

using std::cout;
using std::endl;
using std::vector;
using std::setw;
using std::string;
using std::max;
using namespace Porg;

static int get_digits(ulong);
static int get_width(ulong);
static bool match_pkg(string const&, string const&);


DB::DB()
:
	vector<Pkg*>(),
	m_total_size(0),
	m_total_files(0)
{ }


DB::~DB()
{
	for (iterator p(begin()); p != end(); delete *p++) ;
}


//
// get all packages logged in database
//
void DB::get_pkgs_all()
{
	Dir dir(Opt::logdir());

	for (string name; dir.read(name); add_pkg(name)) ;

	if (empty())
		Out::vrb("porg: No packages logged in '" + Opt::logdir() + "'");
}


//
// Search the database for packages matching any of the strings in args given
// by the command line
//
void DB::get_pkgs(vector<string> const& args)
{
	Dir dir(Opt::logdir());

	for (uint i = 0; i < args.size(); ++i, dir.rewind()) {
		
		bool found = false;

		for (string name; dir.read(name); )
			found |= (match_pkg(args[i], name) && add_pkg(name));

		if (!found) {
			Out::vrb("porg: " + args[i] + ": Package not logged");
			g_exit_status = EXIT_FAILURE;
		}
	}
}


bool DB::add_pkg(string const& name)
{
	try 
	{
		Pkg* pkg = new Pkg(name);
		push_back(pkg);
		m_total_size += pkg->size();
		m_total_files += pkg->nfiles();
		return true;
	}
	catch (...) 
	{ 
		return false; 
	}
}


//
// get widths for printing pkg sizes and number of files
//
void DB::get_pkg_list_widths(int& size_w, int& nfiles_w) const
{
	size_w = Opt::print_totals() ? get_width(m_total_size) : 0;
	
	ulong max_nfiles(Opt::print_totals() ? m_total_files : 0);
	
	for (const_iterator p(begin()); p != end(); ++p) {
		
		size_w = max(size_w, get_width((*p)->size()));
		
		if (!Opt::print_totals())
			max_nfiles = max(max_nfiles, (*p)->nfiles());
	}

	nfiles_w = get_digits(max_nfiles);
}


//
// get width for printing file sizes
//
int DB::get_file_size_width() const
{
	int size_w = Opt::print_totals() ? get_width(m_total_size) : 0;

	for (const_iterator p(begin()); p != end(); ++p) {
		for (Pkg::const_iter f((*p)->files().begin()); f != (*p)->files().end(); ++f)
			size_w = max(size_w, get_width((*f)->size()));
	}

	return size_w;
}


void DB::print_conf_opts() const
{
	for (const_iterator p(begin()); p != end(); ++p) {
		
		if (size() > 1)
			cout << (*p)->name() << ":" << endl;
		
		cout << (*p)->conf_opts() << endl;
		
		if (!(*p)->conf_opts().empty() && size() > 1 && p != end() - 1)
			cout << endl;
	}
}


void DB::query() const
{
	for (uint i(0); i < Opt::args().size(); ++i) {
		
		bool found = false;
		string path(clear_path(Opt::args()[i]));
		cout << path << ':';
		
		for (const_iterator p(begin()); p != end(); ++p) {
			
			if ((*p)->find_file(path)) {
				found = true;
				cout << "  " << (*p)->name();
			}
		}
		
		cout << endl;

		if (!found)
			g_exit_status = EXIT_FAILURE;
	}
}


void DB::print_info() const
{
	for (const_iterator p(begin()); p != end(); (*p++)->print_info()) ;
}


void DB::sort_pkgs(	sort_t type,	// = SORT_BY_NAME
					bool reverse)	// = false
{
	std::sort(begin(), end(), Sorter(type));

	if (reverse)
		std::reverse(begin(), end());
}


void DB::remove() const
{
	// ask the user, if needed
	if (!Opt::remove_batch()) {
		
		cout << "The following packages will be "
			 << (Opt::remove_unlog() ? "unlogged:" : "removed:") << endl;
		
		for (const_iterator p(begin()); p != end(); ++p)
			cout << "    " << (*p)->name() << endl;
		
		cout << "Do you want to proceed (y/N) ? ";
		
		string buf;
		if (getline(std::cin, buf) && buf != "y")
			return;
	}

	if (Opt::remove_unlog()) {
		for (const_iterator p(begin()); p != end(); (*p++)->unlog()) ;
		return;
	}
	
	// auxiliary DB to check for shared files
	DB aux;
	aux.get_pkgs_all();

	for (const_iterator p(begin()); p != end(); ++p) {
		(*p)->remove(aux);
		aux.del_pkg((*p)->name());
	}
}


void DB::del_pkg(string const& name)
{
	for (iterator p(begin()); p != end(); ++p) {
		if ((*p)->name() == name) {
			erase(p);
			break;
		}
	}
}


void DB::list_pkgs() const
{
	int size_w = 0, nfiles_w = 0;

	// get widths for printing pkg sizes and number of files

	if (Opt::print_sizes() || Opt::print_nfiles())
		get_pkg_list_widths(size_w, nfiles_w);

	// list packages
	
	for (const_iterator p(begin()); p != end(); ++p)
		(*p)->list(size_w, nfiles_w);

	// print totals, if needed
	
	if (Opt::print_totals()) {
		
		if (Opt::print_sizes())
			cout << setw(size_w) << fmt_size(m_total_size) << "  ";
		
		if (Opt::print_nfiles())
			cout << setw(nfiles_w) << m_total_files << "  ";

		if (Opt::print_date())
			cout << fmt_date(0, Opt::print_hour()) << "  ";
		
		cout << "TOTAL" << endl;
	}
}


void DB::list_files() const
{
	int size_w(get_file_size_width());

	for (const_iterator p(begin()); p != end(); ++p) {
		(*p)->list_files(size_w);
		if (!Opt::print_no_pkg_name() && size() > 1)
			cout << endl;
	}

	if (Opt::print_totals())
		cout << setw(size_w) << fmt_size(m_total_size) << "  TOTAL" << endl;
}


//------------//
// DB::Sorter //
//------------//


DB::Sorter::Sorter(sort_t const& t /* = SORT_BY_NAME */)
:
	m_sort_func()
{
	switch (t) {
		case SORT_BY_SIZE: 		m_sort_func = &Sorter::sort_by_size; break;
		case SORT_BY_NFILES:	m_sort_func = &Sorter::sort_by_nfiles; break;
		case SORT_BY_DATE: 		m_sort_func = &Sorter::sort_by_date; break;
		default: 				m_sort_func = &Sorter::sort_by_name;
	}
}


bool DB::Sorter::operator()(Pkg* left, Pkg* right) const
{
	return (this->*m_sort_func)(right, left);
}

bool DB::Sorter::sort_by_name(Pkg* left, Pkg* right) const
{
	return left->name() > right->name();
}

bool DB::Sorter::sort_by_size(Pkg* left, Pkg* right) const
{
	return left->size() < right->size();
}

bool DB::Sorter::sort_by_nfiles(Pkg* left, Pkg* right) const
{
	return left->nfiles() < right->nfiles();
}

bool DB::Sorter::sort_by_date(Pkg* left, Pkg* right) const
{
	return left->date() > right->date();
}


//-------------------//
// static free funcs //
//-------------------//


//
// Return the number of digits of a number
//
static int get_digits(ulong n)
{
	int ret;
	for (ret = 0; n; n /= 10, ret++) ;
	return ret;
}


inline static int get_width(ulong size)
{
	return fmt_size(size).size();
}


static bool match_pkg(string const& str, string const& pkg)
{
	if (Opt::exact_version())
		return str == pkg;

	string str_base = Pkg::get_base(str);
	string pkg_base = Pkg::get_base(pkg);
	string str_version = Pkg::get_version(str);
	string pkg_version = Pkg::get_version(pkg);
	
	if (pkg_base != str_base)
		return false;

	else if (str_version.empty() || str_version == pkg_version)
		return true;

	else if (str_version.compare(0, str_version.size(), 
		pkg_version.c_str(), str_version.size()))
		return false;

	return ispunct(pkg_version[str_version.size()]);
}

