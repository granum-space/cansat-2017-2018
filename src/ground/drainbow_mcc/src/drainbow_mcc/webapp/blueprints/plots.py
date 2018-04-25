import time
import json

from flask import Blueprint, render_template, abort, jsonify, request, current_app
from jinja2 import TemplateNotFound

from ...common.definitions import ZSET_NAME_MPU6000, ZSET_NAME_TEMPERATURE
from ...common.definitions import now, viewlimit
from ..redis_store import redis_store


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


def _get_last_timestamp(plotname):
    elems = redis_store.zrangebyscore("ZSET_NAME_%s" % plotname, 0, now(), start=-1, num=1, withscores=True, score_cast_func=int)
    return elems[-1][1]


def _get_mpu6000_data_abstract(yvalue_name):
    """ Преобразует набор "мавлинкоджсоновых элементов в набор элементов точек для графика
    Элементы оси Y выбираются по указанному ключу """

    # Достаем элементы
    elems = redis_store.zrangebyscore(ZSET_NAME_MPU6000, int(request.args.get("latestUpdateTime")), now(), withscores=True, score_cast_func=int)

    data = []
    for e in elems:
        value, score = e
        value = json.loads(value.decode("utf-8"))
        data.append({
            "x": value["time_boot_ms"] / 1000,  # будем показывать секунды, считаем что бортовое время в мс
            "y": value[yvalue_name]
        })

    return data


def _get_acc_data():
    """ Датасет для графика акселерометра """

    datax, datay, dataz = [_get_mpu6000_data_abstract(x) for x in ("xacc", "yacc", "zacc")]

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": _get_last_timestamp("MPU6000"),
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)


def _get_gyro_data():
    """ Датасет для графика гироскопа """

    datax, datay, dataz = [_get_mpu6000_data_abstract(x) for x in ("xgyro", "ygyro", "zgyro")]

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": _get_last_timestamp("MPU6000"),
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)


def _get_temperature_data():
    temperature = _get_mpu6000_data_abstract("temperature")
    temperature = [temperature[i] / 1000.0 for i in range(0, len(temperature))]

    data = {
        "datas": [temperature],
        "latestUpdateTime": _get_last_timestamp("MPU6000"),
        "viewlimit": viewlimit("MPU6000")
    }

    return jsonify(data)
