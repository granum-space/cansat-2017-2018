import time
import sys
import logging
import math
#import numpy as np

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_data_transmission_handshake_message

from . import _log
from .live_stream import MavSender

log = _log.getChild(__name__)

#camera = cv2.VideoCapture(0)
setup_dict = {}

f = open('config.txt')
for line in f:
    setup_dict[line.split()[0]] = int(line.split()[2])

def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    mavsender = MavSender(url="udpout:localhost:11000", source_system=2, source_component=7)

    #i = 0
    while True:
        #intensity = []
        #img = camera.read()
        #cropped_img = get_cropped_img(img, setup_dict)

        #filename = "output%d.png"%i
        #cv2.imwrite(filename, crop_img)

        file = open('crop8.png', mode='rb')
        data = file.read()

        mavsender.send_picture(data=data, width=65, height=260)

        #intensity = get_spectrum_from_img(cropped_img)

        #i += 1
        log.debug("message sent")
        time.sleep(10)

    #camera.release()

main(sys.argv[1:])
