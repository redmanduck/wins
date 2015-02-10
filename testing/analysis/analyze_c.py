#!/usr/bin/env python
#!/usr/local/bin/python2.7

import sys
import collections
import numpy as np
import pdb
import math
import bisect

if (len(sys.argv) < 4):
  print 'please specify input file, the test file and the interval'
  sys.exit(1)

out_count = 0
window_size = int(sys.argv[3])

def write_data(data):
  out_file = open(sys.argv[1] + '_p' + str(out_count), 'w')
  for mac in data:
    out_file.write('mac: ' + mac)

    for sig in data[mac]:
      out_file.write('\n{0:3}'.format(sig))
      for loc in data[mac][sig]:
        out_file.write(',{0}'.format(loc))
    out_file.write('\n')
  out_file.close()

def predicted_loc(data, loc_data):
  likely_locs = []

  # For each detected access point (ap)
  for mac in loc_data:
    if (not mac in data):
      continue
    sig = loc_data[mac]

    # If detected signal is not in the map data for the ap,
    # find the nearest known signal strength and do analaysis based
    # on that instead.
    if (not loc_data[mac] in data[mac]['sigs']):
      sorted_sigs = sorted(data[mac]['sigs'].keys())
      prev_index = bisect.bisect_left(sorted_sigs, sig) - 1
      if (prev_index + 1 < len(sorted_sigs) and
          sorted_sigs[prev_index + 1] - sig < sig - sorted_sigs[prev_index]):
        sig = sorted_sigs[prev_index + 1]
      else:
        sig = sorted_sigs[prev_index]

    if (sig == 0):
      pdb.set_trace()
      continue

    # Make a list of all possible locations tagged with the known probability of
    # detecting the signal strength at that point and the known variance in the
    # signal strength for this ap at that point
    for loc in data[mac]['sigs'][sig]['locs']:
      likely_locs.append((loc,
          float(data[mac]['locs'][loc]['counts'][sig]) / window_size,
          data[mac]['locs'][loc]['var']))

  # Determine the probability of being at each of the possible points
  locs = {}
  for loc in likely_locs:
    if (not loc[0] in locs):
      locs[loc[0]] = { 'prob_total' : 0.0, 'precision_total' : 0.0, 'count' : 0 }
    precision = 1 / loc[2]
    locs[loc[0]]['precision_total'] += precision
    locs[loc[0]]['prob_total'] += precision * loc[1]

  total_precision = [ 0.0 ] * 3
  total_weight = [ 0.0 ] * 3
  pred = [ 0.0 ] * 3
  for loc in likely_locs:
    weight = loc[1] * loc[2]
    # print weight
    total_precision[0] += 1 / loc[2]
    total_weight[0] += weight
    pred[0] += loc[0] * weight

  for loc in locs:
    weight = locs[loc]['prob_total'] * locs[loc]['precision_total']
    # print weight
    total_precision[1] += locs[loc]['precision_total']
    total_weight[1] += weight
    pred[1] += loc * weight

  for loc in locs:
    weight = locs[loc]['prob_total'] * locs[loc]['precision_total'] * locs[loc]['precision_total']
    # print weight
    total_precision[2] += locs[loc]['precision_total']
    total_weight[2] += weight
    pred[2] += loc * weight

#  max_weight = 0
#  for loc in locs:
#    weight = locs[loc]['prob_total'] * locs[loc]['precision_total']
#    if (weight > max_weight):
#      max_weight = weight
#      max_i = loc
#    total_precision[2] += locs[loc]['precision_total']
#    total_weight[2] += weight
#    pred[2] += loc * locs[loc]['prob_total']
#  pred[2] = max_i
#  total_precision[2] = locs[max_i]['precision_total']

  for p in range(3):
    pred[p] /= total_weight[p]
  total_precision = [ 1 / p for p in total_precision ]
  # total_weight = [ 1 / w for w in total_weight ]

  return pred, total_precision

location = 0
loc_count = 0
in_file = open(sys.argv[1])

data = {}

for line in in_file:
  line = line.strip()
  if (len(line) == 0):
    continue
  if (line.startswith('---')):
    loc_count += 1
    if (loc_count == window_size):
      location += 1
      loc_count = 0
    continue
  if line.startswith('==='):
    if (not out_count == 0):
      print 'fix input file'
      sys.exit(1)
    data = {}
    out_count += 1
    continue

  parts = line.split(',')
  if not parts[0] in data:
    data[parts[0]] = { 'sigs' : {}, 'locs' : {} }

  if not int(parts[3]) in data[parts[0]]['sigs']:
    data[parts[0]]['sigs'][int(parts[3])] = { 'locs' : [] }
  data[parts[0]]['sigs'][int(parts[3])]['locs'].append(location)

  if not location in data[parts[0]]['locs']:
    data[parts[0]]['locs'][location] = { 'sigs' : [] }
  data[parts[0]]['locs'][location]['sigs'].append(int(parts[3]))

precs = []
for mac in data:
  for loc in data[mac]['locs']:
    sigs = data[mac]['locs'][loc]['sigs']

    # Convert to int list
    data[mac]['locs'][loc]['sigs'] = sigs

    # Assume signal strength of 0 when ap not detected
    sigs.extend([0] * (window_size - len(sigs)))
    np_sigs = np.array(sigs, float)
    var = np_sigs.var()
    if (not var == 0):
      precs.append(1 / var)
    if (var < 20):
      var = 20
    data[mac]['locs'][loc]['var'] = var
    data[mac]['locs'][loc]['counts'] = collections.Counter(sigs)

print 'mean {0}, median {1}'.format(np.array(var).mean(),
    np.median(np.array(var)))

in_file.close()
in_file = open(sys.argv[2])
out_file = open(sys.argv[3] + sys.argv[2] + '_a', 'w')

def write_stats(f, means, location):
  if (len(means) == 0):
    pdb.set_trace()
  f.write(str(location))
  for i in range(3):
    array = [ m[i] for m in means ]
    mean = np.array(array).mean()
    variance = np.array(array).var()
    f.write(',' + str(mean) + ',' + str(variance))
  f.write('\n')

location = 0
loc_data = {}
loc_count = 0
means = []
variances = []
for line in in_file:
  line = line.strip()
  if (line.startswith('===') or len(line) == 0):
    continue
  if (line.startswith('---')):
    mean, variance = predicted_loc(data, loc_data)
    means.append(mean)
    variances.append(variance)
    loc_data = {}
    loc_count += 1
    if (loc_count == window_size):
      write_stats(out_file, means, location)
      location += 1
      loc_count = 0
      means = []
      variances = []
    continue
  parts = line.split(',')
  loc_data[parts[0]] = int(parts[3])
# write_stats(out_file, means, location)
out_file.close()
