import operator
import sys, json
import md5
f = open(sys.argv[1],"r")
dir = {}
for line in f.readlines():
    li = line.split(" ")
    apcnt = int( li[0])
    freq = int(li[1])
    if not freq in dir:
        dir[freq] = 0
    dir[freq] += apcnt/12.0

sorted_dir = sorted(dir.items(), key=operator.itemgetter(1))
print sorted_dir


# 2412, 2437, 2462, 5240,5745, 5180,5200, 5785,5220, 5765,5805
# 1   ,  6   , 11 , 48, 149,36,  40, 157, 44, 153,161
