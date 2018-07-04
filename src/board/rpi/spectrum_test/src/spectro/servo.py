from time import sleep
from . import i2cdev
import sys

addr = 0x09
in_min = 0
in_max = 180
out_min = 1600
out_max = 5000

up = 148
down = 178

def map(x, in_min, in_max, out_min, out_max):
	if x > in_max:
		return out_max
	elif x < in_min:
		return out_min
	else:
		signal = (x - in_min)*(out_max - out_min)/(in_max - in_min) + out_min
		signal = int(signal)
		return signal

class Servo:
	def __init__(self):
		self.i2c = i2cdev.I2C(bus=1)
		self.addr = addr
		self.i2c.set_addr(addr=self.addr)

	def set_servo(self, position):
		signal = map(position, in_min, in_max, out_min, out_max)
		b1 = signal & 0xFF
		b2 = (signal >> 8) & 0xFF
		self.i2c.write(data=[b2, b1])

	def open(self):
		self.i2c.open(bus=1)

	def close(self):
		self.i2c.close()

if __name__ == '__main__':
	servo = Servo()
	servo.open()
	servo.set_servo(10)
	#while True:
	#	servo.set_servo(up)
	#	print(up)
	#	sleep(2)
	#	servo.set_servo(down)
	#	print(down)
	#	sleep(2)
