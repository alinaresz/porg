//=======================================================================
// newpkg.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "porg/file.h"
#include "porg/rexp.h"
#include "newpkg.h"
#include "out.h"
#include <string>
#include <fstream>
#include <glob.h>

using std::string;
using std::set;
using namespace Porg;

static void get_var(string const&, string const&, string&);
static void get_define(string const&, string const&, string&);
static string search_file(string const&);


NewPkg::NewPkg(string const& name_, set<string> const& files_)
:
	BasePkg(name_)
{
	for (set<string>::const_iterator f(files_.begin()); f != files_.end(); ++f)
		log_file(*f);
	
	if (m_files.empty())
		throw Error(m_name + ": No files to log");;

	Out::dbg_title("package information");

	read_config();
	read_pc();
	read_desktop();
	read_spec();

	get_configure_options();
	get_icon_path();

	Out::dbg_title();
	Out::dbg("Name:       "	+ m_base_name);
	Out::dbg("Version:    "	+ m_version);
	Out::dbg("Summary:    "	+ m_summary);
	Out::dbg("Author:     "	+ m_author);
	Out::dbg("URL:        "	+ m_url);
	Out::dbg("License:    "	+ m_license);
	Out::dbg("Conf. opt.: "	+ m_conf_opts);
	Out::dbg("Icon:       "	+ m_icon_path);
	Out::dbg(description_str(true));

	write_log();
}


void NewPkg::read_spec()
{
	string spec(search_file(m_base_name + ".spec"));
	if (spec.empty())
		return;

	Out::dbg("Reading " + spec);

	get_var(spec, "Icon", m_icon_path);
	get_var(spec, "Summary", m_summary);
	get_var(spec, "URL", m_url);
	get_var(spec, "Packager", m_author);
	get_var(spec, "Vendor", m_author);
	get_var(spec, "Copyright", m_license);
	get_var(spec, "License", m_license);

	// get description

	std::ifstream f(spec.c_str());
	string buf;

	while (getline(f, buf) && buf.find("%description") != 0) ;
	
	while (getline(f, buf) && buf.find_first_of("%#") != 0)
		m_description += buf + '\n';
}


void NewPkg::read_pc()
{
	string pc(search_file(m_base_name + ".pc"));
	if (pc.empty())
		return;

	Out::dbg("Reading " + pc);

	get_var(pc, "Description", m_summary);
	get_var(pc, "URL", m_url);
}


void NewPkg::read_desktop()
{
	string desktop(search_file(m_base_name + ".desktop"));
	if (desktop.empty())
		return;

	Out::dbg("Reading " + desktop);

	get_var(desktop, "Icon", m_icon_path);
	get_var(desktop, "GenericName", m_summary);
	get_var(desktop, "Comment", m_summary);
}


void NewPkg::read_config()
{
	string config("config.log");

	if (access(config.c_str(), R_OK) < 0) {
		config = "config.h";
		if (access(config.c_str(), R_OK) < 0)
			return;
	}

	Out::dbg("Reading " + config);

	get_define(config, "PACKAGE_URL", m_url);
	get_define(config, "PACKAGE_BUGREPORT", m_author);
	get_define(config, "PACKAGE_NAME", m_summary);
	get_define(config, "PACKAGE_STRING", m_summary);
}


void NewPkg::get_configure_options()
{
	Rexp re("^ *\\$ .*/configure[[:space:]]+(.*)$");
	string config("config.log");
	std::ifstream f(config.c_str());

	if (!f) {
		f.open("configure.log");
		if (!f)
			return;
		re.compile("./configure[[:space:]]+(.*)$");
	}

	Out::dbg("Retrieving configure options from " + config);
	
	for (string buf; getline(f, buf); ) {
		if (re.exec(buf)) {
			m_conf_opts = re.match(1);
			break;
		}
	}
}


void NewPkg::get_icon_path()
{
	string& path(m_icon_path);

	if (path.empty())
		path = m_base_name;
	
	// If it's an absolute path, we're done	
	else if (path[0] == '/')
		return;

	// otherwise search for the icon file in the list of files installed by
	// the package
	
	// if path does not have any image format suffix, add 'suf' to the expression
	
	string exp("/" + path);
	string suf("\\.(png|xpm|jpg|ico|gif|svg)$");
	Rexp re(suf, REG_ICASE);
	
	if (!re.exec(path))
		exp += suf;

	// Search the logged files for the path of the icon
	
	re.compile(exp, REG_ICASE);
	bool found = false;

	for (uint i(0); !found && i < m_files.size(); ++i) {
		if (re.exec(m_files[i]->name())) {
			path = m_files[i]->name();
			found = !access(path.c_str(), F_OK);
		}
	}

	if (!found)
		path.clear();
}


void get_var(string const& file, string const& tag, string& val)
{
	std::ifstream f(file.c_str());
	Porg::Rexp re("^" + tag + "[[:space:]:=]+(.*)$", REG_ICASE);

	for (string buf; getline(f, buf); ) {
		if (re.exec(buf)) {
			val = re.match(1);
			break;
		}
	}
}


//
// Get a define from a C header file, unquoting the result.
//
void get_define(string const& file, string const& tag, string& val)
{
	std::ifstream f(file.c_str());
	Porg::Rexp re("^[[:space:]]*#define[[:space:]]+" + tag 
		+ "[[:space:]\"]+(.*[^\"])");

	for (string buf; getline(f, buf); ) {
		if (re.exec(buf)) {
			val = re.match(1);
			break;
		}
	}
}


string search_file(string const& name)
{
	glob_t g;
	memset(&g, 0, sizeof(g));
	
	string file, patt[3] = { name, "*/" + name, "*/*/" + name };

	for (int i = 0; i < 3 && file.empty(); ++i) {
		if (!glob(patt[i].c_str(), 0, 0, &g) && g.gl_pathc)
			file = g.gl_pathv[0];
	}

	globfree(&g);
	return file;
}

