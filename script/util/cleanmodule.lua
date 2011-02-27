local rawrequire = require;

-- Declare module cleanly.
-- Create both public and private namespaces for module.
-- Global assignments inside module get placed in both
-- public and private namespaces.
function module(file, modname)
	package.private = package.private or {}
	local info = package.private[modname]
	
	if (info) then
		setfenv(2, info.priv)
		package.loaded[file] = info.pub

	else
		local pub = {}     -- public namespace for module
		local priv = {}  -- private namespace for module
		local privmt = {}
		privmt.__index = _G
		privmt.__newindex = function(priv, k, v)
			rawset(pub, k, v)
			rawset(priv, k, v)
		end
		setmetatable(priv, privmt)
		setfenv(2, priv)

		info = {}
		info.pub = pub
		info.priv = priv
		info.modname = modname
		
		package.loaded[file] = info
		package.private[modname] = info
	end
	
	--print(file, modname, "done")
end

-- Require module, but store module only in
-- private namespace of caller (not public namespace).
function require(name, add_to_env)
  	local result = rawrequire(name)
  	if (add_to_env) then
		rawset(getfenv(2), result.modname, result.pub)
	end
	return result.pub 
end

