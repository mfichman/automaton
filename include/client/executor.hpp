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
#pragma once

#include <string>
#include <list>
#include "rule.hpp"

namespace Automaton {
class Interpreter;

class Executor {
public:
	/**
	 * Creates a new executor using the given interpreter.
	 * @param interp the interpreter
	 */
	Executor(Interpreter& interp);
	
	/**
	 * Loads the rules using the given start script.
	 * @param path the start script
	 * @throw ScriptLoadException if the script couldn't be loaded
	 */
	void load(const std::string& path);
	
	/**
     * Applies the rules to the target computer.
	 */
	void run();
	
	/**
	 * Adds a new rule to the rule set.
	 * @param ref the rule table reference
	 */
	void add_rule(int ref);

private:
	Interpreter& interp;
	std::list<Rule::ptr> rules;
};
}
