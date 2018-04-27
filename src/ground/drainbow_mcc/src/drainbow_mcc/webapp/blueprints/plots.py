import time
import json
import math

from flask import Blueprint, render_template, abort, jsonify, request, current_app
from jinja2 import TemplateNotFound

from ...common.definitions import ZSET_NAME_MPU6000
from ..redis_store import redis_store


def now():
    return float(round(time.time()*1000))


def viewlimit(plotname):
    return now() - current_app.config["%s_PLOT_SCOPE_MS" % plotname].total_seconds()*1000


plots = Blueprint('plots', __name__, url_prefix="/plots")


@plots.route("/")
def plots_index():
    try:
        return render_template('plots.html')
    except TemplateNotFound:
        abort(404)


@plots.route("/plot_data")
def plot_data():
    chart_name = request.args.get("chartName")

    if chart_name == "acc":
        return _get_acc_data()
    elif chart_name == "gyro":
        return _get_gyro_data()
    elif chart_name == "temperature":
        return _get_temperature_data()


def _get_mpu6000_data_abstract(yvalue_name, time):
    """ Преобразует набор "мавлинкоджсоновых элементов в набор элементов точек для графика
    Элементы оси Y выбираются по указанному ключу """

    time = min(time, viewlimit("MPU6000"))

    # Достаем элементы
    latest_update_time = float(request.args.get("latestUpdateTime"))
    elems = redis_store.zrangebyscore(ZSET_NAME_MPU6000, latest_update_time, time, withscores=True, score_cast_func=int)

    data = []
    for e in elems:
        value, score = e
        value = json.loads(value.decode("utf-8"))
        data.append({
            "x": value["time_boot_ms"] / 1000,  # будем показывать секунды, считаем что бортовое время в мс
            "y": value[yvalue_name],
            "servertime": score
        })

    return data


def _get_acc_data():
    """ Датасет для графика акселерометра """
    time = now()
    datax, datay, dataz = [_get_mpu6000_data_abstract(x, time) for x in ("xacc", "yacc", "zacc")]

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": datax[-1]["servertime"],
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)


def _get_gyro_data():
    """ Датасет для графика гироскопа """
    time = now()
    datax, datay, dataz = [_get_mpu6000_data_abstract(x, time) for x in ("xgyro", "ygyro", "zgyro")]

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": datax[-1]["servertime"],
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)


def _get_temperature_data():
    time = now()
    temperature = _get_mpu6000_data_abstract("temperature", time)
    #temperature = [temperature[i] / 1000.0 for i in range(0, len(temperature))]

    data = {
        "datas": [temperature],
        "latestUpdateTime": temperature[-1]["servertime"],
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)
