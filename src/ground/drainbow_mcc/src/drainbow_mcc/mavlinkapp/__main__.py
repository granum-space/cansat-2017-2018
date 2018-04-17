import sys
import logging

from sqlalchemy import create_engine
from .config import get_config
from pymavlink import mavutil

_log = logging.getLogger(__name__)
_config = get_config()


def _build_sql_engine():
    return create_engine(_config["SQLALCHEMY_DATABASE_URI"])


def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    connection = mavutil.mavlink_connection('udpin:localhost:11000')
    mav = connection

    while True:
        msg = mav.recv_match(blocking=True)
        _log.info("Message from %d: %s" % (msg.get_srcSystem(), msg))


if __name__ == "__main__":
    exit(main([]))
