module(..., 'util')

function sandbox(file, env)
	local mt = {}
	
	local public = {print = print}
	mt.__index = function(t, k)
		return env[k]
	end
	mt.__newindex = function(t, k, v)
		if (env[k]) then
			error("Cannot assign value to built-in")
		else
			rawset(t, k, v)
		end
	end
	setmetatable(public, mt)
	
	local main = assert(loadfile(file))
	setfenv(main, public)
	main()
end
