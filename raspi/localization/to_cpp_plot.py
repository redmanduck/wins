import sys
import os
import numpy as np
import pdb
import json
from collections import OrderedDict

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt

MIN_SIGNAL_COUNT = 10
MIN_VAR = 25
MIN_AVG = 20

if len(sys.argv) < 3:
    print "Min 3 args"
    sys.exit(1)

in_file = open(sys.argv[1])

cur_x = 0
cur_y = 0
scale = 1

scans_cur_point = []
macs_cur_point = []
cur_scan = {}
points = []

plot_info = OrderedDict()

for line in in_file:
    parts = line.strip().split(',')

    if parts[0].startswith('start'):
        cur_x = int(parts[1])
        cur_y = int(parts[2])
    elif parts[0].startswith('scale'):
        scale = float(parts[1])
    elif parts[0].startswith('====='):
        mac_info = []
        for mac in macs_cur_point:
            ss_list = []
            for scan in scans_cur_point:
                if mac in scan:
                    ss_list.append(scan[mac])
            zero_count = MIN_SIGNAL_COUNT - len(ss_list)
            if zero_count > 0:
                ss_list.extend([0] * zero_count)

            avg = np.mean(ss_list)
            var = np.var(ss_list, ddof = 1)
            if var < MIN_VAR:
                var = MIN_VAR
            if avg >= MIN_AVG:
                mac_info.append(
                    OrderedDict([
                        ('key', mac),
                        ('value', OrderedDict([
                            ('value0', avg),
                            ('value1', var)
                        ]))
                    ])
                )
            if not mac in plot_info:
                plot_info[mac] = [[] for x in xrange(6)]
            plot_info[mac][0].append(cur_x - 0.25)
            plot_info[mac][1].append(cur_y - 0.25)
            plot_info[mac][2].append(0)
            plot_info[mac][3].append(0.5)
            plot_info[mac][4].append(0.5)
            plot_info[mac][5].append(avg)

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
        points.append(
            OrderedDict([
                ('ptr_wrapper', OrderedDict([
                    ('valid', 1),
                    ('data', OrderedDict([
                        ('value0', cur_x),
                        ('value1', cur_y),
                        ('value2', mac_info),
                        ('value3', formatted_scans)]
                    ))
                ]))
            ])
        )
        scans_cur_point = []
        macs_cur_point = []

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
        scans_cur_point.append(cur_scan)
        cur_scan = {}
    elif parts[0] == '':
        continue
    else:
        cur_scan[parts[0]] = int(parts[3])
        macs_cur_point.append(parts[0])
in_file.close

d = points[0]['ptr_wrapper']['data']
points[0]['ptr_wrapper']['data'] = OrderedDict([
    ('cereal_class_version', 2),
    ('value0', d['value0']),
    ('value1', d['value1']),
    ('value2', d['value2']),
    ('value3', d['value3'])
])

out_file = open(sys.argv[2], 'w')
json.dump({ 'value0' : points }, out_file, indent=4, separators=(',', ': '))
out_file.close()

if not os.path.exists("plots"):
    os.makedirs("plots")
count = 0
for mac in plot_info:
    fig = plt.figure()
    plt.axis('equal')
    ax = fig.add_subplot(111, projection='3d')
    ax.bar3d(*plot_info[mac], color='b', zsort='average')
    ax.view_init(elev=56, azim=24)
    plt.xlim(-12, 0)
    plt.ylim(0, 44)
    ax.set_zlim(0, 100)
    plt.savefig('plots/' + str(count) + '_' + mac + '.png', dpi=200, bbox_inches='tight')
    plt.close()
    count += 1
