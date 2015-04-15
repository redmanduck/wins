import json
import sys
import math
#
# Scale pseudo coordinate
# to scale coordinate
# 
# usage: ./scale.py map <seg1.csv> <seg2.csv> <seg3.csv> 
# creates map.csv

if(len(sys.argv) != 3):
	print "Usage: ./scale.py <segments_file> <output_csv>"
	sys.exit(1)	

filename = sys.argv[1]
f =  open(filename, "r")
pres = json.loads(f.read())
f.close()

def corridor_length(C):

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


	print "------ ", corridor , "------ "
	print "this corr length ", corridor_length(pres[corridor])
	print "base corr length ", corridor_length(pres[dx_corridor])
	current_factor = corridor_length(pres[corridor])/corridor_length(pres[dx_corridor])
	multiplier = pres[corridor]['norm']/dx

	print "wanted", multiplier
	print "current", current_factor
	multiplier = multiplier/current_factor  # effective multiplier
	print "effective multiplier",  multiplier

	#print "multip", corridor, multiplier
	for i in range(0,len(pts)):
		if(pts[0][0] - pts[1][0] == 0):
			pts[i][1] = pts[i][1]*multiplier 
		else:
			pts[i][0] = pts[i][0]*multiplier

		csv.append(pts[i])

	print ">>> ------ (Post scaling)", corridor , "------ "
	print ">>> new corridor_length ", corridor_length(pres[corridor])

# print as csv

f = open(sys.argv[2], "w")
for coord in csv:
	g= str(str(coord[0])+  "," + str( coord[1]) + "\n")
	f.write(g)
f.close()