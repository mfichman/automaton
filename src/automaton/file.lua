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

-- Sets a file to the specified state. If the content is a URL, then download
-- the file on the remote machine. FIXME: Allow a file to be deleted. Also, 
-- investigate in implementation for automically updating the file. This 
-- function doesn't automatically update the file contents, which might cause
-- problems for some running processes.
local function File(argv)
    local path = argv[1]
    local group = argv.group or argv.owner

    if argv.content == nil then
        auto.schedule('touch '..path)
    elseif type(argv.content) == 'string' then
        auto.schedule('chmod -f +rw '..path..'||true')
        auto.schedule('cat <<EOF > '..path..'\n'..argv.content..'\nEOF')
    elseif argv.content.source == 'remote' then
        auto.schedule('chmod -f +rw '..path..'||true')
        auto.schedule('curl --silent '..argv.content.url..' > '..path)
    else
        error('bad content for file '..path)
    end

    auto.schedule('chmod '..argv.mode..' '..path)
    auto.schedule('chown '..argv.owner..':'..group..' '..path)
end

return File
