import time
import sys
import io
import logging

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message

def Listener:
	def __init__(self, T_output_dir, H_output_dir):
		self.T_output_dir = T_output_dir
		self.H_output_dir = H_output_dir

		with open(self.T_output_dir, mode="w", newline="") as ostream:
			fieldnames = ["temperature", "time"]
			self.T_writer = csv.DictWriter(ostream, fieldnames)
			self.T_writer.writeheader()

		with open(self.H_output_dir, mode="w", newline="") as ostream:
			fieldnames = ["humidity", "time"]
			self.H_writer = csv.DictWriter(ostream, fieldnames)
			self.H_writer.writeheader()

	def accept_message(self, msg)
		if msg.get_type() == "AM2320":
			self.temperature = msg.temperature
			self.humidity = msg.humidity
			self.time = msg.time_usec

	def save_data(self):
		row = {"temperature": self.temperature, "time": self.time}
		T_writer.writerow(row)

		row = {"humidity": self.humidity, "time": self.time}
		Hwriter.writerow(row)

def main():
	connection = mavutil.mavlink_connection("udpin:0.0.0.0:11000")
	listener = Listener()

	while True:
		msg = connection.recv_match(blocking=True)
		print(msg)
		listener.accept_message(msg)
		listener.save_data()

if __name__ == "__main__":
	main()
