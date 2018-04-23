import logging
import sys
from datetime import timedelta

from .imu import random_scaled_imu_test

TARGET_URL = 'udpout:localhost:11000'
PAUSE = timedelta(milliseconds=500)


def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    if not argv:
        print("1 - random scaled imu data")
        return 1

    typecode = int(argv[0])

    target = TARGET_URL
    if len(argv) > 1:
        target = argv[1]

    pause = PAUSE
    if len(argv) > 2:
        pause = timedelta(milliseconds=int(argv[2]))

    if typecode == 1:
        random_scaled_imu_test(url=target, pause=pause)


if __name__ == "__main__":
    import sys
    exit(main(sys.argv[1:]))

