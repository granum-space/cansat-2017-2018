from flask import Blueprint, Response, current_app

import socket
import sys

sound = Blueprint('sound', __name__, url_prefix="/sound")

@sound.route('/')
def sound_func():
    root_path = current_app.root_path
    tcp_port = current_app.config['SOUND_TCP_PORT']

    def generate(root_path, tcp_port):
        headername = root_path + '/static/header.wav'
        with open(headername, "rb") as header:
            yield header.read(44) #Sending only a header

        provider = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        provider.connect(('localhost', tcp_port))
        data = provider.recv(1024 * 4)
        while True:
            yield data

            try:
                data = provider.recv(1024 * 4)
            except:
                break

        provider.close()


    return Response(generate(root_path, tcp_port), mimetype="audio/x-wav")