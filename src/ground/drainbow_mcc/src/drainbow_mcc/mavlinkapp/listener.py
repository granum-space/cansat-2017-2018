import time
import sys
import io
import csv
import logging

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message

#from . import _log

#log = _log.getChild(__name__)

class PictureListener():
    DATA_PACKETS_COUNT = 0
    TOTAL_SIZE = 0
    WHOLE_DATA = []
    OUTPUT_DIR = ""
    number_of_whole_packages = 0
    state = "IDLE"

    def __init__(self, *args, **kwargs):
        pass

    def accept_data(self, data, seqnr):
        self.WHOLE_DATA += data
        if seqnr == self.DATA_PACKETS_COUNT - 1: #if last package
            self.WHOLE_DATA = self.WHOLE_DATA[:self.TOTAL_SIZE]
            self.number_of_whole_packages += 1
            self.OUTPUT_DIR = "img%d.png"%(self.number_of_whole_packages-1)
            f = open(self.OUTPUT_DIR, mode="wb")
            f.write(bytes(self.WHOLE_DATA))
            print("picture saved.")
            self.state = "IDLE"
            f.close()

class SpectrumListener():
    DATA_PACKETS_COUNT = 0
    TOTAL_SIZE = 0
    WHOLE_DATA = []
    OUTPUT_DIR = ""
    number_of_whole_packages = 0
    state = "IDLE"

    def __init__(self, *args, **kwargs):
        pass
    
    def accept_data(self, data, seqnr):
        self.WHOLE_DATA += data
        if seqnr == self.DATA_PACKETS_COUNT - 1: #if last package
            self.WHOLE_DATA = self.WHOLE_DATA[:self.TOTAL_SIZE]
            self.number_of_whole_packages += 1
            self.OUTPUT_DIR = "graph%d.csv"%(self.number_of_whole_packages-1)
            with open(self.OUTPUT_DIR, mode="w", newline="") as ostream:
                fieldnames = ["nm", "intensity"]
                writer = csv.DictWriter(ostream, fieldnames)
                writer.writeheader()

                i = 0
                for el in self.WHOLE_DATA:
                    nm = i
                    intensity = el
                    row = {"nm": nm, "intensity": intensity}
                    writer.writerow(row)
                    i += 1
            print("spectrum saved.")
            self.state = "IDLE"


class SpectrumAggregator:

    def __init__(self):
        self.picture_listener = PictureListener("img.png")
        self.spectrum_listener = SpectrumListener("graph.csv")
        self.picture_index_we_wait_for = 0
        self.spectrum_index_we_wait_for = 0

    def accept_message(self, msg):
        if self.picture_listener.state == "IDLE": #ждем заголовка
            if msg.get_type() == "SPECTRUM_PICTURE": #получили заголовок картинки
                self.picture_listener.WHOLE_DATA = []
                self.picture_index_we_wait_for = 0
                self.picture_listener.DATA_PACKETS_COUNT = msg.packets
                self.picture_listener.TOTAL_SIZE = msg.size
                self.picture_listener.state = "ACCUM"
    
        elif self.picture_listener.state == "ACCUM": #ждем данных
            if msg.get_type() == "ENCAPSULATED_DATA":
                seqnr = msg.seqnr
                if seqnr < self.picture_index_we_wait_for:
                    pass
                elif seqnr > self.picture_index_we_wait_for:
                    self.picture_listener.state = "IDLE"
                elif seqnr == self.picture_index_we_wait_for:
                    self.picture_listener.accept_data(data=msg.data, seqnr=seqnr)
                    self.picture_index_we_wait_for += 1

        if self.spectrum_listener.state == "IDLE": #ждем заголовка
            if msg.get_type() == "SPECTRUM_INTENSITY_ARRAY": #получили заголовок спектра
                self.spectrum_listener.WHOLE_DATA = []
                self.spectrum_index_we_wait_for = 0
                self.spectrum_listener.DATA_PACKETS_COUNT = msg.packets
                self.spectrum_listener.TOTAL_SIZE = msg.size
                self.spectrum_listener.state = "ACCUM"
    
        elif self.spectrum_listener.state == "ACCUM": #ждем данных
            if msg.get_type() == "SPECTRUM_ENCAPSULATED_DATA":
                seqnr = msg.seqnr
                if seqnr < self.spectrum_index_we_wait_for:
                    pass
                elif seqnr > self.spectrum_index_we_wait_for:
                    self.spectrum_listener.state = "IDLE"
                elif seqnr == self.spectrum_index_we_wait_for:
                    self.spectrum_listener.accept_data(data=msg.data, seqnr=seqnr)
                    self.spectrum_index_we_wait_for += 1

def main():
    connection = mavutil.mavlink_connection("udpin:0.0.0.0:11000")
    spectrumAggregator = SpectrumAggregator()

    while True:
        msg = connection.recv_match(blocking=True)
        print(msg)
        spectrumAggregator.accept_message(msg)

if __name__ == "__main__":
    main()
