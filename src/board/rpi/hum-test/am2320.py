# -*- coding: utf-8 -*-
from i2cdev import I2C
from time import sleep

addr = 0x5c

def unite_bytes(msb, lsb):
	return (msb << 8) | lsb

class AM2320():
	def __init__(self):
		self.addr = addr
		self.i2c = I2C(bus=1)

	def open(self, addr):
		self.i2c.open(bus=1)
		self.i2c.set_addr(addr=addr)

	def close(self):
		self.i2c.close()

	def read_data(self):
		try:
			self.i2c.write(data=b'\0x00')
		except:
			pass
		sleep(0.001) # wait at least 0.8ms, at most 3ms 

		buf = []
		buf.append(0x03) # read function code 
		buf.append(0x00) # start address
		buf.append(0x04) # number of registers to read
		self.i2c.write(data=buf)
		sleep(0.0016) # wait at least 1.5ms
		'''
		0 byte: function code
		1 byte: number of bytes
		2 byte: humidity high byte
		3 byte: humidity low byte
		4 byte: temperature high byte
		5 byte: temperature low byte
		6-7 byte: CRC
		'''
		buf = self.i2c.read(nRead=8)

		humidity = unite_bytes(buf[2], buf[3])
		humidity = float(humidity/10.0)

		temperature = unite_bytes(buf[4], buf[5])
		temperature = float(temperature/10.0)

		return temperature, humidity

sensor = AM2320()
sensor.open(addr=addr)

while True:
	t, h = sensor.read_data()
	print("temp:", t, "C", "humidity:", h, "%")
	sleep(0.5)

snesor.close()





