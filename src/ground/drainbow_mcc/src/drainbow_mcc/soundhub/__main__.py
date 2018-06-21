import socket
import threading

from .config import get_config

_config = get_config()

#FIXME завести общие настройки


udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
udp_sock.bind(('0.0.0.0', _config['SOUND_UDP_PORT']))

tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
tcp_sock.bind(('localhost', _config['SOUND_TCP_PORT']))
tcp_sock.listen(0)

subscribers = []

def forwarder():
	while True:
		data, sender = udp_sock.recvfrom(_config['SOUND_CHUNK'] * _config['SOUND_CHANNELS'] * 4)

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