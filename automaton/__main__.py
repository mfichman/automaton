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
import argparse
import subprocess
import sys
import os
import paramiko

# FIXME: These commands are specific to the bash driver; they should be moved
# to a bash driver initialization routine
auto.core.precmd.append('#!/usr/bin/env bash')
auto.core.precmd.append('set -e')

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
    parser = argparse.ArgumentParser()
    parser.add_argument('host', nargs='*', type=str)
    args = parser.parse_args()

    sys.path.append(os.getcwd())
    execfile('config.py')
    execfile('spec.py') # Load the entry-point spec file

    payload = auto.core.payload()

    if len(args.host) > 0:
        for host in args.host:
            #ssh.connect(host, username='mfichman', password='@#hsgSiF1QqDaZntXB!!')
            ssh = paramiko.SSHClient()
            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            ssh.connect(host, username='root') #, key_file='~/.ssh/id_rsa') #, key_file=h='@#hsgSiF1QqDaZntXB!!')
            (stdin, stdout, stderr) = ssh.exec_command('bash -c "cat > automaton.sh"') 
            stdin.write(payload)
            stdin.channel.shutdown_write()
            sys.stdout.write(stdout.read())
            sys.stderr.write(stderr.read())
            ssh.close()

            ssh = paramiko.SSHClient()
            ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
            ssh.connect(host, username='root') #, key_file='~/.ssh/id_rsa') #, key_file=h='@#hsgSiF1QqDaZntXB!!')
            (stdin, stdout, stderr) = ssh.exec_command('bash automaton.sh') 
            stdin.write(payload)
            stdin.channel.shutdown_write()
            sys.stdout.write(stdout.read())
            sys.stderr.write(stderr.read())
            ssh.close()
    else:
        print(payload)
            

if __name__ == '__main__':
    main()

