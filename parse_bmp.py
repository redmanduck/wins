from PIL import Image
from numpy import*
from BitVector import BitVector as bv
im = Image.open('FullMapEE.bmp')
L = list(im.getdata())
for l,v in enumerate(L):
	if L[l] == 255:
		L[l] = 0
	else:
		L[l] = 1

#add 5 more rows
for i in range(850):
	L.append(0)


for K in range(16):
	for i in range(170):
		# read entire columns
		buf = bv(size=8)
		for j in range(8):
			# read entire block
			buf[7-j] = int(L[i + 170*j + 170*8*K])
		print "0x" + buf.get_hex_string_from_bitvector(), ",",
	print 
