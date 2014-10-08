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

class Link(auto.Rule):
    def __init__(self, path, **kwargs):
        super(Link, self).__init__()    
        kind = kwargs.get('kind', 'symbolic')
        group = kwargs.get('group', kwargs['owner'])
    
        if kind == 'symbolic':
            auto.schedule('ln -sfT %s %s' % (kwargs['target'], path))
        elif kind == 'hard':
            auto.schedule('ln -fT %s %s' % (kwargs['target'], path))
        else:
            raise Exception('invalid link kind: %s' % kind)
    
        auto.schedule('chown -h %s:%s %s' % (kwargs['owner'], group, path))
