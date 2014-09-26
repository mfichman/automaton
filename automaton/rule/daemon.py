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

def pre():
    auto.Package('runit')

auto.pre(pre)

# Sets up a daemon. FIXME: Move this into a RunItDaemon provider.
class Daemon(auto.Rule):

    def __init__(self, name, **kwargs):
        user = kwargs.get('user', name)

        auto.Directory(
            kwargs['workdir'],
            owner=user,
            mode=0700,
        )
    
        auto.Directory(
            '%s/log' % kwargs['workdir'],
            owner=user,
            mode=0700,
        )
    
        auto.File(
            '%s/run' % kwargs['workdir'],
            owner=user,
            mode=0700,
            content='#!/usr/bin/env bash\nset -e\nexec chpst -u%s %s 2>&1' % (name, kwargs['command']),
        )
    
        auto.File(
            '%s/log/run' % kwargs['workdir'],
            owner=user,
            mode=0700,
            content='#!/usr/bin/env bash\nset -e\nexec svlogd -t .'
        )
        
        auto.Link(
            '/etc/service/%s' % name,
            owner='root',
            mode=0600,
            target=kwargs['workdir'],
        )

