import time
import sys
import logging

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message
from . import _log
from .am2320_lib import AM2320
from .mavsender import MavSender

log = _log.getChild(__name__)

print("I am in main")
logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

addr = 0x5c
sensor = AM2320()
sensor.open(addr=addr)

mavsender = MavSender(url="udpout:172.16.164.203:11000", source_system=2, source_component=7)

while True:
	t, h = sensor.read_data()
	mavsender.send_data(data=[t, h])
	log.debug("message sent")
	time.sleep(1)

sensor.close()
