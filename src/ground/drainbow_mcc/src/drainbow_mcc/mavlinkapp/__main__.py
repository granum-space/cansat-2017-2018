import sys
import logging
import time

from pymavlink import mavutil

from .config import get_config
from .redis_store import redis_store

_log = logging.getLogger(__name__)
_config = get_config()


def main(argv):
    logging.basicConfig(stream=sys.stdout, level=logging.DEBUG)

    connection = mavutil.mavlink_connection('udpin:localhost:11000')
    mav = connection

    while True:
        msg = mav.recv_match(blocking=True)
        _log.info("Message from %d: %s" % (msg.get_srcSystem(), msg))
        jmsg = msg.to_json()
        _log.info(jmsg)

        pipeline = redis_store.pipeline()
        the_time = time.time() * 1000
        _log.info(the_time)
        pipeline.zadd("test_pipe", time.time() * 1000, jmsg)
        pipeline.execute()


if __name__ == "__main__":
    exit(main([]))
