import time
from pymavlink import mavutil 

class MavSender:

    def __init__(self, url, source_system, source_component):
        self.connection = mavutil.mavlink_connection(url, source_system=source_system, source_component=source_component)
        self.mav = self.connection.mav

    # data[0] - temperature
    # data[1] - humidity

    def send_data(self, data):
        self.mav.am2320_send(
            time_boot_ms=int(time.time()*1000),
            temperature=data[0],
            humidity=data[1]
        )
        
