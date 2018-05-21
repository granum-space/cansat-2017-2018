import os
import argparse
import sys
import csv

from pymavlink import mavutil


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

    mav = mavutil.mavlink_connection(f, notimestamps=notimestamps)
    processors = {}
    while True:
        msg = mav.recv_msg()
        if not msg:
            break

        msg_id = msg.get_msgId()

        if msg_id not in processors:
            fpath = os.path.join(base_path, msg.get_type() + ".csv")
            stream = open(fpath, mode='w', newline='')
            writer = csv.DictWriter(stream, fieldnames=msg.get_fieldnames())
            writer.writeheader()
            processors.update({msg_id: writer})

        processor = processors[msg_id]
        msg_dict = msg.to_dict()
        del msg_dict["mavpackettype"]  # Совершенно излишне в нашем случае
        processor.writerow(msg_dict)

        print(msg)


if __name__ == "__main__":
    exit(main(sys.argv[1:]))
