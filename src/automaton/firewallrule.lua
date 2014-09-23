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

-- Flush existing rules and allow established connections. Ensure iptables is
-- installed.
auto.pre(function() 
    auto.Package { 'iptables' }
    auto.schedule('iptables -F')
    auto.schedule('iptables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT')
    auto.schedule('iptables -A INPUT -i lo -j ACCEPT')
end)

-- Set up the iptables init file and reload the rules from that file.
auto.post(function()
    auto.Directory {
        '/etc/iptables',
        owner='root',
        mode=0700,
    }
    auto.Execute {
        'iptables-save > /etc/iptables/rules.v4',
    }
    auto.File {
        '/etc/iptables/rules.v4',
        owner='root',
        mode='0600',
    }
end)

-- Inserts a firewall rule for the machine.
local function FirewallRule(argv)
    local command = {'iptables'}

    if argv.direction == 'input' then
        table.insert(command, '-A INPUT')
    elseif argv.direction == 'output' then
        table.insert(command, '-A OUTPUT')
    else
        error('invalid direction: '..argv.direction)
    end

    if argv.action == 'accept' then
        table.insert(command, '-j ACCEPT')
    elseif argv.action == 'drop' then
        table.insert(command, '-j DROP')
    else
        error('invalid action: '..argv.action)
    end

    if argv.protocol ~= nil then
        table.insert(command, '-p '..argv.protocol)
    end
    if argv.port ~= nil then
        table.insert(command, '--dport '..argv.port)
    end

    auto.schedule(table.concat(command, ' '))
end

return FirewallRule
