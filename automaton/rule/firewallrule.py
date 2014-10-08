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

# Inserts a firewall rule for the machine.
class FirewallRule(auto.Rule):
    def __init__(self, **kwargs):
        super(FirewallRule, self).__init__()    
        command = ['iptables']

        if kwargs['direction'] == 'input':
            command.append('-A INPUT')
        elif kwargs['direction'] == 'output':
            command.append('-A OUTPUT')
        else:
            raise Exception('invalid direction: %s' % kwargs['direction'])
    
        if kwargs['action'] == 'accept':
            command.append('-j ACCEPT')
        elif kwargs['action'] == 'drop':
            command.append('-j DROP')
        else:
            raise Exception('invalid action: %s' % kwargs['action'])
    
        if 'source' in kwargs:
            command.append('--source %s' % kwargs['source'])
        if 'destination' in kwargs:
            command.append('--destination %' % kwargs['destination'])
        if 'protocol' in kwargs:
            command.append('-p %s' % kwargs['protocol'])
        if 'port' in kwargs:
            command.append('--dport %s' % kwargs['port'])
    
        auto.schedule(' '.join(command))

    @staticmethod
    def pre():
        # Flush existing rules and allow established connections. Ensure
        # iptables is installed.
        auto.Package('iptables')
        auto.schedule('iptables -F')
        auto.schedule('iptables -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT')
        auto.schedule('iptables -A INPUT -i lo -j ACCEPT')


    @staticmethod
    def post():
        # Set up the iptables init file and reload the rules from that file.
        auto.FirewallRule(
            action='drop',
            direction='input',
        )
        auto.Directory(
            '/etc/iptables',
            owner='root',
            mode=0700,
        )
        auto.Execute(
            'iptables-save > /etc/iptables/rules.v4',
        )
        auto.File(
            '/etc/iptables/rules.v4',
            owner='root',
            mode=0600,
        )
