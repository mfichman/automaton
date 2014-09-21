-- Copyright (c) 2014 Matt Fichman
--
-- Permission is hereby granted, free of charge, to any person obtaining a 
-- copy of this software and associated documentation files (the "Software"),
-- to deal in the Software without restriction, including without limitation 
-- the rights to use, copy, modify, merge, publish, distribute, sublicense, 
-- and/or sell copies of the Software, and to permit persons to whom the 
-- Software is furnished to do so, subject to the following conditions:
-- 
-- The above copyright notice and this permission notice shall be included in 
-- all copies or substantial portions of the Software.
-- 
-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
-- FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
-- AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
-- LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
-- FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
-- IN THE SOFTWARE.

local m = {default={}, override={}}

-- Look up the value in the 't' table first, then check the default table.
local function index(k, override, default)
    local value = override[k]
    if value ~= nil then 
        return value
    end
    local value = default[k]
    if value ~= nil then
        return default[k]
    end
    error('metadata item not found: '..k)
end

-- Check if the table has non-integer keys or not
local function isarray(t) 
    for k, v in pairs(t) do
        if type(k) ~= 'number' then return false end
    end
    return true
end

-- If a nil field is accessed, then return a default-lookup table instead.
-- When the client looks up a non-existing key, e.g., metadata.foo.bar, then 
-- return the default value if it exists (e.g., metadata.default.foo.bar)
local function metatable(override, default) 
    local mt = {}
    mt.__index = function(t, k)
        local overridevalue = override[k]
        local defaultvalue = default[k]
        local value
        if overridevalue ~= nil then
            value = overridevalue
        elseif defaultvalue ~= nil then
            value = defaultvalue 
        else
            error('metadata item not found: '..k)
        end

        if type(overridevalue) == 'table' and type(defaultvalue) == 'table' then
            if isarray(value) then
                return value
            end
            local proxy = {}
            setmetatable(proxy, metatable(overridevalue, defaultvalue))
            return proxy
        else
            return value
        end
    end
    return mt
end

setmetatable(m, metatable(m.override, m.default))

return m
