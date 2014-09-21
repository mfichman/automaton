automaton
=========

Automaton is a server configuration management tool, like Puppet or Chef.
Configuration is written in Lua, and then compiled down to a script that a
system-specific driver executes. For Linux hosts, the default driver uses bash.
Automaton compiles the entire server configuration into a single bash
script/slug, transfers the slug via SSH pipe to the remote machine(s), and executes
the script.

Samples
-------

Specify a file's attributes an contents:

```lua
local auto = require('automaton')

auto:File {
    homedir..'/server/eula.txt',
    owner=username,
    mode=0600,
    content='eula=true\n',
}
```

Install packages:

```lua
auto:Package { 
    'python-pip' 
}
```

Execute scripts:

```lua
auto:Execute { 
    'echo hello world!\n' 
}
```


Basic Usage
-----------

First, download `lua` or `luajit`. Then, create a project directory with this format:

```
app/
    config.lua
    spec.lua
    resources/
        minecraft-s3.py
        run.t
```

`config.lua` contains a config info. For example:

```lua
local auto = require('automaton')
local minecraft = {}

minecraft.user = 'minecraft'
minecraft.version = '1.8'
minecraft.ops = {}
minecraft.options = {}
minecraft.options.generatorsettings = ''
minecraft.options.allownether = true
minecraft.options.levelname = 'world'
minecraft.options.enablequery = false
minecraft.options.allowflight = false
```

`spec.lua` contains instructions for configuring the machine:

```lua
auto:User { username }

auto:Directory {
    homedir,
    owner=username,
    mode=0700,
}

auto:File {
    homedir..'/server/minecraft-s3.py',
    owner=username,
    mode=0700,
    content=auto.content('minecraft-s3.py'),
}

auto:File {
    homedir..'/server/run',
    owner=username,
    mode=0700,
    content=auto.template('run.t'),
}
```

Lastly, `resources` contains templates and other files that Automaton transfers to the remote machine:

```
#!/bin/bash
set -e
cd /home/#{minecraft.user}/server
export S3_ACCESS_KEY=#{s3.accesskey}
export S3_SECRET_KEY=#{s3.secretkey}
python minecraft-s3.py download #{minecraft.savename}
java -Xmx1024M -Xms1024M -jar minecraft_server.jar nogui
```

Automaton processes subsitutions with the `#{}` format and substitutes configuration info from `config.lua`

Running
-------

To run your project, do:

```
luajit src/automaton/main.lua
```

