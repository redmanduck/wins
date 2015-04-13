import json
import sys
#
# Scale pseudo coordinate
# to scale coordinate
# 
# usage: ./scale.py map <seg1.csv> <seg2.csv> <seg3.csv> 
# creates map.csv

filename = sys.argv[1]
f =  open(filename, "r")
pres = json.loads(f.read())

dx = -1
for corridor in pres:
	pts = pres[corridor]['points']
	L = pres[corridor]['norm']
	p = L/(len(pts)-1)
	if(dx == -1):
		dx = p
	if p < dx:
		dx = p

csv =[]
for corridor in pres:
	pts = pres[corridor]['points']
	
	#sort by position
	pts.sort(key=lambda x: x[0], reverse=False)
	pts.sort(key=lambda x: x[1], reverse=False)


	gap = 1
	multiplier  = gap/dx
	pred = pts[0]
	csv.append(pred)
	for i in range(1,len(pts)):
		if(pred[0] - pts[i][0] == 0):
			pts[i][1] = gap*multiplier +  pred[1]
		else:
			pts[i][0] = gap*multiplier +  pred[0]

		csv.append(pts[i])
		pred = pts[i]


# print as csv

for coord in csv:
	print coord[0], "," , coord[1]