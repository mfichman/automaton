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

#include "common.hpp"

namespace Automaton {
class Interpreter;

class Rule {
public:
	/**
	 * Creates a new rule from the given script.
	 * @param script the path to the Lua rule script.
	 * @param ref reference to the rule object in the Lua interpreter.
	 */
	Rule(int ref, Interpreter& interp);
	
	/**
     * Destroys the rule object.
	 */
	~Rule();
	
	/**
     * Applies this rule to the machine.
	 * @throw RuleExecutionException if the rule could not be executed
	 */
	void up();
	
	/**
     * Removes this rule from the machine.
	 * @throw RuleExecutionException if the rule could not be executed
	 */
	void down();
	
	/**
	 * Determines if the rule is present on the machine.
	 * @throw RuleExecutionException if the rule could not be checked
	 */
	bool check();
	
	typedef std::tr1::shared_ptr<Rule> ptr;
	
private:
	/**
	 * Calls the given member function with no arguments on the stack.
	 * @param function name of the function to call
	 * @param returns the number of return values
	 */
	void call(const std::string& function, int returns);
	
	Rule(const Rule& rule);
	void operator=(const Rule& rule);

	int ref;
	Interpreter& interp;
};
}
