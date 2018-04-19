import time
import json

from flask import Blueprint, render_template, abort, jsonify, request, current_app
from jinja2 import TemplateNotFound

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

    # Грузим данные на 10 секунд назад
    now = int(round(time.time()*1000))
    ago = now - current_app.config["IMU_PLOT_SCOPE_MS"].total_seconds()*1000

    elems = redis_store.zrangebyscore("acc", ago, now, withscores=True, score_cast_func=int)

    datax = []
    for e in elems:
        value, score = e
        value = json.loads(value.decode("utf-8"))
        datax.append({
            "x": value["time_boot_ms"] / 1000,
            "y": value["xacc"]
        })

    dataset1 = {
        "label": "acceleration",
        "lineTension": 0,
        "data": datax,
    }

    data = {
        "datasets": [dataset1]
    }

    return jsonify(data)
