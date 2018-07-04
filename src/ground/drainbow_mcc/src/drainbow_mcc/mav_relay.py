import logging
import sys
import time
from datetime import datetime

from .common.config import get_config
from pymavlink.mavutil import mavlink_connection


_log = logging.getLogger(__name__)


def relay_main():
    config = get_config()
    log_level = config["MAV_RELAY_LOG_LEVEL"]

    logging.basicConfig(stream=sys.stdout, level=log_level)
    _log.info("Запускаюсь")

    mav_url_in = config["MAV_RELAY_LISTEN_URL"]
    rcv_timeout = config["MAV_RELAY_RECEIVE_TIMEOUT"]
    report_period = config["MAV_RELAY_REPORT_PERIOD"]

    _log.info("Использую вход на %s", mav_url_in)
    in_connection = mavlink_connection(mav_url_in)

    now = datetime.utcnow().isoformat()
    logfile = "/opt/logs/%s_mavrelay.mav" % now
    in_connection.setup_logfile(logfile)
    _log.error("mavlog setted up %s" % logfile)


    last_report_time = time.time()
    msg_counter = 0
    while True:
        msg = in_connection.recv_match(blocking=True, timeout=rcv_timeout)
        if msg:
            _log.debug("msg: %s", msg)
            msg_counter += 1

        now = time.time()
        if now - last_report_time > report_period:
            _log.info("всего сообщений %d", msg_counter)


if __name__ == "__main__":
    relay_main()
