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

local auto = require('automaton')
local table = require('table')
local username = auto.metadata.minecraft.user
local homedir = '/home/'..username
local version = auto.metadata.minecraft.version


-- Install required packages
auto.Package { 'openjdk-7-jre' }
auto.Package { 'runit' }
auto.PythonPackage { 'tinys3' }

-- Set up user and home directories for minecraft server
auto.User { username }

auto.Directory {
    homedir,
    owner=username,
    mode=0700,
}

auto.Directory {
    homedir..'/server', 
    owner=username,
    mode=0700,
}

-- Executable jar file
auto.File {
    homedir..'/server/minecraft_server.jar',
    owner=username,
    mode=0700,
    content=auto.remote('https://s3.amazonaws.com/Minecraft.Download/versions/1.8/minecraft_server.'..version..'.jar'),
}

-- Configuration files
auto.File {
    homedir..'/server/eula.txt',
    owner=username,
    mode=0600,
    content='eula=true\n',
}

auto.File {
    homedir..'/server/ops.txt',
    owner=username,
    mode=0600,
    content=table.concat(auto.metadata.minecraft.ops),
}

auto.File {
    homedir..'/server/server.properties',
    owner=username,
    mode=0600,
    content=auto.template('server.properties.t'),
}

auto.File {
    '/root/.bashrc',
    owner='root',
    mode=0400,
    content=auto.template('bashrc.t'),
}

-- Backup script
auto.File {
    homedir..'/server/minecraft-s3.py',
    owner=username,
    mode=0700,
    content=auto.content('minecraft-s3.py'),
}

-- Firewall entry for server port
auto.FirewallRule {
    action='accept',
    direction='input',
    port=auto.metadata.minecraft.options.serverport,
    protocol='tcp',
}

-- Startup script & link service to runit
auto.File {
    homedir..'/server/run',
    owner=username,
    mode=0700,
    content=auto.template('run.t'),
}

auto.Link {
    '/etc/service/minecraft',
    owner='minecraft',
    mode=0600,
    target=homedir..'/server',
}
