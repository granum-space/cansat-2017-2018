import os

from flask import Flask

from .assets import assets
from .redis_store import redis_store

from .blueprints.landing import landing
from .blueprints.plots import plots

app = Flask(
    __name__,
    static_folder="static",
    template_folder="templates"
)

app.config.from_object(os.environ["DRAINBOW_MCC_CONFIG"])

assets.init_app(app)
redis_store.init_app(app)

app.register_blueprint(landing)
app.register_blueprint(plots)


