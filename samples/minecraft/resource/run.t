#!/usr/bin/env bash -e
cd /home/#{minecraft.user}/server
export S3_ACCESS_KEY=#{s3.accesskey}
export S3_SECRET_KEY=#{s3.secretkey}
python minecraft-s3.py download #{minecraft.savename}
java -Xmx1024M -Xms1024M -jar minecraft_server.jar nogui
