import logging
import sys
import time
from datetime import datetime

from pymavlink.mavutil import mavlink_connection

_log = logging.getLogger(__name__)

def defrag_main():
    log_level = logging.INFO
    logging.basicConfig(stream=sys.stdout, level=log_level)
    _log.info("Запускаюсь")

    mav_url_in = "/dev/ttyAMA0"
    mav_url_out = "udpout:granum.tk:12000"
    rcv_timeout = 1
    report_period = 5

    _log.info("Использую вход на %s и выход на %s", mav_url_in, mav_url_out)
    in_connection = mavlink_connection(mav_url_in)
    out_connection = mavlink_connection(mav_url_out)

    now = datetime.utcnow().isoformat()
    logfile = "/opt/logs/%s_mavdefrag.mav" % now
    in_connection.setup_logfile(logfile)
    _log.error("mavlog setted up %s" % logfile)

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
