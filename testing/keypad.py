import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)


def alloff():
	for i in range(21, 19):
		GPIO.output(i, False)

GPIO.setup(21, GPIO.IN) 
GPIO.setup(20, GPIO.IN) 
GPIO.setup(19, GPIO.IN) 
GPIO.setup(18, GPIO.IN) 

GPIO.setup(17, GPIO.OUT) 
GPIO.setup(16, GPIO.OUT) 
GPIO.setup(15, GPIO.OUT) 
GPIO.setup(14, GPIO.OUT)

#for i in range(21, 19):
#	alloff()
GPIO.output(17, GPIO.HIGH)
GPIO.output(16, GPIO.HIGH)
GPIO.output(15, GPIO.HIGH)
GPIO.output(14, GPIO.HIGH)

while True:
	#probe all
	time.sleep(0.1)
	if GPIO.input(21):
		print "row 1"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(20):
		print "row 2"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(19):
		print "row 3"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(18):
		print "row 4"
#		if(input):
#			print "Button pressed at GPIO ", i

GPIO.cleanup()
