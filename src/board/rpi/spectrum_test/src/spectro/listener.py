import time
import sys

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message

def main():

    connection = mavutil.mavlink_connection("udpin:0.0.0.0:11000")
    
    while True:
        msg = connection.recv_match(blocking=True)
        print(msg)

if __name__ == "__main__":
    main()