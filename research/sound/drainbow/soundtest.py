import pyaudio
import socket

CHUNK = 1024
FORMAT = pyaudio.paInt32
CHANNELS = 1
RATE = 44100

SERVER = ('10.3.10.100', 21353)

pa = pyaudio.PyAudio()

stream = pa.open(	format = FORMAT,
			channels = CHANNELS,
			rate = RATE,
			input = True,
			frames_per_buffer = CHUNK,
			)


sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


while True:
	data = stream.read(CHUNK)
	sock.sendto(data, SERVER)
