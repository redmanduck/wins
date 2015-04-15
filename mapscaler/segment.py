import sys, json
rawfile = sys.argv[1]

"""
 TODO: optimization
 we can segment without specifying bound

 H = {}
 for p in A:
 	H[p.x] <- p 
 	H[p.y] <- p

 for key, M in H:
	 for p in M:
	 	# all p in H are on the same line, 
	 	# may or may not be conncted
	 	for j in M:
	    	if not reachable(p, j):
	    		H.remove(j)

""" 
bnd = open(sys.argv[2], "r")
jdataa = bnd.read()
bounds  = json.loads(jdataa)
bounded = {}

# return key of bound that it satisfies
# return False if none satisfied
print "Processing segmentation for", bounds["name"]

def satisfy_any_bound(point):
	for k in bounds["data"]:
		rule = bounds["data"][k]["rule"]
		nw = rule[0]
		ne = rule[1]
		sw = rule[2]
		se = rule[3]

		cond = (point[0] >= nw[0]) and (point[1] <= nw[1]) and (point[0] >= sw[0]) and (point[1] >= sw[1]) and (point[0] <= ne[0]) and (point[0] <= se[0]) and (point[1] <= ne[1]) and (point[1] >= se[1])

		if cond:
			return k

	raise Exception("No rule satisfied")

f = open(rawfile, "r")
dat = f.read().split("\n")
for line in dat:
	coo = line.split(",")
	x = int(coo[0])
	y = int(coo[1])
	rule = satisfy_any_bound((x,y))
	if not rule in bounded:
		bounded[rule] = {
			"points": [],
			"norm": bounds["data"][rule]["dim"]
		}
	bounded[rule]['points'].append((x,y))

fo = open(sys.argv[1] + ".segs", "w")
fo.write(json.dumps(bounded))
print "Written to file", sys.argv[1] + ".segs"