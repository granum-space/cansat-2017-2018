from sys import argv

import logging
import argparse
import sys
from datetime import timedelta

from .imu import random_scaled_imu_test
from .play import play_file

TARGET_URL = 'udpout:localhost:11000'
PAUSE = timedelta(milliseconds=500)


def fake_main(args):
    typecode = args.typecode
    target = args.target_url
    pause = args.pause

    if typecode == 1:
        random_scaled_imu_test(url=target, pause=pause)


def play_main(args):
    play_file(args.target_url, args.file_path, notimestamps=True)


def main(argv):
    parser = argparse.ArgumentParser("granum data emitter", add_help=True)
    parser.add_argument("--target_url", default=TARGET_URL, nargs="?", dest="target_url")

    subparsers = parser.add_subparsers(help="mode", dest="mode")

    test_parser = subparsers.add_parser("fake")
    test_parser.add_argument("-t,--typecode", nargs="?", default=1, dest="typecode")
    test_parser.add_argument("-p,--pause", nargs="?", default=PAUSE, dest="pause")

    play_parser = subparsers.add_parser("play")
    play_parser.add_argument("-f,--file_path", required=True, dest="file_path")

    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    args = parser.parse_args(argv)
    if not args.mode:
        parser.print_help()
        return 1

    if args.mode == "fake":
        return fake_main(args)

    if args.mode == "play":
        play_main(args)


if __name__ == "__main__":
    import sys
    exit(main(sys.argv[1:]))

