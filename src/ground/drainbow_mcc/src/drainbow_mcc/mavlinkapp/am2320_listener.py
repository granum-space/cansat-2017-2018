import time
import sys
import io
import logging
import csv

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message


class Listener:
    def __init__(self, T_output_dir, H_output_dir):
        self.T_output_dir = T_output_dir
        self.H_output_dir = H_output_dir

        self.T_ostream = open(self.T_output_dir, mode="w", newline="")
        fieldnames = ["temperature", "time"]
        self.T_writer = csv.DictWriter(self.T_ostream, fieldnames)
        self.T_writer.writeheader()

        self.H_ostream = open(self.H_output_dir, mode="w", newline="")
        fieldnames = ["humidity", "time"]
        self.H_writer = csv.DictWriter(self.H_ostream, fieldnames)
        self.H_writer.writeheader()

    def save_data(self):
        row = {"temperature": self.temperature, "time": self.time}
        self.T_writer.writerow(row)

        row = {"humidity": self.humidity, "time": self.time}
        self.H_writer.writerow(row)

    def accept_message(self, msg):
        if msg.get_type() == "AM2320":
            self.temperature = float(msg.temperature / 10.0)
            self.humidity = float(msg.humidity / 10.0)
            self.time = msg.time_boot_ms


def main():
    connection = mavutil.mavlink_connection("udpin:0.0.0.0:11000")
    listener = Listener("temperature.csv", "humidity.csv")

    while True:
        msg = connection.recv_match(blocking=True)
        print(msg)
        listener.accept_message(msg)
        listener.save_data()


if __name__ == "__main__":
    main()
