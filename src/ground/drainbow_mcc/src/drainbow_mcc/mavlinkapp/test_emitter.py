from pymavlink import mavutil

connection = mavutil.mavlink_connection('udpout:localhost:11000')
connection.mav.test_message_send(field1=1, field2=2, field3=3, field4=4)
