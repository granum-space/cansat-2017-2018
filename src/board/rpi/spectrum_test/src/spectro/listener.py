import time
import sys
import io
#import cv2

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message

class Listener:
    DATA_PACKETS_COUNT = 0
    TOTAL_SIZE = 0
    WHOLE_DATA = []
    OUTPUT_DIR = ""

    def __init__(self, output_dir):
        self.OUTPUT_DIR = output_dir

    def accept_data(self, data, seqnr):
        raise NotImplemented()

    def accept_message(self, msg):
        if msg.get_type() == "DATA_TRANSMISSION_HANDSHAKE":
            self.DATA_PACKETS_COUNT = msg.packets
            self.TOTAL_SIZE = msg.size

        elif msg.get_type() == "ENCAPSULATED_DATA":
            self.accept_data(msg.data, msg.seqnr)

class PictureListener(Listener):

    def __init__(self, *args, **kwargs):
        super(PictureListener, self).__init__(*args, **kwargs)

    def accept_data(self, data, seqnr):
        self.WHOLE_DATA += data
        if seqnr == self.DATA_PACKETS_COUNT - 1: #if last package
            self.WHOLE_DATA = self.WHOLE_DATA[:self.TOTAL_SIZE]
            f = open(self.OUTPUT_DIR, mode="wb")
            f.write(bytes(self.WHOLE_DATA))
            print("picture saved.")
            self.WHOLE_DATA = []
            f.close()

class SpectrumListener(Listener):

    def __init__(self, *args, **kwargs):
        super(SpectrumListener, self).__init__(*args, **kwargs)
    
    def accept_data(self, data, seqnr):
        self.WHOLE_DATA += data
        if seqnr == self.DATA_PACKETS_COUNT - 1: #if last package
            self.WHOLE_DATA = self.WHOLE_DATA[:self.TOTAL_SIZE]
            self.WHOLE_DATA = []
            with open(self.OUTPUT_DIR, mode="w", newline="") as ostream:
                fieldnames = ["nm", "intensity"]
                writer = csv.DictWriter(ostream, fieldnames)
                writer.writeheader()

                i = 0
                for intensity in data:
                    nm = i
                    intensity = intensity
                    row = {"nm": nm, "intensity": intensity}

                    writer.writerow(row)
                    i += 1

def main():
    connection = mavutil.mavlink_connection("udpin:0.0.0.0:11000")
    picture_listener = PictureListener("picture.png")
    spectrum_listener = SpectrumListener("spectrum.png")

    while True:
        msg = connection.recv_match(blocking=True)
        picture_listener.accept_message(msg=msg)

if __name__ == "__main__":
    main()