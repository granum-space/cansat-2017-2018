import sys
import logging
import time
import json

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_scaled_mpu6000_message

from .config import get_config
from .redis_store import redis_store

from ..common.definitions import ZSET_NAME_MPU6000

_log = logging.getLogger(__name__)
_config = get_config()
_putback = _config["MAV_PLOT_DATA_PBACK"]


def update_zset(set_name, message):
    p = redis_store.pipeline()

    # кастуем соообщение в json
    # Добавим в сообщение метку времени для уникальности, так как в zrange можно добавлять только уникальные метки
    # Так же, по времени будем строить ось x наших данных в виде "стоймости" элемента zset-а
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
    logging.basicConfig(stream=sys.stdout, level=_config["LOG_LEVEL"])

    _log.info("Запускаюсь. Слушаю url: %s" % _config["MAV_LISTEN_URL"])
    connection = mavutil.mavlink_connection(_config["MAV_LISTEN_URL"])
    mav = connection

    while True:
        msg = mav.recv_match(blocking=True)
        _log.debug("got message %s", msg)

        if isinstance(msg, MAVLink_scaled_mpu6000_message):
            """ Сообщение с данными MPU6000 """
            update_zset(ZSET_NAME_MPU6000, msg)


if __name__ == "__main__":
    exit(main([]))
