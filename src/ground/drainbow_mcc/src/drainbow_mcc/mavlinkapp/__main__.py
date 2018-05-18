import sys
import logging
import time
import json

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_scaled_imu_message, MAVLink_scaled_pressure_message, \
    MAVLink_hil_gps_message, MAVLink_sonar_message, MAVLink_attitude_quaternion_message

from .config import get_config
from .redis_store import redis_store

from ..common.definitions import ZSET_NAME_IMU, ZSET_NAME_PRESSURE, ZSET_NAME_DISTANCE, ZSET_NAME_MAP, ZSET_NAME_ATTITUDE

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
    if(dmsg['mavpackettype'] == 'SCALED_IMU'):
        print(dmsg.get('time_boot_ms'))
    jmsg = json.dumps(dmsg)
    p.zadd(set_name, timestamp, jmsg)
    p.execute()

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

        if isinstance(msg, MAVLink_scaled_imu_message):
            """ Сообщение с данными IMU """
            update_zset(ZSET_NAME_IMU, msg)

        elif isinstance(msg, MAVLink_scaled_pressure_message):
            """ Сообщение с данными BMP280 """
            update_zset(ZSET_NAME_PRESSURE, msg)

        elif isinstance(msg, MAVLink_sonar_message):
            """ Сообщение с данными сонара """
            update_zset(ZSET_NAME_DISTANCE, msg)

        elif isinstance(msg, MAVLink_hil_gps_message):
            """ Сообщение с данными GPS """
            update_zset(ZSET_NAME_MAP, msg)

        elif isinstance(msg, MAVLink_attitude_quaternion_message):
            """ Сообщение с данными GPS """
            update_zset(ZSET_NAME_ATTITUDE, msg)

if __name__ == "__main__":
    exit(main([]))
