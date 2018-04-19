import sys
import logging
import time
import json

from pymavlink import mavutil

from .config import get_config
from .redis_store import redis_store

_log = logging.getLogger(__name__)
_config = get_config()
_putback = _config["MAV_PLOT_DATA_PBACK"]


def update_zset(set_name, message):
    p = redis_store.pipeline()

    # кастуем соообщение в json
    # Добавим в сообщение метку времени для уникальности, так как в zrange можно добавлять только уникальные метки
    # Так же, по времени будем строить ось y наших данных в виде "стоймости" элемента zset-а
    timestamp = int(round(time.time() * 1000))

    dmsg = message.to_dict()
    dmsg["gnd_timestamp"] = timestamp
    jmsg = json.dumps(dmsg)
    p.zadd(set_name, timestamp, jmsg)

    # Теперь удаляем из zrange все что старше, чем позволяет наши pback из конфига
    deadline = timestamp - _putback.total_seconds() * 1000
    p.zremrangebyscore(set_name, 0, deadline)
    p.execute()


def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    connection = mavutil.mavlink_connection('udpin:localhost:11000')
    mav = connection

    while True:
        msg = mav.recv_match(blocking=True)
        update_zset("acc", msg)
        _log.debug("got message %s", msg)


if __name__ == "__main__":
    exit(main([]))
