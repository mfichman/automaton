local auto = require('automaton')

-- FIXME: These commands are specific to the bash driver; they should be moved
-- to a bash driver initialization routine
auto.schedule('#!/bin/bash')
auto.schedule('set -e')

-- FIXME: Support additional drivers by loading these units from the
-- driver-specific directory.
auto.Spec = require('automaton.spec')
auto.File = require('automaton.file')
auto.Directory = require('automaton.directory')
auto.Execute = require('automaton.execute')
auto.Package = require('automaton.package')
auto.PythonPackage = require('automaton.pythonpackage')
auto.User = require('automaton.user')
auto.Link = require('automaton.link')
auto.FirewallRule = require('automaton.firewallrule')

require('config')
require('spec') -- Load the entry-point spec file


for i, v in ipairs(arg) do
    if v == '--print-slug' then
        auto.action = 'printslug'
    else
        error('invalid argument: '..v)
    end
end

auto.run()



