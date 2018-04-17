from flask import Blueprint, render_template, abort
from jinja2 import TemplateNotFound
from flask import jsonify

plots = Blueprint('plots', __name__, url_prefix="/plots")


@plots.route("/")
def plots_index():
    try:
        return render_template('plots.html')
    except TemplateNotFound:
        abort(404)


@plots.route("/plot_data")
def plot_data():

    dataset1 = {
        "label": "acceleration",
        "lineTension": 0,
        "data": [
            {"x": 0, "y": 1},
            {"x": 1, "y": 2},
            {"x": 2, "y": 3},
            {"x": 3, "y": 2},
            {"x": 4, "y": 1},
            {"x": 5, "y": 0},
        ],
    }

    data = {
        "datasets": [dataset1]
    }

    return jsonify(data)
