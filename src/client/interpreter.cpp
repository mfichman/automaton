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
#include "interpreter.hpp"
#include "builtins.hpp"
extern "C"
{
	#include <lualib.h>
	#include <lauxlib.h>
}
#include <sstream>
#include <cerrno>
#include <iostream>

using namespace Automaton;

Interpreter::Interpreter(const std::string& path) :
	state(lua_open(), &lua_close)
{
	lua_State* env = state.get();
	
	// Open libraries and perform basic setup
	luaL_openlibs(env);	
	
	save("package", "path", path.c_str());
	save("automaton", "interpreter", this);
	load("script/bootstrap.lua");
	
	// Load built-in functions
	register_all_builtins(*this);
	
	assert(check_stack());
}
	 
void Interpreter::load_sandboxed(const std::string& path)
{
	lua_pushstring(state.get(), path.c_str());
	get_table("executor");
	call("util.sandbox", 2, 0);
	
}

void Interpreter::load(const std::string& path)
{
	if (luaL_dofile(state.get(), path.c_str()) != 0) {
		std::string message = lua_tostring(state.get(), -1);
		lua_pop(state.get(), 1);
		assert(check_stack());
		throw ScriptLoadException(message);
	}
}
	
void Interpreter::unref(int ref)
{
	lua_unref(state.get(), ref);
}
	
bool Interpreter::check_stack()
{
	return (lua_gettop(state.get()) == 0);
}
	
std::tr1::shared_ptr<lua_State> Interpreter::get_state()
{
	return state;
}

void Interpreter::get_table(const std::string& key)
{
	lua_State* env = state.get();
	lua_getglobal(env, "_G");
	
	if (key.empty()) {
		return;
	}
	
	std::istringstream iss(key);
	std::string token;
	int index = lua_gettop(env);
	assert(!lua_isnil(env, index));
	
	while (std::getline(iss, token, '.')) {	
		// Get the field using the index of the last visited table
		lua_getfield(env, index, token.c_str());
		
		if (lua_isnil(env, -1)) {
			// The field is nil, so pop the nil value, then create a new table,
			// then add that table to the last visited table
			lua_pop(env, 1);
			lua_newtable(env);
			lua_setfield(env, index, token.c_str());
			lua_getfield(env, index, token.c_str());
			
		} else if (!lua_istable(env, -1)) {
			throw InvalidOperationException("Object is not a table");
		}
		
		lua_remove(env, index);
	}
	
	assert(lua_gettop(env) == index);
}

void Interpreter::call(const std::string& name, int args, int results)
{
	int split = name.find_last_of('.');
	std::string prefix; 
	std::string function; 
	
	if (split >= 0) {
		prefix = name.substr(0, split);
		function = name.substr(split + 1, std::string::npos);
	} else {
		function = name;	
	}
	
	lua_State* env = state.get();
	
	// Look up the function in the object table, which should
	// be below the arguments on the stack
	get_table(prefix);
	lua_getfield(env, -1, function.c_str());
	
	// Calling an illegal function
	assert(lua_isfunction(env, -1));
	
	// Move the function BEFORE the arguments, then pop off the lookup
	// table we pushed on the stack with get_table
	lua_insert(env, -args - 2);
	lua_pop(env, 1);
	
	// Call the function 
	if (lua_pcall(env, args, results, 0)) {
		std::string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		throw ScriptExecutionException(message);
	}
}

Interpreter& Interpreter::from_env(lua_State* env)
{
	lua_getglobal(env, "automaton");
	assert(lua_istable(env, -1));
	lua_getfield(env, -1, "interpreter");
	assert(lua_isuserdata(env, -1));
	Interpreter* interp = (Interpreter*)lua_touserdata(env, -1);
	
	lua_pop(env, 2);
	return *interp;
}