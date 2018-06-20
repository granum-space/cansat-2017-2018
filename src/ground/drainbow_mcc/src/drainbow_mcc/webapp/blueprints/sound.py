from flask import Blueprint, Response, current_app

import socket

sound = Blueprint('sound', __name__, url_prefix="/sound")

TCP_PORT = 23578 #в конфиг

@sound.route('/')
def sound_func():
    root_path = current_app.root_path

    def generate(root_path):
        headername = root_path + '/static/header.wav'
        with open(headername, "rb") as header:
            yield header.read(44) #Sending only a header

        provider = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        provider.connect(('localhost', TCP_PORT))
        data = provider.recv(1024)
        while data:
            yield data
            data = provider.recv(1024)

    return Response(generate(root_path), mimetype="audio/x-wav")