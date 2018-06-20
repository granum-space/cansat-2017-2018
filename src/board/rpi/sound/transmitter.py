import pyaudio
import socket

FORMAT = pyaudio.paInt32 #FIXME завести общие настройки
CHANNELS = 1
RATE = 44100
CHUNK = 1024

SERVER = '10.3.10.100'
PORT = 23576

audio = pyaudio.PyAudio()

stream = audio.open(format=FORMAT, channels=CHANNELS,
                rate=RATE, input=True,
                frames_per_buffer=CHUNK)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
	data = stream.read(CHUNK)
	sock.sendto(data, (SERVER, PORT))