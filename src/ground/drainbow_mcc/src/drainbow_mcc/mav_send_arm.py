import os
import argparse
import sys
import csv

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_granum_command_message

if __name__ == "__main__":
    mav = mavutil.mavlink_connection("udpout:10.3.10.1:20000", source_system=3, source_component=1)
    msg = MAVLink_granum_command_message(42)
    mav.mav.send(msg)
    print("hello?")