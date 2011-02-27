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
#include "executor.hpp"
#include "interpreter.hpp"
#include <cassert>
#include <iostream>
extern "C"
{
	#include <lualib.h>
	#include <lauxlib.h>
}

using namespace Automaton;

static int automaton_add_rule(lua_State* env);

Executor::Executor(Interpreter& interp) :
	interp(interp)
{	
	// Automaton-internal functions and references
	interp.save("automaton", "executor", this);
	interp.save("automaton", "add_rule", automaton_add_rule);
	assert(interp.check_stack());
}

void Executor::load(const std::string& path)
{
	interp.load_sandboxed(path);
}

void Executor::run()
{
	std::list<Rule::ptr>::iterator i;
	for (i = rules.begin(); i != rules.end(); i++) {
		if ((*i)->check()) {
			(*i)->up();
		}
	}
}

void Executor::add_rule(int ref)
{
	rules.push_back(Rule::ptr(new Rule(ref, interp)));
}

static int automaton_add_rule(lua_State* env)
{	
	assert(lua_istable(env, -1));
	int ref = lua_ref(env, LUA_REGISTRYINDEX);
	
	lua_getglobal(env, "automaton");
	lua_getfield(env, -1, "executor");
	Executor* executor = (Executor*)lua_touserdata(env, -1);
	
	// Save a reference to the table
	executor->add_rule(ref);
	lua_pop(env, 2);
	
	assert(lua_gettop(env) == 0);
	return 0;
}
