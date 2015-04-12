import sys
import numpy as np
import argparse
import pdb
import json
from collections import OrderedDict

device = 'android'

def parseOptions():
  parser = argparse.ArgumentParser()
  parser.add_argument("infile", help="data file")
  parser.add_argument("outfile", help="json output file")
  parser.add_argument("minsignalcount", help="min signal count")
  parser.add_argument("--logscale", help="convert dbm to log scale values",
          action='store_true')
  args = parser.parse_args()
  return args

args = parseOptions()

in_file = open(args.infile)
if args.logscale:
    MIN_VAR = 1E+3
else:
    MIN_VAR = 10

cur_x = 0
cur_y = 0
scale = 1

scans_cur_point = []
macs_cur_point = set()
cur_scan = {}
points = []

for line in in_file:
    parts = line.strip().split(',')

    if parts[0].startswith('start'):
        cur_x = int(parts[1])
        cur_y = int(parts[2])
    elif parts[0].startswith('scale'):
        scale = float(parts[1])
    elif parts[0].startswith('device'):
        device = parts[1].strip().lower()
    elif parts[0].startswith('====='):
        if len(cur_scan) > 0:
            scans_cur_point.append(cur_scan)
            cur_scan = {}

        mac_info = []
        for mac in macs_cur_point:
            ss_list = []
            for scan in scans_cur_point:
                if mac in scan:
                    ss_list.append(scan[mac])
            zero_count = int(args.minsignalcount) - len(ss_list)
            if zero_count > 0:
                ss_list.extend([0] * zero_count)

            avg = np.mean(ss_list)
            var = np.var(ss_list, ddof = 1)
            if var < MIN_VAR:
                var = MIN_VAR
            mac_info.append(
                OrderedDict([
                    ('key', mac),
                    ('value', OrderedDict([
                        ('value0', avg),
                        ('value1', var)
                    ]))
                ])
            )

        formatted_scans = []
        for scan in scans_cur_point:
            scan_data = []
            for mac in scan:
                scan_data.append(
                    OrderedDict([
                        ('value0', mac),
                        ('value1', scan[mac])
                    ])
                )
            formatted_scans.append(scan_data)

        costs = [1, 1, 1, 1]
        points.append(
            OrderedDict([
                ('ptr_wrapper', OrderedDict([
                    ('valid', 1),
                    ('data', OrderedDict([
                        ('value0', cur_x),
                        ('value1', cur_y),
                        ('value2', costs),
                        ('value3', mac_info),
                        ('value4', formatted_scans)]
                    ))
                ]))
            ])
        )
        scans_cur_point = []
        macs_cur_point = set()

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
    elif parts[0].startswith('-----'):
        if len(cur_scan) > 0:
            scans_cur_point.append(cur_scan)
            cur_scan = {}
    elif parts[0] == '':
        continue
    else:
        if device == 'android':
            if args.logscale:
                cur_scan[parts[0].strip().lower()] = \
                        float(1E+9 * pow(10, float(parts[2]) / 10))
            else:
                cur_scan[parts[0].strip().lower()] = float(parts[2]) + 100
                #cur_scan[parts[0].strip().lower()] = int(parts[3])
            macs_cur_point.add(parts[0].strip().lower())
        elif device == 'pi':
            realparts = parts[0].split('=')
            if args.logscale:
                cur_scan[realparts[0].strip().lower()] = \
                        float(1E+9 * pow(10, float(realparts[1]) / 10))
            else:
                cur_scan[realparts[0].strip().lower()] = \
                        float(realparts[1]) + 100
            macs_cur_point.add(realparts[0].strip().lower())
in_file.close

d = points[0]['ptr_wrapper']['data']
points[0]['ptr_wrapper']['data'] = OrderedDict([
    ('cereal_class_version', 2),
    ('value0', d['value0']),
    ('value1', d['value1']),
    ('value2', d['value2']),
    ('value3', d['value3']),
    ('value4', d['value4'])
])

out_file = open(args.outfile, 'w')
json.dump({ 'value0' : points }, out_file, indent=4, separators=(',', ': '))
out_file.close()
