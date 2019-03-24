//=======================================================================
// basepkg.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "basepkg.h"
#include "baseopt.h"
#include "file.h"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iomanip>

using std::string;
using namespace Porg;


BasePkg::BasePkg(string const& name_)
:
	m_files(),
	m_inodes(),
	m_name(name_),
	m_log(BaseOpt::logdir() + "/" + name_),
	m_base_name(get_base(name_)),
	m_version(get_version(name_)),
	m_date(time(0)),
	m_size(0),
	m_nfiles(0),
	m_icon_path(),
	m_url(),
	m_license(),
	m_summary(),
	m_description(),
	m_conf_opts(),
	m_author(),
	m_sorted_by_name(false)
{ }


//
// Read info line from log
// Each info line has the form '#<char>:<value>', where 
// 		<char> is a single character defining the info field, and 
// 		<value> is its value.
//
void BasePkg::read_info_line(string const& buf)
{
	assert(buf.size() > 2 && buf[0] == '#' && buf[2] == ':');

	string val(buf.substr(3));

	switch (buf[1]) {

		case CODE_DATE: 		m_date = str2num<int>(val);		break;
		case CODE_SIZE: 		m_size = str2num<float>(val);	break;
		case CODE_NFILES: 		m_nfiles = str2num<ulong>(val);	break;
		case CODE_CONF_OPTS:	m_conf_opts = val; 				break;
		case CODE_ICON_PATH:	m_icon_path = val;				break;
		case CODE_SUMMARY: 		m_summary = val; 				break;
		case CODE_URL: 			m_url = val; 					break;
		case CODE_LICENSE: 		m_license = val; 				break;
		case CODE_AUTHOR: 		m_author = val;					break;
		case CODE_DESCRIPTION:
			if (!m_description.empty())
				m_description += "\n";
			m_description += val;
			break;
		
		default: assert(false); break;
	}
}
	

void BasePkg::read_log()
{
	FileStream<std::ifstream> f(m_log);
	string buf;
	
	if (!(getline(f, buf) && buf.find("#!porg") == 0))
		throw Error(m_log + ": '#!porg' header missing");

	char path[4096], link_path[4096];
	ulong size;

	while (getline(f, buf)) {

		switch (sscanf(buf.c_str(), "%[^|]|%lu|%s", path, &size, link_path)) {

			case 1:	// info header
				read_info_line(buf);
				break;
			
			case 2: // regular file
				m_files.push_back(new File(path, size)); 
				break;
			
			case 3: // symlink
				m_files.push_back(new File(path, size, link_path)); 
				break;
			
			default: // parse error
				assert(false);
				break;
		}
	}

	sort_files();
}


BasePkg::~BasePkg()
{
	for (iter f(m_files.begin()); f != m_files.end(); delete *f++) ;
}


void BasePkg::unlog() const
{
	if (unlink(m_log.c_str()) != 0 && errno != ENOENT)
		throw Error("unlink(" + m_log + ")", errno);
}


string BasePkg::description_str(bool debug /* = false */) const
{
	string const head(debug ? "porg :: " : "");
	string desc("Description: ");
	
	if (m_description.find('\n') == string::npos)
		desc += m_description;
	else {
		std::istringstream is(m_description);
		for (string buf; getline(is, buf); )
			desc += "\n" + head + "   " + buf;
	}

	return desc;
}


string BasePkg::format_description() const
{
	string code(string("#") + CODE_DESCRIPTION + ":");

	if (m_description.empty())
		return code + "\n";

	string ret;
	std::istringstream is(m_description);

	for (string buf; getline(is, buf); )
		ret += code + buf + "\n";

	return ret;
}


void BasePkg::write_log() const
{
	// Create log file

	FileStream<std::ofstream> of(m_log);

	// write info header

	of	<< "#!porg-" PACKAGE_VERSION "\n"
		<< '#' << CODE_DATE 		<< ':' << m_date << '\n'
		<< '#' << CODE_SIZE 		<< ':' << std::setprecision(0) << std::fixed << m_size << '\n'
		<< '#' << CODE_NFILES       << ':' << m_nfiles << '\n'
		<< '#' << CODE_AUTHOR		<< ':' << m_author << '\n'
		<< '#' << CODE_SUMMARY		<< ':' << Porg::strip_trailing(m_summary, '.') << '\n'
		<< '#' << CODE_URL			<< ':' << m_url << '\n'
		<< '#' << CODE_LICENSE		<< ':' << m_license << '\n'
		<< '#' << CODE_CONF_OPTS	<< ':' << m_conf_opts << '\n'
		<< '#' << CODE_ICON_PATH	<< ':' << m_icon_path << '\n'
		<< format_description();

	// write installed files
	
	for (const_iter f(m_files.begin()); f != m_files.end(); ++f)
		of << (*f)->name() << '|' << (*f)->size() << '|' << (*f)->ln_name() << '\n';
}


void BasePkg::log_file(string const& path)
{
	File* file = new File(path);
	m_files.push_back(file);

	m_nfiles++;

	// detect hardlinks to installed files, to count their size only once
	
	assert(file->inode() > 0);
	
	if (m_inodes.find(file->inode()) == m_inodes.end()) {
		m_inodes.insert(file->inode());
		m_size += file->size();
	}
}


bool BasePkg::find_file(File* file)
{
	assert(file != NULL);

	if (!m_sorted_by_name)
		sort_files();
	
	return std::binary_search(m_files.begin(), m_files.end(), file, Sorter());
}


bool BasePkg::find_file(string const& path)
{
	File file(path);
	return find_file(&file);
}


void BasePkg::sort_files(	sort_t type,	// = SORT_BY_NAME
							bool reverse)	// = false
{
	std::sort(m_files.begin(), m_files.end(), Sorter(type));
	
	if (reverse)
		std::reverse(m_files.begin(), m_files.end());
	
	m_sorted_by_name = (type == SORT_BY_NAME && !reverse);
}


string BasePkg::get_base(string const& name)
{
	for (uint i = 1; i < name.size(); ++i) {
		if (isdigit(name.at(i)) && name.at(i - 1) == '-')
			return name.substr(0, i - 1);
	}
	return name;
}


string BasePkg::get_version(string const& name)
{
	for (uint i = 1; i < name.size(); ++i) {
		if (isdigit(name.at(i)) && name.at(i - 1) == '-')
			return name.substr(i);
	}
	return "";
}


//-----------------//
// BasePkg::Sorter //
//-----------------//


BasePkg::Sorter::Sorter(sort_t type /* = SORT_BY_NAME */)
:
	m_sort_func(type == SORT_BY_NAME ? &Sorter::sort_by_name : &Sorter::sort_by_size)
{ }


inline bool BasePkg::Sorter::operator()(File* left, File* right) const
{
	return (this->*m_sort_func)(left, right);
}


inline bool BasePkg::Sorter::sort_by_name(File* left, File* right) const
{
	return left->name() < right->name();
}


inline bool BasePkg::Sorter::sort_by_size(File* left, File* right) const
{
	return left->size() > right->size();
}

