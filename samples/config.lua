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

s3.accesskey = ''
s3.secretkey = ''

