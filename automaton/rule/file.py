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

# Sets a file to the specified state. If the content is a URL, then download
# the file on the remote machine. FIXME: Allow a file to be deleted. Also, 
# investigate in implementation for automically updating the file. This 
# function doesn't automatically update the file contents, which might cause
# problems for some running processes.
class File(auto.Rule):
    def __init__(self, path, **kwargs):
        group = kwargs.get('group', kwargs['owner'])

        if 'content' not in kwargs:
            auto.schedule('touch %s' % path)
        elif type(kwargs['content']) == str:
            auto.schedule('chmod -f +rw %s||true' % path)
            auto.schedule('cat <<EOF > %s\n%s\nEOF' % (path, kwargs['content']))
        elif kwargs['content']['source'] == 'remote':
            auto.schedule('chmod -f +rw %s||true' % path)
            auto.schedule('curl --silent %s > %s' % (kwargs['content']['url'], path))
        else:
            raise Exception('bad content for file: %s' % path)
         
        auto.schedule('chmod %s %s' % (oct(kwargs['mode']), path))
        auto.schedule('chown %s:%s %s' % (kwargs['owner'], group, path))

