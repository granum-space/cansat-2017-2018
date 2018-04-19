from datetime import timedelta

from .imu import random_scaled_imu_test

TARGET_URL = 'udpout:localhost:11000'


def main(argv):
    if not argv:
        print("1 - random scaled imu data")
        return 1

    typecode = int(argv[0])

    pause = timedelta(milliseconds=500)
    if len(argv) > 1:
        pause = timedelta(milliseconds=int(argv[1]))

    if typecode == 1:
        random_scaled_imu_test(url=TARGET_URL, pause=pause)


if __name__ == "__main__":
    import sys
    exit(main(sys.argv[1:]))

