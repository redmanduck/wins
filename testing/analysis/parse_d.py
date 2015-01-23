#!/usr/bin/env python

import sys
import collections

def write_data(data):
  out_file = open(sys.argv[1] + "_p", "w")
  for mac in data:
    out_file.write("mac: " + mac + "\n")
    out_file.write(" loc")
    c_all = collections.Counter(data[mac]['all'])
    for num in c_all:
      out_file.write(",{0:>3}".format(num))

    for loc in data[mac]:
      if loc == 'all':
        continue
      out_file.write('\n{0:4d}'.format(loc))
      c = collections.Counter(data[mac][loc])
      for num in c_all:
        out_file.write(",{0:3d}".format(c[num]))
    out_file.write('\n')
  out_file.close()

if (len(sys.argv) < 2):
  print "please specify input file and if data is continuous or discrete"
  sys.exit(1)

location = 0
in_file = open(sys.argv[1])

data = {}

for line in in_file:
  line = line.strip()
  if (len(line) == 0 or line.startswith("---")):
    continue
  if line.startswith("==="):
    location += 1
    continue

  parts = line.split(',')
  if not parts[0] in data:
    data[parts[0]] = { location : [], 'all' : [] }
  if not location in data[parts[0]]:
    data[parts[0]][location] = []
  data[parts[0]][location].append(parts[3])
  data[parts[0]]['all'].append(parts[3])

write_data(data)
