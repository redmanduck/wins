import RPi.GPIO as GPIO
from time import sleep
GPIO.setmode(GPIO.BOARD)

def translate(ch):
	DB = [7,11,13,15,12,16,18,22]
	print ("char: %x" %(ch)),
	for i in range(8):
		if ch%2:
			GPIO.output(DB[i], GPIO.HIGH)
			print (" %d " %(1)),
		else:
			GPIO.output(DB[i], GPIO.HIGH)
			print (" %d " % (0)),
		ch=ch>>1
	print ""

def comm_out(ch):
	CS1 = 40
	A0 = 37
	WR = 36
	GPIO.output(CS1, GPIO.HIGH)
	GPIO.output(A0, GPIO.LOW)
	sleep(0.1)
	GPIO.output(WR, GPIO.LOW)
	translate(ch)
	sleep(0.1)
	GPIO.output(WR, GPIO.HIGH)
	GPIO.output(CS1, GPIO.HIGH)
	sleep(0.5)

def data_out(ch):
	CS1 = 40
	A0 = 37
	WR = 36
	GPIO.output(CS1, GPIO.HIGH)
	GPIO.output(A0, GPIO.HIGH)
	sleep(0.1)
	GPIO.output(WR, GPIO.LOW)
	translate(ch)
	sleep(0.1)
	GPIO.output(WR, GPIO.HIGH)
	GPIO.output(CS1, GPIO.HIGH)
	sleep(0.5)

def init():
	print "Start"
	CS1 = 40
	RES = 38
	A0 = 37
	WR = 36
	RD = 35
	DB0 = 7
	DB1 = 11
	DB2 = 13
	DB3 = 15
	DB4 = 12
	DB5 = 16
	DB6 = 18
	DB7 = 22
	GPIO.setup(CS1, GPIO.OUT)
	GPIO.output(CS1, GPIO.HIGH)
	GPIO.setup(RES, GPIO.OUT)
	GPIO.output(RES, GPIO.HIGH)
	GPIO.setup(A0, GPIO.OUT)
	GPIO.output(A0, GPIO.LOW)
	GPIO.setup(WR, GPIO.OUT)
	GPIO.output(WR, GPIO.LOW)
	GPIO.setup(RD, GPIO.OUT)
	GPIO.output(RD, GPIO.LOW)
	GPIO.setup(DB0, GPIO.OUT)
	GPIO.output(DB0, GPIO.LOW)
	GPIO.setup(DB1, GPIO.OUT)
	GPIO.output(DB1, GPIO.LOW)
	GPIO.setup(DB2, GPIO.OUT)
	GPIO.output(DB2, GPIO.LOW)
	GPIO.setup(DB3, GPIO.OUT)
	GPIO.output(DB3, GPIO.LOW)
	GPIO.setup(DB4, GPIO.OUT)
	GPIO.output(DB4, GPIO.LOW)
	GPIO.setup(DB5, GPIO.OUT)
	GPIO.output(DB5, GPIO.LOW)
	GPIO.setup(DB6, GPIO.OUT)
	GPIO.output(DB6, GPIO.LOW)
	GPIO.setup(DB7, GPIO.OUT)
	GPIO.output(DB7, GPIO.LOW)
	sleep(1)

	GPIO.output(RD, GPIO.HIGH)
	GPIO.output(WR, GPIO.HIGH)
	GPIO.output(CS1, GPIO.LOW)
	GPIO.output(RES, GPIO.HIGH)
	GPIO.output(RES, GPIO.LOW)
	sleep(0.2)
	GPIO.output(RES, GPIO.HIGH)
	sleep(0.2)
	comm_out(0xA2)
	comm_out(0xA0)
	comm_out(0xC8)
	comm_out(0xC0)
	comm_out(0x40)
	comm_out(0x25)
	comm_out(0x81)
	comm_out(0x19)
	comm_out(0x2F)
	comm_out(0xAF)

def closePorts():
  GPIO.cleanup()
  print "Finished"

def main():
	comm_out(0xA4)
	comm_out(0xE0)
	sleep(3)

init()
main()
closePorts()

