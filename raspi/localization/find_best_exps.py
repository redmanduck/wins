import os
import sys
import numpy as np
import pdb

dirs = [d for d in os.listdir('.') if os.path.isdir(d) and d.startswith('ee')]

maps = {}

for d in dirs:
    files = [f for f in os.listdir(d) if 'summary' not in f]
    for f in files:
        in_file = open(d + '/' + f)
        #data = maps.setdefault(int(f[8:10].strip('_')) % 6, {})
        data = maps.setdefault(f, {})
        rank = 0
        for line in in_file:
            vals = line.split()
            if vals[0] == 'mean':
                continue
            vals = map(float, vals)

            initial = data.get((vals[2], vals[3]), [0, 0, 0, 0])
            data[(vals[2], vals[3])] = map(sum, zip(initial, (rank, vals[0],
                    vals[1], 1)))
            rank += 1
out_file = open('best_exps.txt', 'w')
num_types = len(dirs)
for f in maps:
    for k in maps[f].keys():
        if maps[f][k][3] != num_types:
            del maps[f][k]
    out_file.write(str(f) + '\n')
    out_file.write('%5s %5s %5s %5s %5s\n' % ('arank', 'amean', 'astd', 'e1', 'e2'))
    l = sorted(maps[f].items(), key=lambda k: k[1])
    for v in l[:20]:
        out_file.write('%5.0f %5.3f %5.3f %5.2f %5.2f\n' %
                (v[1][0] / num_types, v[1][1] / num_types, v[1][2] / num_types, v[0][0], v[0][1]))
    out_file.write('\n')
