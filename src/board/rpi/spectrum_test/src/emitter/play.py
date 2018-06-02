import logging
import time
from pymavlink import mavutil

_log = logging.getLogger(__name__)


def get_msg_time(msg, zerotime):
    """ Переводит бортовое время сообщения в текущее
        С учетом того, что когда на борту было 0, на земле было (какбудто) zerotime
        и с учетом того, что настоящее время тикает в дробных секундах
    """

    if hasattr(msg, "time_boot_ms"):
        return msg.time_boot_ms / 1000 + zerotime
    elif hasattr(msg, "time_usec"):
        # return msg.time_usec / 10**6 + zerotime
        return 0  # TODO: тут чет не то
    else:
        _log.error("cant get time for message %s", msg)
        return 0


def play_file(url, filepath, **mavlogfile_kwargs):
    source_mav = mavutil.mavlogfile(filepath, **mavlogfile_kwargs)
    target_con = mavutil.mavlink_connection(url)
    target_mav = target_con.mav

    _log.info("Играю файл %s", filepath)
    _log.info("Отпарвляю пакеты на url %s", url)

    zero_time = time.time()
    while True:
        msg = source_mav.recv_msg()
        if not msg:
            break

        play_time = get_msg_time(msg, zero_time)
        sleep_amount = play_time - time.time()

        _log.debug(msg)
        _log.debug("sleep amount: %s", sleep_amount)

        if sleep_amount > 0:
            time.sleep(sleep_amount)

        target_mav.send(msg)
