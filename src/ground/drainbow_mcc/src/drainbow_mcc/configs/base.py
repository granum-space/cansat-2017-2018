from datetime import timedelta

# Настройки мавлинковской службы

MAV_PLOT_DATA_PBACK = timedelta(seconds=120)
""" Интервал времени в течение которого будут хранится даннные телеметрии для отображения от "сейчас" """

# Настройки веб службы
PLOT_DATA_UPDATE_PERIOD_MS = 500
""" период обновления графиков (в мс) """

IMU_PLOT_SCOPE_MS = timedelta(seconds=30)
""" Диапазон отображения последних данных гироскопа """

REDIS_URL = "redis://:@localhost:6379/0"

