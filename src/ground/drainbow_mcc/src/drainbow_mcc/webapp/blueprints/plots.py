import time
import json
import math
from cgi import valid_boundary

from flask import Blueprint, render_template, abort, jsonify, request, current_app
from jinja2 import TemplateNotFound

from ...common.definitions import ZSET_NAME_IMU, ZSET_NAME_PRESSURE
from ..redis_store import redis_store


def now():
    return float(round(time.time()*1000))


def viewlimit(plotname, timebase=now()):
    return timebase - current_app.config["%s_PLOT_SCOPE_MS" % plotname].total_seconds()*1000


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
    elif chart_name == "pressure":
        return _get_pressure_data()

@plots.route("/map_data")
def map_data():
    # Достаем элементы
    time = now()
    latest_update_time = float(request.args.get("latestUpdateTime"))
    latest_update_time = max(latest_update_time, viewlimit("MAP", time))

    zsetname = current_app.config["ZSET_NAME_MAP"]
    elems = redis_store.zrangebyscore(zsetname, latest_update_time, time, withscores=True, score_cast_func=int)

    data = []
    for e in elems:
        value, score = e
        value = json.loads(value.decode("utf-8"))
        data.append({
            'time_usec': value['time_usec'],
            'fix_type': value['fix_type'],
            'lat': value['lat'],
            'lon': value['lon'],
            'alt': value['alt'],
            'servertime': score
        })

    if(int(request.args.get("latestUpdateTime")) == -1):
        data = [{
                'time_usec': 1,
                'fix_type': 3,
                'lat': 35,
                'lon': 55,
                'alt': 170,
                'servertime': 15
            }, {
                'time_usec': 2,
                'fix_type': 3,
                'lat': 37,
                'lon': 55,
                'alt': 160,
                'servertime': 16
            }, {
                'time_usec': 3,
                'fix_type': 3,
                'lat': 37,
                'lon': 50,
                'alt': 70,
                'servertime': 17
            }]

    return jsonify(data)


def _get_data_abstract(plotname, yvalue_name, time=now()):
    """ Преобразует набор "мавлинкоджсоновых элементов в набор элементов точек для графика
    Элементы оси Y выбираются по указанному ключу """

    # Достаем элементы
    latest_update_time = float(request.args.get("latestUpdateTime"))
    latest_update_time = max(latest_update_time, viewlimit(plotname, time))

    zsetname = current_app.config["ZSET_NAME_%s" % plotname]
    elems = redis_store.zrangebyscore(zsetname, latest_update_time, time, withscores=True, score_cast_func=int)

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
    datax, datay, dataz = [_get_data_abstract("IMU", x, time) for x in ("xacc", "yacc", "zacc")]

    for x, y, z in zip(datax, datay, dataz):
        x["y"] /= 1000.0
        y["y"] /= 1000.0
        z["y"] /= 1000.0

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": datax[-1]["servertime"],
        "viewlimit": viewlimit("IMU", time)
    }

    return jsonify(data)


def _get_gyro_data():
    """ Датасет для графика гироскопа """
    time = now()
    datax, datay, dataz = [_get_data_abstract("IMU", x, time) for x in ("xgyro", "ygyro", "zgyro")]

    for x, y, z in zip(datax, datay, dataz):
        x["y"] /= 1000.0
        y["y"] /= 1000.0
        z["y"] /= 1000.0

    data = {
        "datas": [datax, datay, dataz],
        "latestUpdateTime": datax[-1]["servertime"],
        "viewlimit": viewlimit("IMU", time)
    }

    return jsonify(data)


def _get_temperature_data():
    time = now()
    temperature = _get_data_abstract("PRESSURE", "temperature", time)

    for record in temperature:
        record["y"] /= 100.0

    data = {
        "datas": [temperature],
        "latestUpdateTime": temperature[-1]["servertime"],
        "viewlimit": viewlimit("PRESSURE", time)
    }

    return jsonify(data)

def _get_pressure_data():
    time = now()
    pressure = _get_data_abstract("PRESSURE", "press_abs", time)

    data = {
        "datas": [pressure],
        "latestUpdateTime": pressure[-1]["servertime"],
        "viewlimit": viewlimit("PRESSURE", time)
    }

    return jsonify(data)
