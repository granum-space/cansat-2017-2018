import pyaudio
import socket

CHUNK = 256
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

PORT = 21353

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('0.0.0.0', PORT))
sock.listen(0)

pa = pyaudio.PyAudio()

streamOut = pa.open(	format = FORMAT,
					channels = CHANNELS,
					rate = RATE,
					output = True,
					frames_per_buffer = CHUNK,
					)

conn, addr = sock.accept()

while True:
	#data, sender = sock.recvfrom(CHUNK * CHANNELS * 2)
	data = conn.recv(CHUNK * CHANNELS * 2)
	streamOut.write(data)