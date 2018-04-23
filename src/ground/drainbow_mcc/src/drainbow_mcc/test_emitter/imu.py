import random
import logging
import time
from datetime import timedelta
from pymavlink import mavutil

_log = logging.getLogger(__name__)


def now():
    return int(round(time.time()*1000))


def random_scaled_imu_test(url: str, pause: timedelta):
    connection = mavutil.mavlink_connection(url)
    mav = connection.mav

    _log.info("Запускаю генерацию случайных данных БИНС")
    _log.info("параметры: %s, %s" % (url, pause))
    boot_time = now()
    datagen = lambda: int(round(random.uniform(0, 1)*9.8*3))

    while True:
        msg = mav.scaled_imu_encode(
            time_boot_ms=now() - boot_time,
            xacc=datagen(),
            yacc=datagen(),
            zacc=datagen(),
            xgyro=datagen(),
            ygyro=datagen(),
            zgyro=datagen(),
            xmag=datagen(),
            ymag=datagen(),
            zmag=datagen(),
        )

        _log.debug(msg)
        mav.send(msg)
        time.sleep(pause.total_seconds())

