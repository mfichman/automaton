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
#include "exceptions.hpp"

#include <iostream>

namespace Automaton { 
	
class Interpreter {
	
public:
	typedef int (*funct)(lua_State*);

	/**
	 * Creates a new script interpreter to load Lua files.
	 * @param path the search path
	 */
	 Interpreter(const std::string& path);

	 /**
	  * Loads the given file into the interpreter.
	  * @param path the path to the script file
	  * @throw ScriptLoadException if the script failed to load
	  */
	void load(const std::string& path);
	
	/**
	 * Loads the file in a sandbox.
	 */
	void load_sandboxed(const std::string& path);
	
	/**
     * Deletes a reference from the Lua interpreter.
	 * @param ref the reference
	 */
	void unref(int ref);
	
	/**
	 * Checks the lua stack to make sure that it is empty.
	 * @return true if the stack is empty, false otherwise
	 */
	bool check_stack();
	
	/**
	 * Returns a pointer to the the Lua interpreter.
	 * @return the internal Lua interpreter
	 */
	std::tr1::shared_ptr<lua_State> get_state();
	
	/**
	 * Pops an object off the stack.
	 * @return an object of the requested type
	 * @throw StackUnderflowException if the stack is empty
	 */
	template <typename T>
	T pop();
	
	/**
	 * Pushes an object onto the stack.
	 * @param value and object
	 */
	template <typename T>
	void push(T value);
	
	/**
	 * Loads an item from a table on top of the stack
	 * @param key the table key
	 */
	template <typename T>
	T load(const std::string& key);
	
	/**
	 * Saves a value.
	 */
	template <typename T>
	T load(const std::string& table, const std::string& key);
	
	/**
	 * Saves the pointer on the table.
	 * @param key the key to save at
	 * @param pointer the pointer
	 */
	template <typename T>
	void save(const std::string& key, T value);
	
	/**
	 * Saves a value.
	 */
	template <typename T>
	void save(const std::string& table, const std::string& key, T value);
	
	/**
	 * Retrieves a value and puts it on the stack.
	 * @param key the global key
	 */
	void get_table(const std::string& key);
	
	/**
	 * Calls the Lua function in the given table
	 * @param name the name of the function
	 * @param args the number of arguments
	 * @param results the number of results
	 */
	void call(const std::string& name, int args, int results);
	
	/**
	 * Loads the associated interpreter from the given Lua environment.
	 * @param env the Lua environment
	 * @return the interpreter
	 */
	static Interpreter& from_env(lua_State* env);
	
	/**
	 * Creates a new table.
	 */
	void new_table();
	
private:
	std::tr1::shared_ptr<lua_State> state;
};

template <>
inline int Interpreter::pop()
{
	lua_State* env = state.get();
	if (lua_gettop(env) == 0) {
		throw StackUnderflowException();	
	}
	int value = lua_tointeger(env, -1);
	lua_pop(env, 1);
	return value;
}

template <>
inline bool Interpreter::pop()
{
	lua_State* env = state.get();
	if (lua_gettop(env) == 0) {
		throw StackUnderflowException();	
	}
	bool value = lua_toboolean(env, -1);
	lua_pop(env, 1);
	return value;
}

template <>
inline std::string Interpreter::pop()
{
	lua_State* env = state.get();
	if (lua_gettop(env) == 0) {
		throw StackUnderflowException();	
	}
	
	std::string value;
	if (!lua_isnil(env, -1)) {
		value = lua_tostring(env, -1);
	}
	
	lua_pop(env, 1);
	return value;
}

template <typename T>
inline T Interpreter::pop()
{
	lua_State* env = state.get();
	if (lua_gettop(env) == 0) {
		throw StackUnderflowException();
	}
	
	T value;
	if (!lua_isnil(env, -1)) {
		value = lua_touserdata(env, -1);
	}
	
	lua_pop(env, 1);
	return value;
}

template <>
inline void Interpreter::pop()
{
	lua_pop(state.get(), 1);	
}

template <>
inline void Interpreter::push(int value)
{
	lua_pushinteger(state.get(), value);
}

template <>
inline void Interpreter::push(unsigned int value)
{
	lua_pushinteger(state.get(), value);
}

template <>
inline void Interpreter::push(bool value)
{
	lua_pushboolean(state.get(), value);
}

template <>
inline void Interpreter::push(const std::string value)
{
	lua_pushstring(state.get(), value.c_str());
}

template <>
inline void Interpreter::push(const char* value)
{
	lua_pushstring(state.get(), value);	
}

template <typename T>
inline void Interpreter::push(T value)
{
	lua_pushlightuserdata(state.get(), value);
}

template <>
inline void Interpreter::push(funct value)
{
	lua_pushcfunction(state.get(), value);	
}

template <typename T>
inline T Interpreter::load(const std::string& key)
{	
	lua_State* env = state.get();
	lua_getfield(env, -1, key.c_str());
	return pop<T>();
}

template <typename T>
inline T Interpreter::load(const std::string& table, const std::string& key)
{
	get_table(table);
	T t = load<T>(key);
	pop<void>();
	return t;
}

template <typename T>
inline void Interpreter::save(const std::string& key, T value)
{
	lua_State* env = state.get();
	push(value);
	lua_setfield(env, -2, key.c_str());
}

template <typename T>
inline void Interpreter::save(const std::string& table, const std::string& key, T value)
{
	get_table(table);
	save(key, value);
	pop<void>();
}

inline void Interpreter::new_table()
{
	lua_newtable(state.get());	
}

}
