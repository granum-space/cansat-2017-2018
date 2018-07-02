import sys
import logging
import time
import json
import os

from pymavlink import mavutil
from pymavlink.dialects.v20.granum import MAVLink_scaled_imu_message, MAVLink_scaled_pressure_message, \
    MAVLink_am2320_message, MAVLink_luminosity_message, MAVLink_granum_message_message, \
    MAVLink_hil_gps_message, MAVLink_sonar_message, MAVLink_attitude_quaternion_message, \
    MAVLink_picture_header_message, MAVLink_intensity_header_message, MAVLink_encapsulated_data_message, \
    MAVLink_intensity_encapsulated_data_message

from .redis_store import redis_store

from ..common.config import get_config
from ..common.definitions import ZSET_NAME_IMU, ZSET_NAME_PRESSURE, ZSET_NAME_DISTANCE, ZSET_NAME_HUMIDITY, \
    ZSET_NAME_LUMINOSITY, ZSET_NAME_MESSAGES, ZSET_NAME_MAP, ZSET_NAME_ATTITUDE, ZSET_NAME_SPECTRUM


from .spectrum import SpectrumAggregator, SpectrumAcceptor, PictureSaver

_log = logging.getLogger(__name__)
_config = get_config()
_putback = _config["MAV_PLOT_DATA_PBACK"]


def update_zset(set_name, message):
    p = redis_store.pipeline()

    # кастуем соообщение в json
    # Добавим в сообщение метку времени для уникальности, так как в zrange можно добавлять только уникальные метки
    # Так же, по времени будем строить ось x наших данных в виде "стоимости" элемента zset-а
    timestamp = int(round(time.time() * 1000))

    dmsg = message.to_dict()
    jmsg = json.dumps(dmsg)

    if(dmsg['mavpackettype'] == 'SONAR'): print(dmsg['time_boot_ms'])

    p.zadd(set_name, timestamp, jmsg)
    p.execute()

    # Теперь удаляем из zrange все что старше, чем позволяет наши pback из конфига
    deadline = timestamp - _putback.total_seconds() * 1000
    p.zremrangebyscore(set_name, 0, deadline)
    p.execute()


class SpectrumRedisSaver(SpectrumAcceptor):
    def __init__(self):
        super().__init__()

    def accept(self, data, identifier):
        dictioned = {'identifier': identifier, 'data': data}

        class Dummy:
            def to_dict(self): return dictioned

        update_zset(ZSET_NAME_SPECTRUM, Dummy())


def main(argv):
    logging.basicConfig(stream=sys.stdout, level=_config["MAV_LOG_LEVEL"])

    _log.info("Запускаюсь. Слушаю url: %s" % _config["MAV_LISTEN_URL"])
    connection = mavutil.mavlink_connection(_config["MAV_LISTEN_URL"])
    mav = connection

    mavlinkapp_path = os.path.dirname(os.path.abspath(__file__)) # Path to ...../drainbow_mcc/mavlinkapp/
    drainbow_mcc_path = os.path.dirname(mavlinkapp_path)
    filename_template = drainbow_mcc_path + '/webapp/tmp/img/%d.png'

    spectrum_aggregator = SpectrumAggregator(spectrum_acceptor=SpectrumRedisSaver(),
                                            picture_acceptor=PictureSaver(filename_template=filename_template))

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

        elif isinstance(msg, MAVLink_am2320_message):
            """ Сообщение с данными am2320 """
            update_zset(ZSET_NAME_HUMIDITY, msg)

        elif isinstance(msg, MAVLink_luminosity_message):
            """ Сообщение с данными tsl2561 """
            update_zset(ZSET_NAME_LUMINOSITY, msg)

        elif isinstance(msg, MAVLink_granum_message_message):
            """ Информационные сообщения """
            update_zset(ZSET_NAME_MESSAGES, msg)

        elif isinstance(msg, MAVLink_hil_gps_message):
            """ Сообщение с данными GPS """
            update_zset(ZSET_NAME_MAP, msg)

        elif isinstance(msg, MAVLink_attitude_quaternion_message):
            """ Сообщение с данными о вращении """
            update_zset(ZSET_NAME_ATTITUDE, msg)

        elif isinstance(msg, MAVLink_picture_header_message) \
            or isinstance(msg, MAVLink_intensity_header_message) \
            or isinstance(msg, MAVLink_encapsulated_data_message) \
            or isinstance(msg, MAVLink_intensity_encapsulated_data_message):
            """ Сообщения, относящиеся к спектру """
            spectrum_aggregator.accept_message(msg)


if __name__ == "__main__":
    exit(main([]))
