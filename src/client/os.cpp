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
#include "builtins.hpp"
#include "interpreter.hpp"

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <string>
#include <sstream>
#include <cerrno>

#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>


using namespace Automaton;

static int os_stat(lua_State* env);
static int os_chmod(lua_State* env);
static int os_chown(lua_State* env);
static int os_getpwnam(lua_State* env);
static int os_getgrnam(lua_State* env);

void Automaton::register_os(Interpreter& interp)
{
	lua_State* env = interp.get_state().get();
	
	interp.get_table("os");
	interp.save("stat", os_stat);
	interp.save("chmod", os_chmod);
	interp.save("chown", os_chown);
	interp.save("getpwnam", os_getpwnam);
	interp.save("getgrnam", os_getgrnam);
	interp.pop<void>();
}

static int os_stat(lua_State* env)
{
	Interpreter& interp = Interpreter::from_env(env);
	
	struct stat buffer;
	struct group* grp;
	struct user* usr;
	if (!stat(lua_tostring(env, -1), &buffer)) {
		lua_newtable(env);
		
		int flags = S_IRWXO | S_IRWXG | S_IRWXU | S_ISUID | S_ISGID;
		int mode = buffer.st_mode & flags;
		
		std::ostringstream ss;
		ss << std::oct << mode;
		
		interp.save("mode", ss.str().c_str());
		interp.save("gid", buffer.st_gid);
		interp.save("uid", buffer.st_uid);
		interp.save("reg", buffer.st_mode & S_IFREG);
		interp.save("dir", buffer.st_mode & S_IFDIR);
		return 1;
		
	} else {
		interp.push(false);
		interp.push(strerror(errno));
		return 2;
	}
}

static int os_chmod(lua_State* env)
{
	Interpreter& interp = Interpreter::from_env(env);
	std::istringstream ss(interp.pop<std::string>());
	std::string file = interp.pop<std::string>();
	
	int mode;
	ss >> std::oct >> mode;
	
	if (!chmod(file.c_str(), mode)) {
		interp.push(true);
		return 1;
		
	} else {
		interp.push(false);
		interp.push(strerror(errno));
		return 2;		
	}
}

static int os_chown(lua_State* env)
{
	Interpreter& interp = Interpreter::from_env(env);
	int gid = interp.pop<int>();
	int uid = interp.pop<int>();
	std::string file = interp.pop<std::string>();
	
	if (!chown(file.c_str(), uid, gid)) {
		interp.push(true);
		return 1;
		
	} else {
		interp.push(true);
		interp.push(strerror(errno));
		return 2;
	}
}

static int os_getpwnam(lua_State* env)
{
	std::string usr_name = lua_tostring(env, -1);
	struct passwd* usr = getpwnam(usr_name.c_str());
	
	if (usr) {
		lua_pushinteger(env, usr->pw_uid);
		return 1;
	} else {
		lua_pushboolean(env, false);
		lua_pushstring(env, strerror(errno));
		return 2;
	}
}

static int os_getgrnam(lua_State* env)
{
	std::string grp_name = lua_tostring(env, -1);
	struct group* grp = getgrnam(grp_name.c_str());
	
	if (grp) {
		lua_pushinteger(env, grp->gr_gid);
		return 1;
	} else {
		lua_pushboolean(env, false);
		lua_pushstring(env, strerror(errno));
		return 2;
	}
}
