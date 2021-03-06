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

local os = require('os')
local io = require('io')
local string = require('string')
local m = {}

m.metadata = require('automaton.metadata')
m.host = {}

m.metadata.default.ssh = {}
m.metadata.default.ssh.host = '127.0.0.1'
m.metadata.default.ssh.port = 22
m.metadata.default.ssh.privatekey = '~/.ssh/id_rsa'
m.metadata.default.ssh.user = 'automaton'

-- FIXME: Detect package manager
m.host.packagemanager = 'apt'

local command = {}
local pre = {}
local post = {}

-- Internal: Schedule a function to run before commands are executed.
function m.pre(fn)
    table.insert(pre, fn)
end

-- Internal: Schedule a function to run after commands are executed.
function m.post(fn)
    table.insert(post, fn)
end

-- Internal: Schedule a command to run the compiled script/slug.
function m.schedule(cmd)
    table.insert(command, cmd)
end

-- Specify that Automaton should download file contents from a remote host.
function m.remote(url)
    return {url=url, source='remote'}
end

-- Load the contents of a resource file for use in a `File` directive.
function m.content(path)
    local fd = io.open(m.resourcepath..'/'..path)
    if not fd then
        error('resource '..path..' not found')
    end
    return fd:read('*all')
end

-- Replace all expression found in the templatized #{} blocks with variables
-- from the metadata store. 
function m.template(path)
    local fd = io.open(m.resourcepath..'/'..path)
    if not fd then
        error('resource '..path..' not found')
    end
    local raw = fd:read('*all')

    local function replace(match)
        local item = m.metadata
        for name in match:gmatch('(%w+)') do
            item = item[name]
        end
        return tostring(item)
    end
    return raw:gsub('#{(.-)}', replace)
end

-- Open a connection using SSH, and pipe input/ouput to/from the remote process.
-- FIXME: This is driver-specific, and should be in a driver function.
function m.ssh(args, mode)
    local cmd = { 'ssh',
        '-l', m.metadata.ssh.user,
        '-i', m.metadata.ssh.privatekey,
        '-o', 'StrictHostKeyChecking=no', 
        '-o', 'UserKnownHostsFile=/dev/null',
        '-p', m.metadata.ssh.port,
        m.metadata.ssh.host,
    }
    for i, v in ipairs(args) do
        table.insert(cmd, v)
    end
    return io.popen(table.concat(cmd, ' '), mode)
end

-- Upload compiled shell script and execute.
-- FIXME: This is driver-specific, and should be in a driver function.
function m.run()
    local maincmds = table.concat(command, '\n')

    command = {}
    for i, fn in ipairs(pre) do
        fn()
    end
    local precmds = table.concat(command, '\n')

    command = {}
    for i, fn in ipairs(post) do
        fn()
    end
    local postcmds = table.concat(command, '\n')

    local payload = string.gsub(table.concat({precmds, maincmds, postcmds}, '\n'), '\r\n', '\n')
    if m.action == 'printslug' then
        print(payload)
        return
    end

    local fd = m.ssh({'bash -c "cat > automaton.sh"'}, 'wb')
    fd:write(payload)
    fd:close()

    local fd = m.ssh({'sudo', 'bash', 'automaton.sh'}, 'wb')
    fd:close()
end

return m
