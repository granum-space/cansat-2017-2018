import pyaudio
import socket

CHUNK = 1024
FORMAT = pyaudio.paInt16
CHANNELS = 1
RATE = 44100

DEVICE_INDEX = 1

SERVER = ('localhost', 21353)

pa = pyaudio.PyAudio()

stream = pa.open(	format = FORMAT,
			channels = CHANNELS,
			input_device_index = 2,
			rate = RATE,
			input = True,
			frames_per_buffer = CHUNK,
			)


sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(SERVER)

while True:
	data = stream.read(CHUNK)
	sock.send(data)
