from flask import Blueprint, render_template, abort
from jinja2 import TemplateNotFound

landing = Blueprint('landing', __name__, url_prefix="/")


@landing.route('/')
def plots_index():
    try:
        return render_template('landing.html')
    except TemplateNotFound:
        abort(404)
