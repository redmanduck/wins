import json
import sys
import math
#
# Scale pseudo coordinate
# to scale coordinate
# 
# usage: ./scale.py map <seg1.csv> <seg2.csv> <seg3.csv> 
# creates map.csv

filename = sys.argv[1]
f =  open(filename, "r")
pres = json.loads(f.read())

def FCD(C):
	# fake coordinate world distance
	# takes cooridor

	first = C['points'][0]
	last = C['points'][len(C['points']) - 1]

	return math.sqrt( (first[0] - last[0])**2 + (first[1] - last[1])**2 )

dx = -1
dx_corridor = -1

for corridor in pres:
	pts = pres[corridor]['points']
	
	#sort by position
	pts.sort(key=lambda x: x[0], reverse=False)
	pts.sort(key=lambda x: x[1], reverse=False)

	p = pres[corridor]['norm']

	if(dx == -1):
		dx = p
		dx_corridor = corridor
	if p < dx:
		dx = p
		dx_corridor = corridor

csv =[]
for corridor in pres:
	
	pts = pres[corridor]['points']

	current_factor = FCD(pres[corridor])/FCD(pres[dx_corridor])
	multiplier  = pres[corridor]['norm']/dx

	#print "wanted", multiplier
	#print "current", current_factor
	
	multiplier = multiplier/current_factor  # effective multiplier

	#print "multip", corridor, multiplier
	for i in range(0,len(pts)):
		if(pts[0][0] - pts[1][0] == 0):
			pts[i][1] = pts[i][1]*multiplier 
		else:
			pts[i][0] = pts[i][0]*multiplier

		csv.append(pts[i])


# print as csv

for coord in csv:
	print coord[0], "," , coord[1]