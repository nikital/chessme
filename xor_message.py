#! /usr/bin/env python

import sys
from itertools import cycle

def xor (message, key):
    key = cycle (ord(c) for c in key)
    for c in message:
        yield chr (ord (c) ^ key.next ())

def main ():
    key = sys.argv[1]
    with open (sys.argv[2]) as f:
        message = f.read ()
    message = ''.join (xor (message, key))

    encoded = ''.join ('\\x{:02x}'.format (ord (i)) for i in message)

    print '"{}"'.format (encoded)

if __name__ == '__main__':
    main ()
