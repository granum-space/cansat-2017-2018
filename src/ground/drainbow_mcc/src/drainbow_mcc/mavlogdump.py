import os
import argparse
import sys
import csv

from pymavlink import mavutil
from .spectro_listener import SpectrumAggregator

def main(argv):
    parser = argparse.ArgumentParser("tm parser to csvs", add_help=True)
    parser.add_argument("-i,--input", nargs="?", dest="input", required=True)
    parser.add_argument("-o,--output_dir", nargs="?", dest="output_dir", default=".")
    parser.add_argument("--notimestamps", nargs="?", dest="notimestamps", default=True)
    args = parser.parse_args(argv)

    base_path = args.output_dir
    f = args.input
    notimestamps = args.notimestamps

    if not os.path.isdir(base_path):
        os.makedirs(base_path, exist_ok=True)

    spectrumAggregator = SpectrumAggregator()
    mav = mavutil.mavlink_connection(f, notimestamps=notimestamps)
    processors = {}
    msg_dict = {}
    mega_msg_id = 768
    time_boot_ms = 0
    servo = 0
    i = 0
    b = True
    while True:
        msg = mav.recv_msg()
        if not msg:
            break
        msg_id = msg.get_msgId()

        if msg_id not in processors and b:
            if msg_id == 153 or msg_id == 154: # INTENSITY_HEADER or INTENSITY_ENCAPSULATED_DATA
                fpath = os.path.join(base_path, "mega.csv")
                stream = open(fpath, mode='w', newline='')
                writer = csv.DictWriter(stream, fieldnames=["number", "time_boot_ms", "servo", "data"])
                writer.writeheader()
                processors.update({mega_msg_id: writer})
                b = False
            else:
                fpath = os.path.join(base_path, msg.get_type() + ".csv")
                stream = open(fpath, mode='w', newline='')
                writer = csv.DictWriter(stream, fieldnames=msg.get_fieldnames())
                writer.writeheader()
                processors.update({msg_id: writer})

        if msg_id == 153 or msg_id == 154: # INTENSITY_HEADER or INTENSITY_ENCAPSULATED_DATA
            spectrumAggregator.accept_message(msg)
            if msg_id == 153:
                time_boot_ms = msg.time_boot_ms
                servo = msg.servo
            elif msg_id == 154:
                intensity = []
                if spectrumAggregator.data_ready == True:
                    print("ready")
                    spectrumAggregator.data_ready = False
                    intensity = spectrumAggregator.WHOLE_DATA
                    print(len(intensity))
                    msg_dict = {"number": i, "time_boot_ms": time_boot_ms, "servo": servo, "data": intensity}
                    processor = processors[mega_msg_id] # открываем соответствующий writer
                    processor.writerow(msg_dict)
                    i += 1
        else:
            processor = processors[msg_id]
            msg_dict = msg.to_dict()
            if "mavpackettype" in msg_dict:
                del msg_dict["mavpackettype"]  # Совершенно излишне в нашем случае
            processor.writerow(msg_dict)


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
