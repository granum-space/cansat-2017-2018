#!/usr/bin/python3

from init_env import init_env
init_env()


from pymavlink import mavutil

connection = mavutil.mavlink_connection('udpin:localhost:11000')
mav = connection

while (True):
    msg = mav.recv_match(blocking=True)
    print("Message from %d: %s" % (msg.get_srcSystem(), msg))
