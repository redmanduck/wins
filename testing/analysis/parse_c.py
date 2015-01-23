#!/usr/bin/env python

import sys
import collections

out_count = 0

def write_data(data):
  out_file = open(sys.argv[1] + "_p" + str(out_count), "w")
  for mac in data:
    out_file.write("mac: " + mac)

    for sig in data[mac]:
      out_file.write('\n{0:3}'.format(sig))
      for loc in data[mac][sig]:
        out_file.write(",{0}".format(loc))
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
  if (len(line) == 0):
    continue
  if (line.startswith("---")):
    location += 1
    continue
  if line.startswith("==="):
    if (not out_count == 0):
      write_data(data)
    data = {}
    out_count += 1
    continue

  parts = line.split(',')
  if not parts[0] in data:
    data[parts[0]] = {}
  if not parts[3] in data[parts[0]]:
    data[parts[0]][parts[3]] = []
  data[parts[0]][parts[3]].append(location)

write_data(data)
