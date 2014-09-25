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

auto.pre(function() 
    auto.Package {'runit'}
end)

-- Sets up a daemon. FIXME: Move this into a RunItDaemon provider.
local function Daemon(argv)
    local name = argv[1]   
    local user = argv.user or name

    auto.Directory {
        argv.dir,
        owner=user,
        mode=0700,
    }

    auto.Directory {
        argv.dir..'/log',
        owner=user,
        mode=0700,
    }

    auto.File {
        argv.dir..'/run',
        owner=user,
        mode=0700,
        content=string.format('#!/bin/bash -e\nexec chpst -u%s %s 2>&1', name, argv.command),
    }

    auto.File {
        argv.dir..'/log/run',
        owner=user,
        mode=0700,
        content='#!/bin/bash\nexec svlogd -t .'
    }
    
    auto.Link {
        '/etc/service/'..name,
        owner='root',
        mode=0600,
        target=argv.dir,
    }

    auto.Execute { 'sv start /etc/service/'..name..'||true' }
end

return Daemon
