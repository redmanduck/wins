import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

GPIO.setup(05, GPIO.IN)
GPIO.setup(06, GPIO.IN)
GPIO.setup(13, GPIO.IN)
GPIO.setup(19, GPIO.IN)

GPIO.setup(12, GPIO.OUT)
GPIO.setup(16, GPIO.OUT)
GPIO.setup(20, GPIO.OUT)
GPIO.setup(21, GPIO.OUT)

#for i in range(21, 19):
#	alloff()
GPIO.output(12, GPIO.HIGH)
GPIO.output(16, GPIO.HIGH)
GPIO.output(20, GPIO.HIGH)
GPIO.output(21, GPIO.HIGH)

while True:
	#probe all
	time.sleep(0.1)
	if GPIO.input(05):
		print "row 1"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(06):
		print "row 2"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(13):
		print "row 3"
	time.sleep(0.1)
#	time.sleep(1)
	if GPIO.input(19):
		print "row 4"
#		if(input):
#			print "Button pressed at GPIO ", i

GPIO.cleanup()
