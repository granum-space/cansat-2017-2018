import time

ZSET_NAME_MPU6000 = "granum:imu"
""" Имя редисового zset, в котором хранятся сообщения MPU6000 """

def now():
    return round(time.time()*1000)

def viewlimit(plotname):
    return now() - current_app.config["%s_PLOT_SCOPE_MS" % plotname].total_seconds()*1000