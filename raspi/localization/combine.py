import sys
import numpy as np
import pdb
import json
from collections import OrderedDict

MIN_SIGNAL_COUNT = 4
MIN_VAR = 25
MIN_AVG = 20

if len(sys.argv) < 3:
    print "Min 3 args"
    sys.exit(1)

lines = OrderedDict()
cur_x = 0
cur_y = 0
first = True

def firstInsert(line):
    if not (cur_x, cur_y) in lines:
        lines[(cur_x, cur_y)] = []
    lines[(cur_x, cur_y)].append(line)

for fname in sys.argv[2:]:
    in_file = open(fname)

    cur_x = 0
    cur_y = 0
    scale = 1

    for line in in_file:
        parts = line.strip().split(',')

        if parts[0].startswith('start'):
            cur_x = int(parts[1])
            cur_y = int(parts[2])
            if first:
                firstInsert(line)
            elif (cur_x, cur_y) not in lines:
                raise Exception('Unknown position')
        elif parts[0].startswith('scale'):
            if first:
                firstInsert(line)
            elif (cur_x, cur_y) not in lines:
                raise Exception('Unknown position')
            scale = float(parts[1])
        elif parts[0].startswith('====='):
            if parts[0][5] == 'N':
                cur_y += scale
            elif parts[0][5] == 'S':
                cur_y -= scale
            elif parts[0][5] == 'E':
                cur_x += scale
            elif parts[0][5] == 'W':
                cur_x -= scale
            elif parts[0][5] == '=':
                continue
            else:
                pdb.set_trace()
                raise Exception('Unknown direction')

            if first:
                firstInsert(line)
            elif (cur_x, cur_y) not in lines:
                raise Exception('Unknown position')
        else:
            if first:
                firstInsert(line)
            else:
                lines[(cur_x, cur_y)].append(line)
    in_file.close

out_file = open(sys.argv[1], 'w')
for p in lines:
    for l in lines[p]:
        out_file.write(l)

out_file.close()
