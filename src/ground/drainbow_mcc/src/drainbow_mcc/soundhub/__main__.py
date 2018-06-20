import socket
import threading

#FIXME завести общие настройки
CHANNELS = 1
RATE = 44100
CHUNK = 1024

UDP_PORT = 23576
TCP_PORT = 23578

udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_sock.bind(('0.0.0.0', UDP_PORT))

tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_sock.bind(('localhost', TCP_PORT))
tcp_sock.listen(0)

subscribers = []

def forwarder():
	while True:
		data, sender = udp_sock.recvfrom(CHUNK * CHANNELS * 2)

		for subscriber in subscribers:
			try:
				subscriber.send(data)
			except(ConnectionError):
				subscribers.remove(subscriber)

forwarderThread = threading.Thread(target=forwarder)
forwarderThread.start()

tcp_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

while True:
	conn, addr = tcp_sock.accept()
	subscribers.append(conn)