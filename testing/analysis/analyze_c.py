#!/usr/bin/env python

import sys
import collections
import numpy as np
import pdb
import math
import bisect

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

def predicted_loc(data, loc_data):
  total_precision = 0.0
  pred = 0.0
  for mac in loc_data:
    if (not mac in data):
      continue
    sig = loc_data[mac]
    if (not loc_data[mac] in data[mac]):
      sig = sorted(data[mac].keys())[bisect.bisect_left(data[mac].keys(), loc_data[mac]) - 1]
    if (data[mac][sig]['var'] == 0):
      continue
    precision = 1 / data[mac][sig]['var']
    pred += precision * data[mac][sig]['mean']
    if (math.isnan(pred)):
      pdb.set_trace()
    total_precision += precision
  pred /= total_precision
  return pred, 1 / total_precision

if (len(sys.argv) < 3):
  print "please specify input file and the test file"
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
      print 'fix input file'
      sys.exit(1)
    data = {}
    out_count += 1
    continue

  parts = line.split(',')
  if not parts[0] in data:
    data[parts[0]] = {}
  if not parts[3] in data[parts[0]]:
    data[parts[0]][parts[3]] = { 'locs' : [] }
  data[parts[0]][parts[3]]['locs'].append(location)

for mac in data:
  for sig in data[mac]:
    np_locs = np.array(data[mac][sig]['locs'], float)
    data[mac][sig]['mean'] = np_locs.mean()
    if (math.isnan(data[mac][sig]['mean'])):
      pdb.set_trace()
    data[mac][sig]['var'] = np_locs.var()
    if (math.isnan(data[mac][sig]['var'])):
      pdb.set_trace()

in_file.close()
in_file = open(sys.argv[2])
out_file = open(sys.argv[2] + "_a", "w")

location = 1
loc_data = {}
for line in in_file:
  line = line.strip()
  if (line.startswith("===") or len(line) == 0):
    continue
  if (line.startswith("---")):
    mean, variance = predicted_loc(data, loc_data)
    out_file.write(str(location) + ',' + str(mean) + ',' + str(variance) + '\n')
    loc_data = {}
    location += 1
    continue
  parts = line.split(',')
  loc_data[parts[0]] = parts[3]
out_file.write(str(location) + ',' + str(predicted_loc(data, loc_data)))
out_file.close()
