//=======================================================================
// common.cc
//-----------------------------------------------------------------------
// This file is part of the package porg
// Copyright (C) 2015 David Ricart
// For more information visit http://porg.sourceforge.net
//=======================================================================

#include "config.h"
#include "common.h"
#include <sstream>
#include <iomanip>
#include <fnmatch.h>

using std::string;

static string strip_repeated(string const&, char);


//
// Create a human readable size
//
string Porg::fmt_size(float size)
{
	std::ostringstream s;
	
	if (size < KILOBYTE)
		s << (ulong)size;
	else if (size < (10 * KILOBYTE))
		s << std::setprecision(2) << size / KILOBYTE << "k";
	else if (size < MEGABYTE)
		s << (ulong)size / KILOBYTE << "k";
	else if (size < (10 * MEGABYTE))
		s << std::setprecision(2) << size / MEGABYTE << "M";
	else if (size < GIGABYTE)
		s << (ulong)size / MEGABYTE << "M";
	else
		s << std::setprecision(2) << size / GIGABYTE << "G";
		
	return s.str();
}


//
// Convert date to string
//
string Porg::fmt_date(time_t date, bool print_hour)
{
	struct tm* t;
	char str[32] = "";
	string fmt(string("%x") + (print_hour ? " %H:%M" : ""));

	if (date && (t = localtime(&date))
		&& strftime(str, sizeof(str) - 1, fmt.c_str(), t))
		return str;
	else
		// if date == 0, or an error occurs, return a string
		// of whitespaces with the proper length
		return string(fmt_date(time(0), print_hour).size(), ' ');
}


// 
// Match @inpath against each path in @list, following the standard shell-like
// expansion, but with the following exception: If a path in the list does not
// contain any wildcard, and it is a directory, it matches any file within that
// directory.
//
bool Porg::in_paths(string const& inpath, string const& list)
{
	std::istringstream s(list + ":");
	string path = strip_trailing(inpath, '/');

	for (string buf; getline(s, buf, ':'); ) {

		if (buf.empty())
			continue;

		buf = strip_repeated(strip_trailing(buf, '/'), '/');
		
		if (buf == "/")
			return true;

		else if (buf.find_first_of("*?[") == string::npos) {
			if (buf == path || !path.find(buf + "/"))
				return true;
		}
		
		else if (!fnmatch(buf.c_str(), path.c_str(), 0))
			return true;
	}

	return false;
}


//
// Strip consecutive repeated occurrences of character c in str.
//
string strip_repeated(string const& str, char c)
{
	string ret = str, cc(2, c);
	string::size_type p;

	while ((p = ret.find(cc)) != string::npos)
		ret.erase(p, 1);
	
	return ret;
}


//
// Strip trailing occurrences of character @c in @str. If all characters in
// @str are @c, leave one of them unstripped.
//
string Porg::strip_trailing(string const& str, char c)
{
	string ret = str;

	while (ret.size() > 1 && ret[ret.size() - 1] == c)
		ret.erase(ret.size() - 1);
	
	return ret;
}


//
// convert a string to lowercase
//
string Porg::to_lower(string const& str)
{
	string low(str);

	for (uint i(0); i < low.size(); ++i)
		low[i] = tolower(low[i]);

	return low;
}


//
// Generic exception with errno support
//
Porg::Error::Error(string const& msg, int errno_ /* = 0 */)
:
	std::runtime_error(msg + (errno_ ? (string(": ") + strerror(errno_)) : ""))
{ }

