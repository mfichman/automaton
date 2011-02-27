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
#include "rule.hpp"
#include "interpreter.hpp"
extern "C"
{
	#include <lualib.h>
	#include <lauxlib.h>
}

using namespace Automaton;

Rule::Rule(int ref, Interpreter& interp) :
	ref(ref),
	interp(interp)
{
}

Rule::~Rule()
{
	// Delete the reference so that it gets garbage-collected
	if (ref > 0) {
		interp.unref(ref);
	}
}
	
void Rule::up()
{
	call("up", 0);
	assert(interp.check_stack());
}
	
void Rule::down()
{
	call("down", 0);
	assert(interp.check_stack());
}

bool Rule::check()
{
	call("check", 1);
	bool value = interp.pop<bool>();
	assert(interp.check_stack());
	return value;
}

void Rule::call(const std::string& function, int returns)
{
	lua_State* env = interp.get_state().get();
	
	// Look up the function in the object table
	lua_getref(env, ref);
	lua_pushstring(env, function.c_str());
	lua_gettable(env, -2);
	
	// Swap the function ref and the self ref so that the
	// function is above the object table in the stack
	lua_insert(env, -2);

	// Call the function
	if (lua_pcall(env, 1, returns, 0) != 0) {
		std::string message(lua_tostring(env, -1));
		lua_pop(env, 1);
		assert(interp.check_stack());
		throw RuleExecutionException(message);
	}
}