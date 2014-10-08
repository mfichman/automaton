# Copyright (c) 2014 Matt Fichman
#
# Permission is hereby granted, free of charge, to any person obtaining a 
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation 
# the rights to use, copy, modify, merge, publish, distribute, sublicense, 
# and/or sell copies of the Software, and to permit persons to whom the 
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in 
# all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
# IN THE SOFTWARE.

import automaton as auto
import os
import re
import itertools

command = []
precmd = []
postcmd = []

class Rule(object): 
    def __init__(self):
        if not getattr(self.__class__, '_init', False):
            global command, precmd, postcmd
            save = command
            self.__class__._init = True 
            command = precmd
            self.__class__.pre()
            command = postcmd
            self.__class__.post()
            command = save

    @staticmethod
    def pre():
        pass

    @staticmethod
    def post():
        pass
         

host = auto.metadata.Hash()
host.os = 'debian' # red hat
# FIXME: Detect os version

auto.metadata.default.ssh = auto.metadata.Hash()
auto.metadata.default.ssh.host = '127.0.0.1'
auto.metadata.default.ssh.port = 22
auto.metadata.default.ssh.privatekey = '~/.ssh/id_rsa'
auto.metadata.default.ssh.user = 'automaton'

# Internal: Schedule a command to run the compiled script/slug.
def schedule(cmd):
    command.append(cmd)

# Specify that Automaton should download file contents from a remote host.
def remote(url):
    return {'url':url, 'source':'remote'}

# Load the contents of a resource file for use in a `File` directive.
def content(path):
    fd = open(auto.resourcepath+'/'+path)
    return fd.read()

# Replace all expression found in the templatized #{} blocks with variables
# from the metadata store.
def template(path):
    fd = open(auto.resourcepath+'/'+path)
    raw = fd.read()

    def replace(match):
        item = auto.metadata
        for name in re.findall('(\w+)', match.group(1)):
            item = getattr(item, name)
        if type(item) == bool:
            return 'true' if item else 'false'
        else:
            return str(item)

    return re.sub('#{(.*?)}', replace, raw)

# Open a connection using SSH, and pipe input/ouput to/from the remote process.
# FIXME: This is driver-specific, and should be in a driver function.
def ssh(args, mode):
    # FIXME: Use paramiko
    pass
"""
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
"""

# Upload compiled shell script and execute.
# FIXME: This is driver-specific, and should be in a driver function.
def payload():
    return '\n'.join(itertools.chain(precmd, command, postcmd))+'\n'

"""
    FIXME: Use paramiko
    local fd = m.ssh({'bash -c "cat > automaton.sh"'}, 'wb')
    fd:write(payload)
    fd:close()

    local fd = m.ssh({'sudo', 'bash', 'automaton.sh'}, 'wb')
    fd:close()
"""

