import time
import sys
import logging

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message
from . import _log
from .am2320 import AM2320
from .mavsender import MavSender
log = _log.getChild(__name__)
log.debug("i am alive") 

logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)
log.debug("I am in main")

addr = 0x5c
am2320 = AM2320()
am2320.open(addr=addr)

mavsender = MavSender(url="udpout:172.16.164.203:11000", source_system=2, source_component=7)

while True:
	t, h = am2320.read_data()
	mavsender.send_data(data=[t, h])
	print("message sent")
	time.sleep(2)

am2320.close()
