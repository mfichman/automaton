/*
 * Copyright (c) 2008 Matt Fichman
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */ 
 
#include "filestat.hpp"
#include "interpreter.hpp"

#include <string>
#include <sstream>
#include <cerrno>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>

using namespace Automaton;
 
FileStat::FileStat(Interpreter& interp, bool lookup)
{	
	path = interp.load<std::string>("path");
	if (lookup) {
		init_from_os();
	} else {
		init_from_table(interp);
	}
}

/* Initialize the file fields by reading them from the Lua table */
void FileStat::init_from_table(Interpreter& interp)
{
	std::istringstream ss(interp.load<std::string>("mode"));
	ss >> std::oct >> mode;
	
	std::string user = interp.load<std::string>("user");
	struct passwd* usr = getpwnam(user.c_str());
	if (!usr) {
		throw RuleExecutionException(strerror(errno));
	}
	uid = usr->pw_uid;
	
	std::string group = interp.load<std::string>("group");
	struct group* grp = getgrnam(group.c_str());
	if (!grp) {
		throw RuleExecutionException(strerror(errno));
	}
	gid = grp->gr_gid;
	
	type = interp.load<std::string>("type");
}

/* Initialize the file fields by reading them from the operating system */
void FileStat::init_from_os()
{
	struct stat buffer;
	if (stat(path.c_str(), &buffer)) {
		mode = -1;
		uid = -1;
		gid = -1;
		type = "deleted";
		return;
	}
	
	int flags = S_IRWXO | S_IRWXG | S_IRWXU | S_ISUID | S_ISGID;
	mode = buffer.st_mode & flags; 
	gid = buffer.st_gid;
	uid = buffer.st_uid;
	
	if (buffer.st_mode & S_IFREG) {
		type = "file";
	} else if (buffer.st_mode & S_IFDIR) {
		type = "dir";
	} else if (buffer.st_mode & S_IFLNK) {
		type = "link";	
	}
}
