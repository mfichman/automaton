import zipfile
import tinys3
import os
import time
import sys
import argparse

def pack():
    zf = zipfile.ZipFile('world.zip', 'w')
    for root, dirs, files in os.walk('world'):
        for name in files:
            zf.write(os.path.join(root, name))
    zf.close()

def upload(url):
    accessKey = os.environ['S3_ACCESS_KEY']
    secretKey = os.environ['S3_SECRET_KEY']
    conn = tinys3.Connection(accessKey, secretKey, tls=True)
    fd = open('world.zip', 'rb')
    conn.upload(url, fd, 'mfichman-minecraft')
    fd.close()

def download(url):
    accessKey = os.environ['S3_ACCESS_KEY']
    secretKey = os.environ['S3_SECRET_KEY']
    conn = tinys3.Connection(accessKey, secretKey, tls=True)
    fd = open('world.zip', 'wb')
    resp = conn.get(url, 'mfichman-minecraft')
    for block in resp.iter_content(1024):
        if not block:
            break
        fd.write(block)
    fd.close()

def unpack():
    zf = zipfile.ZipFile('world.zip', 'r')
    zf.extractall()

def main():
    # Download the latest game save, unpack it, then start the server
    parser = argparse.ArgumentParser(prog='s3', description='s3 script')
    subparsers = parser.add_subparsers(dest='command')
    
    sub = subparsers.add_parser('upload')
    sub.add_argument('url', type=str)

    sub = subparsers.add_parser('download')
    sub.add_argument('url', type=str)

    args = parser.parse_args()
    
    if args.command == 'upload':
        pack()
        upload(args.url)
    elif args.command == 'download':
        if os.path.exists('world'):
            print('world already exists')
            sys.exit(0)
        download(args.url)
        unpack()
    else:
        assert(False)

if __name__ == '__main__':
    main()
