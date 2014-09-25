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

import sys

class Hash(object):
    pass

class Proxy(object):
    """
    Return a value from the override config namespace, or the default namespace
    if the override namespace doesn't exist.
    """

    def __init__(self, default, override):
        self.default = default
        self.override = override

    def __getattr__(self, name):
        default = getattr(self.default, name)
        override = getattr(self.override, name, None)
        if type(default) == Hash:
            return Proxy(default, override)
        elif override != None:
            return override
        else:
            return default

class Metadata(Proxy):
    Hash = Hash
    default = Hash()
    override = Hash()

    def __init__(self):
        super(Metadata, self).__init__(self.default, self.override)

    def __getattr__(self, name):
        if name == 'override':
            return self.override
        elif name == 'default':
            return self.default
        else:
            return super(Metadata, self).__getattr__(name)

metadata = Metadata()


