import math
import time 
from pymavlink import mavutil 
#import cv2
#camera = cv2.VideoCapture(0)
setup_dict = {}

f = open('config.txt')
for line in f:
    setup_dict[line.split()[0]] = int(line.split()[2])

'''def get_cropped_img(img, setup_dict):
    width = setup_dict['width']
    height = setup_dict['height']
    x_upleft = setup_dict['x_upleft']
    y_upleft = setup_dict['y_upleft']

    y_min = int(y_upleft)
    y_max = int(y_upleft + height)
    x_min = int(x_upleft)
    x_max = int(x_upleft + width)

    img = cv2.cvtColor(img, cv2.COLOR_BGR2YUV)
    y_component, u_component, v_component = cv2.split(img)
    cropped_img = y_component[y_min:y_max, x_min:x_max]

    return cropped_img

def get_spectrum_from_img(cropped_img):
    intensity = []

    height = cropped_img.shape[0]
    width = cropped_img.shape[1]

    i = height-1
    while i >= 0:
        j = width-1
        summ = 0
        while j >= 0:
            summ += cropped_img[i, j]
            j -= 1
        intensity.append(summ)
        x = i
        y = summ
        
        i -= 1

    return intensity'''

class MavSender:
    BYTES_PER_PACKET = 253

    def __init__(self, url, source_system, source_component):
        self.connection = mavutil.mavlink_connection(url, source_system=source_system, source_component=source_component)
        self.mav = self.connection.mav

    def send_picture(self, data, width, height):
        packets = math.ceil(len(data) / self.BYTES_PER_PACKET)
        self.mav.data_transmission_handshake_send(
            type=0,
            size=len(data),
            width=width,
            height=height,
            packets=packets,
            payload=self.BYTES_PER_PACKET,
            jpg_quality=100
        )
        for i in range(packets-1):
            self.mav.encapsulated_data_send(
                seqnr=i, 
                data=data[self.BYTES_PER_PACKET*i:self.BYTES_PER_PACKET*(i+1)], 
                force_mavlink1=False
            )
        i = packets - 1
        last_data = data[self.BYTES_PER_PACKET*(i):]
        if len(last_data) != 0:
            zero_count = self.BYTES_PER_PACKET - len(last_data)
            last_data += bytes('\x00'*(zero_count), encoding = 'utf-8')
            self.mav.encapsulated_data_send(
                seqnr=i, 
                data=last_data, 
                force_mavlink1=False
            )

'''
for i in range(5):
    intensity = []
    return_value, img = camera.read()
    cropped_img = get_cropped_img(img, setup_dict)

    filename = "output%d.png"%i
    cv2.imwrite(filename, crop_img)
    #print(filename)

    file = open(filename, mode='rb')
    data = file.read()

    intensity = get_spectrum_from_img(cropped_img)
    print("intensity number ", i+1)
    print(intensity)

    time.sleep(1)

camera.release()
'''
