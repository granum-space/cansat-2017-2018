from flask import Blueprint, Response, current_app, copy_current_request_context, _request_ctx_stack, request

import socket
import time

sound = Blueprint('sound', __name__, url_prefix="/sound")

@sound.route('/')
def sound_func():
    root_path = current_app.root_path
    tcp_port = current_app.config['SOUND_TCP_PORT']

    @copy_current_request_context
    def _get_client_info():
        ctx = _request_ctx_stack.top
        req = ctx.request
        env = req.environ

        a = env['REMOTE_ADDR']
        p = env['REMOTE_PORT']

        return '{}:{}'.format(a, p)

    def generate(root_path, tcp_port):
        client = _get_client_info()

        headername = root_path + '/static/header.wav'
        with open(headername, "rb") as header:
            yield header.read(44) #Sending only a header

        logname = root_path + '/tmp/sound_log'
        log = open(logname, 'a')
        log.write(time.asctime() + ' REQ ' + client + '\n')

        try:
            provider = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

            provider.connect(('localhost', tcp_port))

            log.write(time.asctime() + ' CON ' + client + '\n')
            log.close()

            provider.settimeout(1)

            while True:
                try:
                    yield provider.recv(1024 * 4)
                except socket.timeout:
                    yield b'1'

        finally:
            log = open(logname, 'a')
            log.write(time.asctime() + ' CLS ' + client + '\n')
            log.close()

            provider.close()


    return Response(generate(root_path, tcp_port), mimetype="audio/x-wav")