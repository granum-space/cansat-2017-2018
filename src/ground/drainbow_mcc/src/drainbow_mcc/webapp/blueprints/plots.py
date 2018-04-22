import time
import json

from flask import Blueprint, render_template, abort, jsonify, request, current_app
from jinja2 import TemplateNotFound

from ...common.definitions import ZSET_NAME_IMU
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


def _imu_now_and_then():
    """ Вовзращает лимиты графика по времени для IMU на текущий момент """
    now = round(time.time()*1000)
    then = now - current_app.config["IMU_PLOT_SCOPE_MS"].total_seconds()*1000
    return int(then), int(now)


def _get_imu_data_abstract(yvalue_name):
    """ Преобразует набор "мавлинкоджсоновых элементов в набор элементов точек для графика
    Элементы оси Y выбираются по указанному ключу """

    # определеяем диапазон на который будем отображать данные
    then, now = _imu_now_and_then()

    # Достаем элементы
    elems = redis_store.zrangebyscore(ZSET_NAME_IMU, then, now, withscores=True, score_cast_func=int)

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

    datax, datay, dataz = [_get_imu_data_abstract(x) for x in ("xacc", "yacc", "zacc")]

    data = {
        "datasets": [
            {
                "label": "Угловая скорость по X (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#f44242",
                "borderWidth": 2,
                "lineTension": 0,
                "data": datax,
            },
            {
                "label": "Угловая скорость по Y (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#15ff00",
                "borderWidth": 2,
                "lineTension": 0,
                "data": datay,
            },
            {
                "label": "Угловая скорость по Z (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#001dff",
                "borderWidth": 2,
                "lineTension": 0,
                "data": dataz,
            },
        ]
    }

    return jsonify(data)


def _get_gyro_data():
    """ Датасет для графика гироскопа """

    datax, datay, dataz = [_get_imu_data_abstract(x) for x in ("xgyro", "ygyro", "zgyro")]

    data = {
        "datasets": [
            {
                "label": "Угловая скорость по X (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#f44242",
                "borderWidth": 2,
                "lineTension": 0,
                "data": datax,
            },
            {
                "label": "Угловая скорость по Y (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#15ff00",
                "borderWidth": 2,
                "lineTension": 0,
                "data": datay,
            },
            {
                "label": "Угловая скорость по Z (град/с)",
                "fill": False,
                "pointRadius": 0,
                "borderColor": "#001dff",
                "borderWidth": 2,
                "lineTension": 0,
                "data": dataz,
            },

        ]
    }

    return jsonify(data)
