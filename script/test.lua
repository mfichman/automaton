--[[--package.path = package.path .. ":./?/?.lua"

require('util.cleanmodule')
require('executor', true)
]]
--[[--

local env = {}
env.print = print
local mt = {}
mt.__index = function(table, index)
	return executor[index] or _G[index]
end

setmetatable(env, mt)
setfenv(1, env)
]]

SuperFile = File:sub("SuperFile", function(r)


end)
SuperFile {}



File
{
	user="Matt",
	group="staff",
	mode=777,
	path="/tmp/matt.txt",
	source=template("/tmp/blah")	
}	
