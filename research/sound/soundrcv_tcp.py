import pyaudio
import socket

CHUNK = 1024
FORMAT = pyaudio.paInt32
CHANNELS = 1
RATE = 44100

PORT = 21353

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('0.0.0.0', PORT))

pa = pyaudio.PyAudio()

streamOut = pa.open(	format = FORMAT,
					channels = CHANNELS,
					rate = RATE,
					output = True,
					frames_per_buffer = CHUNK,
					)

while True:
	data, sender = sock.recvfrom(CHUNK * CHANNELS * 2)
	streamOut.write(data)