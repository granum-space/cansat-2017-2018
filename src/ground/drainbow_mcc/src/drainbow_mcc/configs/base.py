import logging
from datetime import timedelta

# Общие настройки
LOG_LEVEL = logging.ERROR
""" уровень логгирования """

# Настройки мавлинковской службы
# ==================================================

MAV_PLOT_DATA_PBACK = timedelta(seconds=120)
""" Интервал времени в течение которого будут хранится даннные телеметрии для отображения от "сейчас" """

MAV_LISTEN_URL = 'udpin:0.0.0.0:11000'
""" Адрес для mavutil, на котором служба будет слушать сообщения """

# Настройки веб службы
# ==================================================
PLOT_DATA_UPDATE_PERIOD_MS = 500
""" период обновления графиков (в мс) """

IMU_PLOT_SCOPE_MS = timedelta(seconds=15)
""" Диапазон отображения на графике последних данных MPU6000 """
ZSET_NAME_IMU = "granum:imu"
""" Имя редисового zset, в котором хранятся сообщения SCALED_IMU """

PRESSURE_PLOT_SCOPE_MS = timedelta(seconds=15)
""" Диапазон отображения на графике последних данных MPU6000 """
ZSET_NAME_PRESSURE = "granum:pressure"
""" Имя редисового zset, в котором хранятся сообщения SCALES_PRESSURE """

REDIS_URL = "redis://:@localhost:6379/0"

