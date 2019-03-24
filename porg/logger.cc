//=======================================================================
// logger.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "out.h"
#include "opt.h"
#include "porg/common.h"	// in_paths()
#include "util.h"
#include "pkg.h"
#include "newpkg.h"
#include "logger.h"
#include <fstream>
#include <iterator>
#include <glob.h>
#include <sys/wait.h>

using namespace Porg;
using namespace std;

static string search_libporg();
static void set_env(char const* var, string const& val);


Logger::Logger()
:
	m_pkgname(Opt::log_pkg_name()),
	m_files()
{
	if (Opt::args().empty())
		read_files_from_stream(cin);
	else
		read_files_from_command();

	filter_files();

	if (m_pkgname.empty())
		write_files_to_stream(cout);
	else
		write_files_to_pkg();
}


void Logger::run()
{
	static Logger log;
}


void Logger::write_files_to_pkg() const
{
	bool done = false;

	if (Opt::log_append()) {
		try 
		{
			Pkg already_logged_pkg(m_pkgname);
			already_logged_pkg.append(m_files);
			done = true;
		}
		catch (...) { }
	}

	if (!done)
		NewPkg newpkg(m_pkgname, m_files);

	if (Out::debug()) {
		Out::dbg_title("logged files");
		write_files_to_stream(cerr);
		Out::dbg_title();
	}
}


void Logger::write_files_to_stream(ostream& s) const
{
	copy(m_files.begin(), m_files.end(), ostream_iterator<string>(s, "\n"));
}


void Logger::read_files_from_stream(istream& f)
{
	for (string buf; getline(f, buf); m_files.insert(buf)) ;
}


void Logger::read_files_from_command()
{
	// get name for tmp file

	char* tmpdir = getenv("TMPDIR");
	char tmpfile[4096];

	snprintf(tmpfile, sizeof(tmpfile), "%s/porgXXXXXX", tmpdir ? tmpdir : "/tmp");
	
	if (close(mkstemp(tmpfile)) < 0)
		snprintf(tmpfile, sizeof(tmpfile), "/tmp/porg%d", getpid());

	// exec command

	try
	{
		exec_command(tmpfile);
		FileStream<ifstream> f(tmpfile);
		read_files_from_stream(f);
		unlink(tmpfile);
	}
	catch (...)
	{
		unlink(tmpfile);
		throw;
	}
}


void Logger::exec_command(string const& tmpfile) const
{
	pid_t pid = fork();

	if (pid == 0) { // child

		string command, libporg = search_libporg();
		
		for (uint i(0); i < Opt::args().size(); ++i)
			command += Opt::args()[i] + " ";

#ifdef __APPLE__
		set_env("DYLD_INSERT_LIBRARIES", libporg);
		set_env("DYLD_FORCE_FLAT_NAMESPACE", "1");
#else
		set_env("LD_PRELOAD", libporg);
#endif
		set_env("PORG_TMPFILE", tmpfile);
		if (Out::debug())
			set_env("PORG_DEBUG", "yes");

		Out::dbg_title("settings");
#ifdef __APPLE__
		Out::dbg("DYLD_INSERT_LIBRARIES = " + libporg);
		Out::dbg("DYLD_FORCE_FLAT_NAMESPACE = 1");
#else
		Out::dbg("LD_PRELOAD = " + libporg); 
#endif
		Out::dbg("INCLUDE = " + Opt::include()); 
		Out::dbg("EXCLUDE = " + Opt::exclude()); 
		Out::dbg("command = " + command);
		Out::dbg_title("libporg-log");

		char* cmd[] = { (char*)"sh", (char*)"-c", (char*)(command.c_str()), 0 };
		execv("/bin/sh", cmd);

		throw Error("execv()", errno);
	}

	else if (pid == -1)
		throw Error("fork()", errno);

	wait(0);
}


//
// Convert input files to absolute paths, skip excluded or not included
// files, and skip non-regular or missing files.
//
void Logger::filter_files()
{
	vector<string> filtered;
	struct stat s;
	
	for (set<string>::iterator p = m_files.begin(); p != m_files.end(); ++p) {

		if ((*p).empty())
			continue;

		string path(clear_path((*p)));

		// skip excluded or not included files
		if (in_paths(path, Opt::exclude()) || !in_paths(path, Opt::include()))
			continue;
	
		// skip missing files, if needed
		else if (lstat(path.c_str(), &s) < 0 && !Opt::log_missing())
			continue;

		// log only regular files or symlinks
		else if (s.st_mode & (S_IFREG | S_IFLNK))
			filtered.push_back(path);
	}

	m_files.clear();
	copy(filtered.begin(), filtered.end(), inserter(m_files, m_files.begin()));
}


//
// Search for libporg-log.so in the filesystem.
//
static string search_libporg()
{
#ifdef __APPLE__
	string libpath(LIBDIR "/libporg-log.dylib");
#else
	string libpath(LIBDIR "/libporg-log.so");
#endif
	struct stat s;
	
	if (!stat(libpath.c_str(), &s))
		return libpath;
	
	glob_t g;
	memset(&g, 0, sizeof(g));
	
#ifdef __APPLE__
	if (!glob(LIBDIR "/libporg-log.[0-9]*.dylib", 0, 0, &g) && g.gl_pathc)
#else
	if (!glob(LIBDIR "/libporg-log.so.[0-9]*", 0, 0, &g) && g.gl_pathc)
#endif
		libpath = g.gl_pathv[0];
	
	globfree(&g);
	
	return libpath;
}


static void set_env(char const* var, string const& val)
{
	if (setenv(var, val.c_str(), 1) < 0)
		throw Error(string("setenv('") + var + "', '" + val + "', 1)", errno);
}

