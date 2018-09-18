#!/usr/bin/env python

import sys

for line in sys.stdin:
    if line not in ["0","0\n"]:
        sys.stdout.write("Error detected!\nLine: {}".format(line))
    else:
        sys.stdout.write("Test pass!\n")
