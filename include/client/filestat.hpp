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

namespace Automaton {
class Interpreter;
	
class FileStat {
public:
	/** 
	 * Creates a new file by loading the table from the Lua environment.
	 * @param env the Lua environment
	 * @param lookup if true, the operating system is queried for actual state
	 */
	FileStat(Interpreter& interp, bool lookup);
	
	int uid;
	int gid;
	std::string path;
	std::string type;
	int mode;
	
private:
	/**
	 * Initializes the values of the file structure by loading them from
	 * the Lua environment.
	 * @param env the Lua environment.
	 */
	void init_from_table(Interpreter& interp);
	
	/**
	 * Initializes the values of the file structure by querying the operating
	 * system.
	 */
	void init_from_os();
};
}
