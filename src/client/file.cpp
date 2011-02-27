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
#include "filestat.hpp"

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
#include <fstream>

using namespace Automaton;
using namespace std;

static int file_up(lua_State* env);
static int file_down(lua_State* env);
static int file_check(lua_State* env);
static void generate_from_text(const std::string& path, const std::string& text);
static void generate_from_template(Interpreter& interp, const std::string& path, const std::string& templ);
static void generate_from_url(const std::string& path, const std::string& url);

#define PCHECK(x) if (x) { throw RuleExecutionException(strerror(errno)); }

void Automaton::register_file(Interpreter& interp)
{
	lua_State* env = interp.get_state().get();
	
	interp.push(file_up);
	interp.push(file_down);
	interp.push(file_check);
	interp.call("executor.File", 3, 1);
	
	assert(lua_istable(env, -1));
	
	interp.get_table("executor");
	lua_insert(env, -2);
	lua_setfield(env, -2, "File");
	interp.pop<void>();
	
	interp.check_stack();
}

static int file_check(lua_State* env)
{
	lua_pushboolean(env, true);
	return 1;
}

static int file_up(lua_State* env)
{	
	int top = lua_gettop(env);
	
	Interpreter& interp = Interpreter::from_env(env);
	FileStat stat(interp, true);
	FileStat actual(interp, true);
	bool mismatch = false;
	
	assert(top == lua_gettop(env));
	
	try {
		std::string generator = interp.load<std::string>("generator");
		std::string source = interp.load<std::string>("source");
		
		if (actual.type == "link" || actual.type == "symlink") {
			PCHECK(unlink(actual.path.c_str()));
		} else if (actual.type == "dir") {
			PCHECK(rmdir(actual.path.c_str()));
		}
		
		if (generator == "text") {
			generate_from_text(stat.path, source); 
		} else if (generator == "url") {
			generate_from_url(stat.path, source);
		} else if (generator == "template") {
			std::cout << "Generating from template" << std::endl;
			generate_from_template(interp, stat.path, source);
		}
		
		if (stat.mode != actual.mode || mismatch) {
			PCHECK(chmod(stat.path.c_str(), stat.mode));
		}
		
		if (stat.uid != actual.uid || stat.gid != actual.gid || mismatch) {
			PCHECK(chown(stat.path.c_str(), stat.uid, stat.gid));
		}

	} catch (RuleExecutionException ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	} 
	
	return 0;
}

static int file_down(lua_State* env)
{
	//Interpreter& interp = env;
	
	try {
	} catch (RuleExecutionException ex) {
		lua_pushstring(env, ex.what());
		lua_error(env);
	}

	return 0;
}


static void generate_from_text(const std::string& path, const std::string& text)
{
	std::ofstream out(path.c_str());
	out << text << std::endl;
}

static void generate_from_template(Interpreter& interp, const std::string& path, const std::string& templ)
{
	interp.push(path.c_str());
	interp.push(templ.c_str());
	interp.get_table("_G");
	interp.call("ltp.render_to_file", 3, 0);
}

static void generate_from_url(const std::string& path, const std::string& url)
{
	std::ifstream ifs(url.c_str(), std::ios::binary);
	std::ofstream ofs(path.c_str(), std::ios::binary);
	ofs << ifs.rdbuf();
}

/**
 	try {
		File desired(env, false);
		File actual(env, true);
		bool mismatch = true;
		
		string content = Interpreter::load<string>(env, "content");
		string source = Interpreter::load<string>(env, "source");
		
		// Check to make sure that the file type is the same as
		// the requested type.
		if (desired.type != actual.type) {
			if (actual.type != "deleted") {
				if (actual.type != "dir") {
					PCHECK(unlink(actual.path.c_str()));
				} else {
					PCHECK(rmdir(actual.path.c_str()));
				}
			}
		}
		
		
		if(desired.type != actual.type) {
			if (desired.type == "link") {
				PCHECK(::link(source.c_str(), desired.path.c_str()));
				
			} else if (desired.type == "symlink") {
				PCHECK(::symlink(source.c_str(), desired.path.c_str()));
				
			} else if (desired.type == "dir") {
				PCHECK(mkdir(desired.path.c_str(), desired.mode));
				
			} else if (desired.type == "file") {
				
			}
		}
		
		
		if (!content.empty()) {
			mismatch = content_patch(actual.path, content);
		} else if (!templ.empty()) {
			mismatch = template_patch(actual.path, content);
		} else if (!source.empty()) {
			mismatch = source_patch(actual.path, content);
		}
		
		

		*/
		