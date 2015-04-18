import operator
import sys, json
import md5
f = open(sys.argv[1],"r")
AP={}
lastmac = ''
for line in f.readlines():
	if "BSS" in line:
		mac = line[4:].replace('(on wlp2s0)\n', '')
		AP[mac] = {'ss':-1, 'freq': -1}
		lastmac = mac
	elif "freq" in line:
		val = line.split(": ")[1].replace("\n","")
		AP[lastmac]['freq'] = int(val);
	elif "signal" in line:
		val = line.split(": ")[1].replace(" dBm\n", '')
		AP[lastmac]['ss'] = float(val)


FREQ={}
for mac in AP:
	freq = AP[mac]["freq"]
	if not freq in FREQ:
		FREQ[freq] = []
	FREQ[freq].append((mac, AP[mac]["ss"]))


unsorted = []
for mhz in FREQ:
	apcount = len(FREQ[mhz])
	b = FREQ[mhz]
	FREQ[mhz] = {}
	FREQ[mhz]['count'] = apcount
	FREQ[mhz]['data'] = b
	OBJ = {}
	OBJ['count'] = apcount
	OBJ['freq'] = mhz
	unsorted.append(OBJ)

newlist = sorted(unsorted, key=lambda x: x['count'], reverse=True)
sum = 0
for item in newlist:
	print item['count'] , item['freq']
	sum += int(item['count'])

#print "--------"
#print sum, "APs"
