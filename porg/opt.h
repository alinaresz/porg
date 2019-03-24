//=======================================================================
// opt.h
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#ifndef PORG_OPT_H
#define PORG_OPT_H

#include "config.h"
#include "porg/common.h"
#include "porg/baseopt.h"
#include <vector>


namespace Porg {


enum {
	MODE_DEFAULT	= 1 << 0,	// alias for MODE_LIST_PKGS
   	MODE_LIST_PKGS	= 1 << 0,
   	MODE_LIST_FILES = 1 << 1,
   	MODE_QUERY 		= 1 << 2,
   	MODE_INFO 		= 1 << 3,
   	MODE_CONF_OPTS 	= 1 << 4,
   	MODE_LOG 		= 1 << 5,
   	MODE_REMOVE 	= 1 << 6
};


class Opt : public BaseOpt
{
	public:

	~Opt();

	static void init(int argc, char* argv[]);

	static bool all_pkgs()			{ return s_all_pkgs; }
	static bool exact_version()		{ return s_exact_version; }
	static bool print_sizes()		{ return s_print_sizes; }
	static bool print_nfiles()		{ return s_print_nfiles; }
	static bool print_totals()		{ return s_print_totals; }
	static bool print_symlinks()	{ return s_print_symlinks; }
	static bool print_no_pkg_name()	{ return s_print_no_pkg_name; }
	static bool remove_batch()		{ return s_remove_batch; }
	static bool remove_unlog()		{ return s_remove_unlog; }
	static bool log_append()		{ return s_log_append; }
	static bool log_missing()		{ return s_log_missing; }
	static bool reverse_sort() 		{ return s_reverse_sort; }
	static bool print_date() 		{ return s_print_date; }
	static bool print_hour() 		{ return s_print_hour; }
	static sort_t sort_type()		{ return s_sort_type; }
	static int mode()				{ return s_mode; };
	static std::string const& log_pkg_name()		{ return s_log_pkg_name; }
	static std::vector<std::string> const& args()	{ return s_args; }
	
	protected:

	Opt(int argc, char* argv[]);

	static void check_mode(int, char);
	static void check_required(char, std::string const&);
	static void set_mode(int m, char optchar);
	static void set_sort_type(std::string const&);

	static bool s_all_pkgs;
	static bool s_exact_version;
	static bool s_print_sizes;
	static bool s_print_nfiles;
	static bool s_print_totals;
	static bool s_print_symlinks;
	static bool s_print_no_pkg_name;
	static bool s_remove_batch;
	static bool s_remove_unlog;
	static bool s_log_append;
	static bool s_log_missing;
	static bool s_reverse_sort;
	static bool s_print_date;
	static bool s_print_hour;
	static bool s_logdir_created;
	static sort_t	s_sort_type;
	static std::string s_log_pkg_name;
	static int s_mode;
	static std::vector<std::string> s_args;
	static char s_mode_char;
	static std::string s_optchars;

};	// class Opt

}	// namespace Porg


#endif  // PORG_OPT_H
