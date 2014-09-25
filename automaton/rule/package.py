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

# Update package database
def pre():
    auto.schedule('apt-get update') 

auto.pre(pre)

# Sets an OS package to the specified state. FIXME: Allow package to be deleted.
class Package(auto.Rule):
    def __init__(self, name, **kwargs):
        if auto.host.packagemanager == 'apt':
            auto.schedule('apt-get install -y %s' % name)
        elif auto.host.packagemanager == 'rpm':
            auto.schedule('rpm install -y %s' % name)
        else:
            raise Exception('unsupported package manager: %s' % auto.host.packagemanager)

