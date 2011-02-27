-- Copyright (c) 2008 Matt Fichman
--
-- Permission is hereby granted, free of charge, to any person obtaining a 
-- copy of this software and associated documentation files (the "Software"), 
-- to deal in the Software without restriction, including without limitation 
-- the rights to use, copy, modify, merge, publish, distribute, sublicense, 
-- and/or sell copies of the Software, and to permit persons to whom the
-- Software is furnished to do so, subject to the following conditions:
--
-- The above copyright notice and this permission notice shall be
-- included in all copies or substantial portions of the Software.
--
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
-- IN THE SOFTWARE.

module(..., 'executor')
require('util', true)

Rule = util.Object:new() 

-- Creates a new rule using the parent 'self' and o
-- as a table of default values.  Checks to make sure all
-- variables are appropriately defined.
-- @param self the parent
-- @param o the table of default values 
local function rnew(self, o)
	o = o or {}
	setmetatable(o, self)
	self.__index = self

	if (o.variables) then
		for i,v in pairs(o.variables) do
			if o[v] == nil then 
				self:throw("Undefined rule variable '" .. v .. "'") 
			end
		end
	end
	if (o.init) then o:init() end
	automaton.add_rule(o);
	return o
end

-- Creates a new object from the given prototype table.
-- @param o the table of default values and functions
-- @return an object using 'o' as the prototype
function Rule:new(o)
	if (type(o) == "string") then
		return function(t)
			getfenv(2)[o] = rnew(self, t) 
			getfenv(2)[o].rule_name = o
			return getfenv(2)[o]
		end
	else
		return rnew(self, o)
	end
end

-- Creates a new class from the given prototype table.
-- @param class the name of the new class
-- @param block the block that will be used to initialize the rule
function Rule:sub(class, block)

	local o = util.Object.sub(self, {class=class})
	block(o)
	
	local public = {}
	public.new = function(dummy, ...) return o:new(...) end
	public.sub = function(dummy, ...) return o:sub(...) end
	public.description = o.description
	
	local publicmt = {}
	publicmt.__index = public
	publicmt.__call = function(dummy, ...) return o:new(...) end
	publicmt.__metatable = false
	--publicmt.__newindex = function(t, k, v) error("Object is read-only") end

	local proxy = {}
	setmetatable(proxy, publicmt)
	getfenv(0)[class] = proxy
	return proxy
end

-- Adds a list of required variables to the variables list.
-- @param list a list of required variables
function Rule:requires(list)
	self.variables = self.variables or {}
	for i,v in ipairs(list) do
		table.insert(self.variables, v)
	end
end

-- Throws an error for this rule.
function Rule:throw(string, level)
	error(self.class .. ": " .. string, level)
end
