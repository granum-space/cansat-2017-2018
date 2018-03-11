#!/usr/bin/python3

import os
import sys

def init_env():
    """
    Функция устанавливает параметры среды и PYTHONPATH чтобы питон находил мавлинк и использовал его правильный диапект
    Функцию нужно вызывать ДО всех импортов связанных с мавликом
    """
    sys.path.append(os.path.abspath('../../common/mavlink'))

    os.environ['MAVLINK20'] = "true"
    os.environ['MAVLINK_DIALECT'] = 'granum'
