import RPi.GPIO as GPIO
import time
import os
import threading 

GPIO.setmode(GPIO.BCM)

GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_DOWN) 
GPIO.setup(20, GPIO.OUT)

GPIO.setup(16, GPIO.OUT)
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

GPIO.output(20, True)

SCAN = 1
STOP = 2

mode = SCAN
nextmode = mode
delim=False
def setmode(newmode):
	global mode
	mode = newmode
	if newmode == STOP:
		return

def monitorbutton():
	global nextmode
	global mode
	global delim
	while True:
		time.sleep(1)
		if GPIO.input(4):
			nextmode = STOP
		else:
			nextmode = SCAN
		
		if mode == STOP and nextmode == SCAN:
			print "using delimiter"
			delim=True
		print "updated next mode to", nextmode
		setmode(nextmode)


thr = threading.Thread(target=monitorbutton, args=(), kwargs={})
thr.start()

ts = "R" + str(time.time())
filename =  str(ts) + ".txt"
print "Saving into ", filename
while True:
	time.sleep(1)
	print "mode is", mode
	if not mode == STOP:
		os.system("echo --------------------- >> scanresult/" + filename)
		if delim:
			os.system("echo ========== [FLAGGED] ============= >> scanresult/" + filename)
			delim = False
		
		GPIO.output(16, True)
		os.system("./wifiscan | grep = >> scanresult/" + filename);
		GPIO.output(16, False)

GPIO.cleanup()
thr.join()
