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
import sys
import os

from automaton.rule.spec import Spec
from automaton.rule.file import File
from automaton.rule.package import Package
from automaton.rule.pythonpackage import PythonPackage
from automaton.rule.user import User
from automaton.rule.directory import Directory
from automaton.rule.execute import Execute
from automaton.rule.link import Link
from automaton.rule.firewallrule import FirewallRule
from automaton.rule.daemon import Daemon

# FIXME: These commands are specific to the bash driver; they should be moved
# to a bash driver initialization routine
auto.schedule('#!/usr/bin/env bash -e')

# FIXME: Support additional drivers by loading these units from the
# driver-specific directory. Currently, there is only this one 'main'
# driver. This driver should really be named 'drivers/debian.lua'
auto.Spec = Spec
auto.File = File
auto.Package = Package
auto.PythonPackage = PythonPackage
auto.User = User
auto.Directory = Directory
auto.Execute = Execute
auto.Link = Link
auto.FirewallRule = FirewallRule
auto.Daemon = Daemon

def main():
    sys.path.append(os.getcwd())
    execfile('config.py')
    execfile('spec.py') # Load the entry-point spec file

    auto.action = 'printslug'
    auto.core.run()

if __name__ == '__main__':
    main()

