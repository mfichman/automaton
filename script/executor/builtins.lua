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
require('executor.rule')

-- Returns the parameters for a templated file
function template(path)
	return {"template", path}
end

-- Returns the parameters for a copied file
function url(path)
	return {"url", path}
end

-- Returns the parameters for a text file
function text(str)
	return {"text", str}
end

-- Links the rule definition with the 
local function builtin(name, setup)
	getfenv(2)[name] = function(up, down, check)
		return Rule:sub(name, function(r)
			setup(r)
			r.up = up
			r.down = down
			r.check = check
		end)
	end
end

-- Controls directory contents, ownership and permissions
builtin("Directory", function(r)
	r:requires {"group", "user", "mode", "path", "content"}
	r.description = "Controls directory contents, ownership and permissions"
end)

-- Controls file contents, ownership and permissions
builtin("File", function(r)
	r:requires {"group", "user", "mode", "path", "source"}
	r.description = "Controls file contents, ownership, and permissions"
	
	function r:init()
		self.generator = self.source[1]
		self.source = self.source[2]
	end
end)

-- Installs and removes software packages
builtin("Package", function(r)
	r.variables = {"name", "version"}
	r.description = "Installs and removes software packages"
end)

-- Creates hard and symbolic links
builtin("Link", function(r)
	r:requires {"group", "user", "mode", "path", "target", "symbolic"}
	r.description = "Creates hard and symbolic links";
	r.symbolic = true
end)

-- Runs shell commands for a task
builtin("Task", function(r)
    r:requires {"user", "command"}
	r.description = "Runs shell commands"
end)

--[[
-- Controls file contents, ownership and permissions
Rule:sub("File", function(r)
	r:requires {"group", "user", "mode", "path", "source"}
	r.description = "Controls file contents, ownership, and permissions"
	
	r.up = automaton.file.up
	r.down = automaton.file.down
	r.check = function() return true end
	
	function r:init()
		self.generator = self.source[1]
		self.source = self.source[2]
	end
end)]]
