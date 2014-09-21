local auto = require('automaton')
local minecraft = {}
local s3 = {}
local ssh = {}
auto.metadata.override.minecraft = minecraft
auto.metadata.override.s3 = s3
auto.metadata.override.ssh = ssh

ssh.host = '127.0.0.1'
ssh.port = 2222
ssh.privatekey = 'C:/Users/Matt/.vagrant.d/insecure_private_key'
ssh.user = 'vagrant'

minecraft.ops = {'mfichman'}
minecraft.savename = '2014-09-20v2'
minecraft.options = {}
minecraft.options.motd = 'WASSUP YA DINGUS'
minecraft.options.onlinemode = true

s3.accesskey = 'AKIAI52RMBODZ3ILQ7LA'
s3.secretkey = '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'

