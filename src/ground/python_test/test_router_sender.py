#!/usr/bin/python3

from init_env import init_env
init_env()

from pymavlink import mavutil

connection = mavutil.mavlink_connection('udpout:localhost:10000')
connection.mav.test_message_send(field1=1, field2=2, field3=3, field4=4)
