import logging
import sys
import time

from .common.config import get_config
from pymavlink.mavutil import mavlink_connection


_log = logging.getLogger(__name__)


def defrag_main():
    config = get_config()
    log_level = config["MAV_DEFRAG_LOG_LEVEL"]

    logging.basicConfig(stream=sys.stdout, level=log_level)
    _log.info("Запускаюсь")

    mav_url_in = config["MAV_DEFRAG_LISTEN_URL"]
    mav_url_out = config["MAV_LISTEN_URL"].replace("in", "out")
    rcv_timeout = config["MAV_DEFRAG_RECEIVE_TIMEOUT"]
    report_period = config["MAV_DEFRAG_REPORT_PERIOD"]

    _log.info("Использую вход на %s и выход на %s", mav_url_in, mav_url_out)
    in_connection = mavlink_connection(mav_url_in)
    out_connection = mavlink_connection(mav_url_out)

    last_report_time = time.time()
    msg_counter = 0
    while True:
        msg = in_connection.recv_match(blocking=True, timeout=rcv_timeout)
        if msg:
            _log.debug("msg: %s", msg)
            out_connection.mav.send(msg)
            msg_counter += 1

        now = time.time()
        if now - last_report_time > report_period:
            _log.info("всего сообщений %d", msg_counter)


if __name__ == "__main__":
    defrag_main()
